// Checks DraftBuffer against the behavior described in ui/DraftBuffer.h.

#include <cstdio>
#include "ui/DraftBuffer.h"

int failureCount = 0;
int checkCount = 0;

void check(bool condition, const char *description) {
  checkCount++;
  if (condition) {
    std::printf("pass: %s\n", description);
  } else {
    failureCount++;
    std::printf("FAIL: %s\n", description);
  }
}

int main() {
  DraftBuffer draft(5);
  check(draft.isEmpty(), "new draft is empty");
  check(draft.length() == 0, "new draft has length 0");
  check(draft.maxLength() == 5, "maxLength matches constructor");
  check(draft.remaining() == 5, "new draft has 5 remaining");
  check(!draft.backspace(), "backspace on empty draft returns false");

  check(draft.append('H'), "append returns true when there is room");
  draft.append('E');
  draft.append('L');
  check(draft.text() == "HEL", "text holds appended characters");
  check(draft.length() == 3, "length counts characters");
  check(draft.remaining() == 2, "remaining shrinks as characters are added");
  check(!draft.isEmpty(), "draft with text is not empty");

  draft.append('L');
  draft.append('O');
  check(draft.isFull(), "draft at max length is full");
  check(!draft.append('!'), "append returns false when full");
  check(draft.text() == "HELLO", "full draft is unchanged by append");

  check(draft.tail(3) == "LLO", "tail returns the last characters");
  check(draft.tail(10) == "HELLO", "tail returns everything when width is larger");

  check(draft.backspace(), "backspace returns true when text exists");
  check(draft.text() == "HELL", "backspace removes the last character");

  draft.clear();
  check(draft.isEmpty(), "clear empties the draft");
  check(draft.remaining() == 5, "clear restores remaining");

  std::printf("\n%d checks, %d failed\n", checkCount, failureCount);
  return failureCount == 0 ? 0 : 1;
}