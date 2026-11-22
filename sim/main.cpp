// Terminal simulator: runs the portable core with a terminal display, terminal keyboard and FakeBackend.
// DemoUi is a placeholder so the simulator does something; the real UI lives in ui/.
//
// Usage: build/sim [--cols N] [--rows N] [--ascii]
//   Defaults mimic the Spelling Ace: 16 columns, 1 row, uppercase-only 14-segment character set.

#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include "backends/fake/FakeBackend.h"
#include "core/KeyMapper.h"
#include "core/MessageStore.h"
#include "core/TextViewport.h"
#include "sim/TerminalDisplay.h"
#include "sim/TerminalKeySource.h"

using namespace mesh;

namespace {

volatile std::sig_atomic_t quitRequested = 0;
void handleInterrupt(int) { quitRequested = 1; }

struct SimOptions {
  int columns = 16;
  int rows = 1;
  bool asciiFont = false;
};

SimOptions parseOptions(int argc, char **argv) {
  SimOptions options;
  for (int index = 1; index < argc; index++) {
    if (std::strcmp(argv[index], "--cols") == 0 && index + 1 < argc) {
      options.columns = std::atoi(argv[++index]);
    } else if (std::strcmp(argv[index], "--rows") == 0 && index + 1 < argc) {
      options.rows = std::atoi(argv[++index]);
    } else if (std::strcmp(argv[index], "--ascii") == 0) {
      options.asciiFont = true;
    }
  }
  if (options.columns < 4 || options.columns > 80) options.columns = 16;
  if (options.rows < 1 || options.rows > 10) options.rows = 1;
  return options;
}

// Stores backend events and remembers which conversations received messages since the last check.
class DemoListener : public MessageStoreSink {
public:
  using MessageStoreSink::MessageStoreSink;

  void onConversation(const std::string &conversationId, ConversationKind kind, const std::string &title) override {
    MessageStoreSink::onConversation(conversationId, kind, title);
    changed = true;
  }
  void onMessage(const Message &message) override {
    MessageStoreSink::onMessage(message);
    changedConversationIds.push_back(message.conversationId);
    changed = true;
  }
  void onDeliveryState(uint32_t messageId, DeliveryState state) override {
    MessageStoreSink::onDeliveryState(messageId, state);
    changed = true;
  }

  bool changed = false;
  std::vector<std::string> changedConversationIds;
};

// Minimal placeholder UI: read messages one at a time, page through long ones, type and send.
class DemoUi {
public:
  DemoUi(MessageStore &store, IMessagingBackend &backend, TerminalDisplay &display)
      : store(store), backend(backend), display(display),
        viewport(display.caps().columns, display.caps().rows) {}

  bool quit = false;

  void handleKey(const KeyEvent &event) {
    if (!event.pressed) return;
    notice.clear();
    switch (event.key) {
      case LogicalKey::Character:
        if (draft.size() < backend.caps().maxTextLength) {
          draft += event.character;
          composing = true;
        } else {
          notice = "LIMIT REACHED";
        }
        break;
      case LogicalKey::Back:
        if (!draft.empty()) draft.pop_back();
        if (draft.empty()) composing = false;
        break;
      case LogicalKey::Clear:
        draft.clear();
        composing = false;
        break;
      case LogicalKey::Enter:
        sendDraft();
        break;
      case LogicalKey::Up:
        if (!composing && !viewport.previousPage() && messageIndex > 0) {
          showMessage(messageIndex - 1);
          viewport.lastPage();
        }
        break;
      case LogicalKey::Down:
        if (!composing && !viewport.nextPage()) {
          Conversation *conversation = currentConversation();
          if (conversation && messageIndex + 1 < conversation->messages.size()) showMessage(messageIndex + 1);
        }
        break;
      case LogicalKey::Left:
      case LogicalKey::Right:
        switchConversation(event.key == LogicalKey::Right ? 1 : -1);
        break;
      case LogicalKey::Menu:
        quit = true;
        break;
      default:
        break;
    }
  }

  void onMessagesArrived(const std::vector<std::string> &conversationIds) {
    Conversation *conversation = currentConversation();
    for (const std::string &conversationId : conversationIds) {
      if (conversation && conversationId == conversation->conversationId) {
        showNewest();
      } else if (Conversation *other = store.find(conversationId)) {
        notice = "NEW IN " + other->title;
      }
    }
  }

  void render() {
    if (composing) {
      TextViewport composeView(display.caps().columns, display.caps().rows);
      composeView.setText(draft + "_");
      composeView.lastPage();
      copyToDisplay(composeView);
    } else {
      refreshMessageText();
      copyToDisplay(viewport);
    }
    display.setStatus(statusText());
    display.flush();
  }

private:
  Conversation *currentConversation() {
    if (store.conversations().empty()) return nullptr;
    conversationIndex %= store.conversations().size();
    return store.find(store.conversations()[conversationIndex].conversationId);
  }

  void switchConversation(int direction) {
    size_t count = store.conversations().size();
    if (count == 0) return;
    conversationIndex = (conversationIndex + count + direction) % count;
    showNewest();
  }

  void showNewest() {
    Conversation *conversation = currentConversation();
    showMessage(conversation && !conversation->messages.empty() ? conversation->messages.size() - 1 : 0);
  }

  void showMessage(size_t index) {
    messageIndex = index;
    viewport.setText(currentMessageText());
    Conversation *conversation = currentConversation();
    if (conversation && messageIndex < conversation->messages.size()) {
      conversation->messages[messageIndex].read = true;
    }
  }

  // Re-wraps when the text changed (e.g. a delivery mark), keeping the page when possible.
  void refreshMessageText() {
    std::string text = currentMessageText();
    if (text == viewport.text()) return;
    size_t page = viewport.pageIndex();
    viewport.setText(text);
    while (viewport.pageIndex() < page && viewport.nextPage()) {
    }
  }

  std::string currentMessageText() {
    Conversation *conversation = currentConversation();
    if (!conversation) return "NO CONVERSATIONS";
    if (conversation->messages.empty()) return conversation->title + ": NO MESSAGES";
    if (messageIndex >= conversation->messages.size()) messageIndex = conversation->messages.size() - 1;
    const Message &message = conversation->messages[messageIndex];
    std::string sender = message.outgoing ? "ME" : store.displayNameFor(message.senderId);
    return sender + ": " + message.text + deliveryMark(message);
  }

  static std::string deliveryMark(const Message &message) {
    if (!message.outgoing) return "";
    switch (message.state) {
      case DeliveryState::Pending: return " [..]";
      case DeliveryState::Sent: return " [>]";
      case DeliveryState::Delivered: return " [OK]";
      case DeliveryState::Failed: return " [FAIL]";
      default: return "";
    }
  }

  void sendDraft() {
    Conversation *conversation = currentConversation();
    if (!composing || draft.empty() || !conversation) return;
    if (backend.sendText(conversation->conversationId, draft) == 0) {
      notice = "SEND FAILED";
      return;
    }
    draft.clear();
    composing = false;
  }

  void copyToDisplay(const TextViewport &source) {
    for (uint8_t row = 0; row < display.caps().rows; row++) display.setLine(row, source.visibleLine(row).c_str());
  }

  std::string statusText() {
    Conversation *conversation = currentConversation();
    char summary[160];
    if (composing) {
      std::snprintf(summary, sizeof(summary), "Composing  %zu/%zu chars", draft.size(), backend.caps().maxTextLength);
    } else {
      size_t messageCount = conversation ? conversation->messages.size() : 0;
      std::snprintf(summary, sizeof(summary), "[%s] message %zu/%zu  page %zu/%zu  unread %zu  backend %s",
                    conversation ? conversation->title.c_str() : "-", messageCount ? messageIndex + 1 : 0,
                    messageCount, viewport.pageIndex() + 1, viewport.pageCount(), store.totalUnread(),
                    backend.caps().protocolName);
    }
    std::string status = summary;
    if (!notice.empty()) status += "  ** " + notice;
    status +=
        "\n\nType to compose, Enter send, Backspace delete, Tab clear\n"
        "Up/Down page or older/newer message, Left/Right conversation, Esc or Ctrl-C quit";
    return status;
  }

  MessageStore &store;
  IMessagingBackend &backend;
  TerminalDisplay &display;
  TextViewport viewport;
  size_t conversationIndex = 0;
  size_t messageIndex = 0;
  bool composing = false;
  std::string draft;
  std::string notice;
};

}  // namespace

int main(int argc, char **argv) {
  SimOptions options = parseOptions(argc, argv);
  DisplayCaps caps;
  caps.columns = static_cast<uint8_t>(options.columns);
  caps.rows = static_cast<uint8_t>(options.rows);
  caps.technology = options.asciiFont ? DisplayTechnology::CharacterCell : DisplayTechnology::Segment14;
  caps.characterSet = options.asciiFont ? CharacterSet::Ascii : CharacterSet::UppercaseOnly;

  TerminalDisplay display(caps);
  TerminalKeySource keySource;
  KeyMapper keyMapper(terminalKeymap, terminalKeymapSize, true);
  MessageStore store;
  DemoListener listener(store);
  FakeBackend backend;
  backend.setListener(&listener);

  std::signal(SIGINT, handleInterrupt);
  keySource.begin();
  backend.begin();
  DemoUi ui(store, backend, display);

  const auto startTime = std::chrono::steady_clock::now();
  bool needsRender = true;
  while (!quitRequested && !ui.quit) {
    auto elapsed = std::chrono::steady_clock::now() - startTime;
    uint32_t nowMs = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
    backend.poll(nowMs);
    if (listener.changed) {
      ui.onMessagesArrived(listener.changedConversationIds);
      listener.changedConversationIds.clear();
      listener.changed = false;
      needsRender = true;
    }
    RawKey rawKey;
    while (keySource.poll(rawKey)) {
      ui.handleKey(keyMapper.map(rawKey));
      needsRender = true;
    }
    if (needsRender) {
      ui.render();
      needsRender = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  keySource.restoreTerminal();
  std::printf("\n");
  return 0;
}
