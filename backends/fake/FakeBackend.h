#pragma once
#include <vector>
#include "core/IMessagingBackend.h"

namespace mesh {

// Offline stand-in for a mesh radio: two contacts, a channel, echo replies, and periodic chatter.
class FakeBackend : public IMessagingBackend {
public:
  FakeBackend();
  const BackendCaps &caps() const override { return backendCaps; }
  void setListener(IMessagingListener *newListener) override { listener = newListener; }
  bool begin() override;
  void poll(uint32_t nowMs) override;
  uint32_t sendText(const std::string &conversationId, const std::string &text) override;

  static constexpr uint32_t localNodeId = 0x1000;
  static constexpr uint32_t aliceNodeId = 0x1001;
  static constexpr uint32_t bobNodeId = 0x1002;

private:
  struct ScheduledEvent {
    uint32_t dueMs;
    bool isDeliveryUpdate;  // true: deliveryState for messageId; false: emit message
    uint32_t messageId;
    DeliveryState deliveryState;
    Message message;
  };

  Message makeIncoming(const std::string &conversationId, uint32_t senderId, const std::string &text);
  void scheduleIncoming(uint32_t delayMs, const std::string &conversationId, uint32_t senderId, const std::string &text);

  BackendCaps backendCaps;
  IMessagingListener *listener = nullptr;
  std::vector<ScheduledEvent> scheduledEvents;
  uint32_t currentMs = 0;
  uint32_t nextMessageId = 1;
  uint32_t nextChatterMs = 4000;
  size_t chatterIndex = 0;
};

}  // namespace mesh
