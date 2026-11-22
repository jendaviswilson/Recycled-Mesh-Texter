#include "FakeBackend.h"

namespace mesh {

namespace {
const char *const channelId = "ch:0";
const char *const aliceId = "dm:1001";
const char *const chatterLines[] = {
    "Anyone copy? Testing from the ridge.",
    "Solar node back online, battery at 80%",
    "Coffee at the usual place, 10am",
    "This is a deliberately long message to exercise wrapping, paging and marquee scrolling on tiny displays.",
};
const size_t chatterLineCount = sizeof(chatterLines) / sizeof(chatterLines[0]);
const uint32_t chatterIntervalMs = 20000;
const uint32_t deliveryDelayMs = 800;
const uint32_t replyDelayMs = 1500;
}  // namespace

FakeBackend::FakeBackend() {
  backendCaps.protocolName = "Fake";
  backendCaps.maxTextLength = 200;
  backendCaps.splitsLongMessages = false;
  backendCaps.deliveryAcks = true;
  backendCaps.rooms = false;
}

bool FakeBackend::begin() {
  if (!listener) return false;
  listener->onContact({aliceNodeId, "ALICE", "Alice Example"});
  listener->onContact({bobNodeId, "BOB", "Bob Example"});
  listener->onConversation(channelId, ConversationKind::Channel, "PRIMARY");
  listener->onConversation(aliceId, ConversationKind::Direct, "ALICE");
  scheduleIncoming(1000, aliceId, aliceNodeId, "Hi! Type a reply and press Enter.");
  return true;
}

void FakeBackend::poll(uint32_t nowMs) {
  currentMs = nowMs;
  if (!listener) return;

  std::vector<ScheduledEvent> dueEvents;
  for (auto event = scheduledEvents.begin(); event != scheduledEvents.end();) {
    if (event->dueMs <= nowMs) {
      dueEvents.push_back(*event);
      event = scheduledEvents.erase(event);
    } else {
      ++event;
    }
  }
  for (ScheduledEvent &event : dueEvents) {
    if (event.isDeliveryUpdate) {
      listener->onDeliveryState(event.messageId, event.deliveryState);
    } else {
      event.message.timestampSeconds = nowMs / 1000;
      listener->onMessage(event.message);
    }
  }

  if (nowMs >= nextChatterMs) {
    nextChatterMs = nowMs + chatterIntervalMs;
    listener->onMessage(makeIncoming(channelId, bobNodeId, chatterLines[chatterIndex]));
    chatterIndex = (chatterIndex + 1) % chatterLineCount;
  }
}

uint32_t FakeBackend::sendText(const std::string &conversationId, const std::string &text) {
  if (!listener || text.empty() || text.size() > backendCaps.maxTextLength) return 0;

  bool isDirect = conversationId.rfind("dm:", 0) == 0;
  Message outgoing;
  outgoing.messageId = nextMessageId++;
  outgoing.conversationId = conversationId;
  outgoing.senderId = localNodeId;
  outgoing.text = text;
  outgoing.timestampSeconds = currentMs / 1000;
  outgoing.outgoing = true;
  outgoing.read = true;
  outgoing.state = isDirect ? DeliveryState::Pending : DeliveryState::Sent;
  listener->onMessage(outgoing);

  if (isDirect) {
    scheduledEvents.push_back({currentMs + deliveryDelayMs, true, outgoing.messageId, DeliveryState::Delivered, {}});
    scheduleIncoming(replyDelayMs, conversationId, aliceNodeId, "Echo: " + text);
  } else {
    scheduleIncoming(replyDelayMs, conversationId, bobNodeId, "Heard you: " + text);
  }
  return outgoing.messageId;
}

Message FakeBackend::makeIncoming(const std::string &conversationId, uint32_t senderId, const std::string &text) {
  Message message;
  message.messageId = nextMessageId++;
  message.conversationId = conversationId;
  message.senderId = senderId;
  message.text = text;
  message.timestampSeconds = currentMs / 1000;
  message.state = DeliveryState::Received;
  return message;
}

void FakeBackend::scheduleIncoming(uint32_t delayMs, const std::string &conversationId, uint32_t senderId,
                                   const std::string &text) {
  scheduledEvents.push_back({currentMs + delayMs, false, 0, DeliveryState::Received,
                             makeIncoming(conversationId, senderId, text)});
}

}  // namespace mesh
