// Host-side checks for the portable core. Run with `make test`.

#include <cstdio>
#include <string>

#include "backends/fake/FakeBackend.h"
#include "core/FontSystem.h"
#include "core/KeyMapper.h"
#include "core/MessageStore.h"
#include "core/TextViewport.h"

using namespace mesh;

namespace {

int failureCount = 0;
int checkCount = 0;

void check(bool condition, const char *description) {
  checkCount++;
  if (!condition) {
    failureCount++;
    std::printf("FAIL: %s\n", description);
  }
}

void testWrapAndPaging() {
  TextViewport viewport(16, 1);
  viewport.setText("HELLO WORLD THIS IS A TEST");
  check(viewport.lineCount() == 2, "wraps into two 16-column lines");
  check(viewport.visibleLine(0) == "HELLO WORLD THIS", "first line holds whole words");
  check(!viewport.fits(), "two lines do not fit one row");
  check(viewport.nextPage(), "can page forward");
  check(viewport.visibleLine(0) == "IS A TEST       ", "second page padded to width");
  check(!viewport.nextPage(), "stops at last page");

  viewport.setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  check(viewport.lineCount() == 2, "long word is hard-split");
  check(viewport.visibleLine(0) == "ABCDEFGHIJKLMNOP", "hard split at column limit");

  viewport.setText("ONE\nTWO");
  check(viewport.lineCount() == 2, "newline forces a break");

  check(TextViewport::fits("SHORT", 16, 1), "short text fits");
  check(!TextViewport::fits("THIS TEXT IS LONGER THAN ONE LINE", 16, 1), "long text does not fit");
}

void testMarquee() {
  TextViewport viewport(4, 1);
  viewport.setText("ABCDEF");
  check(viewport.marqueeLength() == 9, "marquee length includes gap");
  check(viewport.marqueeWindow(0) == "ABCD", "marquee window at start");
  check(viewport.marqueeWindow(4) == "EF  ", "marquee window runs into gap");
  check(viewport.marqueeWindow(9) == "ABCD", "marquee wraps around");
}

void testFontFolding() {
  check(FontSystem::foldText("Hi {x}", CharacterSet::UppercaseOnly) == "HI (X)", "uppercase folding");
  check(FontSystem::foldText("Hi", CharacterSet::Ascii) == "Hi", "ascii keeps lowercase");
  check(FontSystem::fold('\x01', CharacterSet::Ascii) == FontSystem::unsupportedCharacter, "control char replaced");
}

void testKeyMapper() {
  const KeymapEntry table[] = {{0x23, LogicalKey::Enter, 0}, {0x05, LogicalKey::Character, 'Q'}};
  KeyMapper mapper(table, 2, false);
  check(mapper.map({0x23, true}).key == LogicalKey::Enter, "table entry maps to logical key");
  KeyEvent characterEvent = mapper.map({0x05, true});
  check(characterEvent.key == LogicalKey::Character && characterEvent.character == 'Q', "table maps character");
  check(mapper.map({'a', true}).key == LogicalKey::None, "unmapped code ignored without passthrough");
  KeyMapper passthroughMapper(table, 2, true);
  check(passthroughMapper.map({'a', true}).character == 'a', "printable passthrough");
}

void testMessageStore() {
  MessageStore store(3);
  store.conversationFor("ch:0", ConversationKind::Channel, "PRIMARY");
  for (uint32_t messageId = 1; messageId <= 5; messageId++) {
    Message message;
    message.messageId = messageId;
    message.conversationId = "ch:0";
    message.text = "M";
    store.addMessage(message);
  }
  Conversation *conversation = store.find("ch:0");
  check(conversation && conversation->messages.size() == 3, "trims to capacity");
  check(conversation && conversation->messages.front().messageId == 3, "drops oldest first");
  check(store.totalUnread() == 3, "counts unread");
  store.markRead("ch:0");
  check(store.totalUnread() == 0, "mark read");
  check(store.displayNameFor(0xABCD) == "!0000abcd", "unknown node shows hex ID");
}

void testFakeBackend() {
  MessageStore store;
  MessageStoreSink sink(store);
  FakeBackend backend;
  backend.setListener(&sink);
  check(backend.begin(), "fake backend starts");
  backend.poll(0);
  uint32_t messageId = backend.sendText("dm:1001", "hello");
  check(messageId != 0, "send accepted");
  check(backend.sendText("dm:1001", std::string(500, 'x')) == 0, "over-length send rejected");
  backend.poll(2000);
  Conversation *conversation = store.find("dm:1001");
  bool delivered = false;
  bool echoed = false;
  if (conversation) {
    for (const Message &message : conversation->messages) {
      if (message.messageId == messageId && message.state == DeliveryState::Delivered) delivered = true;
      if (message.text == "Echo: hello") echoed = true;
    }
  }
  check(delivered, "direct message marked delivered");
  check(echoed, "echo reply received");
}

}  // namespace

int main() {
  testWrapAndPaging();
  testMarquee();
  testFontFolding();
  testKeyMapper();
  testMessageStore();
  testFakeBackend();
  std::printf("%d checks, %d failed\n", checkCount, failureCount);
  return failureCount == 0 ? 0 : 1;
}
