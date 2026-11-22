#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace mesh {

// Conversation IDs are strings chosen by the backend, e.g. "dm:1001", "ch:0", "room:<key>".

enum class ConversationKind : uint8_t {
  Direct,   // one-to-one
  Channel,  // shared channel (Meshtastic channels, MeshCore public/group channels)
  Room,     // MeshCore room server
};

enum class DeliveryState : uint8_t {
  Received,  // incoming message
  Pending,   // queued, not yet sent over the air
  Sent,      // transmitted, no acknowledgement expected or not yet received
  Delivered, // acknowledged by the recipient
  Failed,
};

struct Contact {
  uint32_t nodeId = 0;
  std::string shortName;
  std::string longName;
};

struct Message {
  uint32_t messageId = 0;
  std::string conversationId;
  uint32_t senderId = 0;
  std::string text;
  uint32_t timestampSeconds = 0;
  bool outgoing = false;
  DeliveryState state = DeliveryState::Received;
  bool read = false;
};

struct Conversation {
  std::string conversationId;
  ConversationKind kind = ConversationKind::Direct;
  std::string title;
  std::vector<Message> messages;

  size_t unreadCount() const {
    size_t count = 0;
    for (const Message &message : messages) {
      if (!message.outgoing && !message.read) count++;
    }
    return count;
  }
};

}  // namespace mesh
