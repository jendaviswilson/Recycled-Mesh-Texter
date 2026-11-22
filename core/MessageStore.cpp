#include "MessageStore.h"
#include <cstdio>

namespace mesh {

MessageStore::MessageStore(size_t maxMessagesPerConversation)
    : maxMessagesPerConversation(maxMessagesPerConversation) {}

Conversation &MessageStore::conversationFor(const std::string &conversationId, ConversationKind kind,
                                            const std::string &title) {
  if (Conversation *existing = find(conversationId)) return *existing;
  Conversation conversation;
  conversation.conversationId = conversationId;
  conversation.kind = kind;
  conversation.title = title;
  conversationList.push_back(conversation);
  return conversationList.back();
}

Conversation *MessageStore::find(const std::string &conversationId) {
  for (Conversation &conversation : conversationList) {
    if (conversation.conversationId == conversationId) return &conversation;
  }
  return nullptr;
}

void MessageStore::addMessage(const Message &message) {
  Conversation &conversation = conversationFor(message.conversationId, ConversationKind::Direct, message.conversationId);
  conversation.messages.push_back(message);
  while (conversation.messages.size() > maxMessagesPerConversation) {
    conversation.messages.erase(conversation.messages.begin());
  }
}

bool MessageStore::updateDeliveryState(uint32_t messageId, DeliveryState state) {
  for (Conversation &conversation : conversationList) {
    for (Message &message : conversation.messages) {
      if (message.messageId == messageId) {
        message.state = state;
        return true;
      }
    }
  }
  return false;
}

void MessageStore::markRead(const std::string &conversationId) {
  if (Conversation *conversation = find(conversationId)) {
    for (Message &message : conversation->messages) message.read = true;
  }
}

size_t MessageStore::totalUnread() const {
  size_t total = 0;
  for (const Conversation &conversation : conversationList) total += conversation.unreadCount();
  return total;
}

void MessageStore::upsertContact(const Contact &contact) {
  for (Contact &existing : contacts) {
    if (existing.nodeId == contact.nodeId) {
      existing = contact;
      return;
    }
  }
  contacts.push_back(contact);
}

const Contact *MessageStore::findContact(uint32_t nodeId) const {
  for (const Contact &contact : contacts) {
    if (contact.nodeId == nodeId) return &contact;
  }
  return nullptr;
}

std::string MessageStore::displayNameFor(uint32_t nodeId) const {
  if (const Contact *contact = findContact(nodeId)) {
    if (!contact->shortName.empty()) return contact->shortName;
    if (!contact->longName.empty()) return contact->longName;
  }
  char hexName[12];
  std::snprintf(hexName, sizeof(hexName), "!%08x", static_cast<unsigned>(nodeId));
  return hexName;
}

void MessageStoreSink::onConversation(const std::string &conversationId, ConversationKind kind,
                                      const std::string &title) {
  store.conversationFor(conversationId, kind, title);
}

void MessageStoreSink::onMessage(const Message &message) { store.addMessage(message); }

void MessageStoreSink::onDeliveryState(uint32_t messageId, DeliveryState state) {
  store.updateDeliveryState(messageId, state);
}

void MessageStoreSink::onContact(const Contact &contact) { store.upsertContact(contact); }

}  // namespace mesh
