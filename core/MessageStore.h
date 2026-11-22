#pragma once
#include <deque>
#include <string>
#include <vector>
#include "IMessagingBackend.h"
#include "MessageTypes.h"

namespace mesh {

// Generic inbox: conversations, their messages, and known contacts. Protocol-independent.
// A deque keeps references to existing conversations valid when new ones are added.
class MessageStore {
public:
  explicit MessageStore(size_t maxMessagesPerConversation = 50);

  // Finds a conversation, creating it if needed.
  Conversation &conversationFor(const std::string &conversationId, ConversationKind kind, const std::string &title);
  Conversation *find(const std::string &conversationId);
  const std::deque<Conversation> &conversations() const { return conversationList; }

  // Adds a message; creates a Direct conversation titled with the ID if it is unknown.
  void addMessage(const Message &message);
  bool updateDeliveryState(uint32_t messageId, DeliveryState state);
  void markRead(const std::string &conversationId);
  size_t totalUnread() const;

  void upsertContact(const Contact &contact);
  const Contact *findContact(uint32_t nodeId) const;
  std::string displayNameFor(uint32_t nodeId) const;

private:
  size_t maxMessagesPerConversation;
  std::deque<Conversation> conversationList;
  std::vector<Contact> contacts;
};

// Listener that writes backend events into a MessageStore. Subclass to also react to events.
class MessageStoreSink : public IMessagingListener {
public:
  explicit MessageStoreSink(MessageStore &store) : store(store) {}
  void onConversation(const std::string &conversationId, ConversationKind kind, const std::string &title) override;
  void onMessage(const Message &message) override;
  void onDeliveryState(uint32_t messageId, DeliveryState state) override;
  void onContact(const Contact &contact) override;

protected:
  MessageStore &store;
};

}  // namespace mesh
