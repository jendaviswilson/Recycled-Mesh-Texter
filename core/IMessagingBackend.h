#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include "MessageTypes.h"

namespace mesh {

// What a protocol can do, so the UI can adapt (e.g. enforce a length limit, show delivery ticks).
struct BackendCaps {
  const char *protocolName;
  size_t maxTextLength;     // longest text sendText() accepts
  bool splitsLongMessages;  // true if the protocol client splits text across several packets
  bool deliveryAcks;        // true if direct messages report Delivered
  bool rooms;               // true if ConversationKind::Room is supported
};

// Receives everything a backend learns. MessageStoreSink is the standard implementation.
class IMessagingListener {
public:
  virtual ~IMessagingListener() = default;
  virtual void onConversation(const std::string &conversationId, ConversationKind kind, const std::string &title) = 0;
  // Called for incoming messages and for each message sent through sendText().
  virtual void onMessage(const Message &message) = 0;
  virtual void onDeliveryState(uint32_t messageId, DeliveryState state) = 0;
  virtual void onContact(const Contact &contact) = 0;
};

// One protocol (Meshtastic, MeshCore, fake). Only one runs per device.
class IMessagingBackend {
public:
  virtual ~IMessagingBackend() = default;
  virtual const BackendCaps &caps() const = 0;
  virtual void setListener(IMessagingListener *listener) = 0;
  virtual bool begin() = 0;
  // Called often from the main loop with a millisecond clock. Must not block.
  virtual void poll(uint32_t nowMs) = 0;
  // Returns the new message ID, or 0 if the text was rejected.
  virtual uint32_t sendText(const std::string &conversationId, const std::string &text) = 0;
};

}  // namespace mesh
