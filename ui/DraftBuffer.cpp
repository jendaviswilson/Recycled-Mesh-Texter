
#include <cstddef>
#include <string>

#include "DraftBuffer.h"

/*
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
*/

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
    if (draftText.size() < maxLength())
        {
            draftText.push_back(character);
            return true;
        }
        else return false;
}

// Removes the last character. Returns false if the draft was already empty.
  bool DraftBuffer::backspace()
  {
    if (draftText.size() > 0)
      {
        draftText.pop_back();
        return true;
      }
      else return false;
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
      if (draftText.size() <= width) return draftText;
      return draftText.substr(draftText.size() - width);
    }
  

