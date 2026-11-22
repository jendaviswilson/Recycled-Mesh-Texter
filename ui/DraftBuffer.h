#pragma once
#include <cstddef>
#include <string>

// Text being typed for an outgoing message, limited to a maximum length.
class DraftBuffer {
public:
  // maxLength: the most characters the draft may hold.
  explicit DraftBuffer(size_t maxLength);

  // Adds one character to the end. Returns false and changes nothing if the draft is full.
  bool append(char character);
  // Removes the last character. Returns false if the draft was already empty.
  bool backspace();
  // Empties the draft.
  void clear();

  const std::string &text() const;
  size_t length() const;
  size_t maxLength() const;
  size_t remaining() const;  // characters still allowed
  bool isEmpty() const;
  bool isFull() const;

  // The last `width` characters, for a display too narrow to show the whole draft.
  // Returns the whole draft if it is shorter than width.
  std::string tail(size_t width) const;

private:
  std::string draftText;
  size_t maximumLength;
};