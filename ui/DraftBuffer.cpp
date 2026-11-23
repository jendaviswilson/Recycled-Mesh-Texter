
#include "DraftBuffer.h"

DraftBuffer::DraftBuffer(size_t maxLength)
{
  maximumLength = maxLength;
}

const std::string &DraftBuffer::text() const
{
  return draftText;
}

size_t DraftBuffer::length() const
{
  return draftText.size();
}

size_t DraftBuffer::maxLength() const
{
  return maximumLength;
}

size_t DraftBuffer::remaining() const
{
  return (maximumLength - draftText.size());
}

bool DraftBuffer::isEmpty() const
{
  return draftText.empty();
}

bool DraftBuffer::isFull() const
{
  return (draftText.size() >= maximumLength);
}

// Adds one character to the end. Returns false and changes nothing if the draft is full.
bool DraftBuffer::append(char character)
{
  if (draftText.size() < maximumLength)
  {
    draftText.push_back(character);
    return true;
  }
  else
  {
    return false;
  }
}

// Removes the last character. Returns false if the draft was already empty.
bool DraftBuffer::backspace()
{
  if (draftText.size() > 0)
  {
    draftText.pop_back();
    return true;
  }
  else
  {
    return false;
  }
}

// Empties the draft.
void DraftBuffer::clear()
{
  draftText.clear();
}

// The last `width` characters, for a display too narrow to show the whole draft.
// Returns the whole draft if it is shorter than width.
std::string DraftBuffer::tail(size_t width) const
{
  if (draftText.size() <= width)
  {
    return draftText;
  }
  else
  {
    return draftText.substr(draftText.size() - width);
  }
}
