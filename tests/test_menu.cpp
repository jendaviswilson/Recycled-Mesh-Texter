// Checks MenuList against the behavior described in ui/MenuList.h.

#include <cstdio>
#include <string>
#include <vector>
#include "ui/MenuList.h"

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

// Shows the actual text between bars when a display check fails.
void checkDisplay(const MenuList &menu, size_t width, const std::string &expected, const char *description) {
  std::string actual = menu.displayText(width);
  check(actual == expected, description);
  if (actual != expected) {
    std::printf("      expected |%s|\n      got      |%s|\n", expected.c_str(), actual.c_str());
  }
}

int main() {
  MenuList menu;
  check(menu.itemCount() == 0, "new menu has no items");
  check(!menu.hasSelection(), "new menu has no selection");
  check(menu.selectedText() == "", "new menu selected text is empty");
  check(!menu.next(), "next on empty menu returns false");
  check(!menu.previous(), "previous on empty menu returns false");
  checkDisplay(menu, 9, "         ", "empty menu displays width spaces");

  std::vector<std::string> names = {"BOB", "CAROL", "CHARLOTTE", "ALEXANDRIA", "ED"};
  menu.setItems(&names);
  check(menu.itemCount() == 5, "item count matches list");
  check(menu.hasSelection(), "menu with items has a selection");
  check(menu.selectedIndex() == 0, "setItems selects the first item");
  check(menu.selectedText() == "BOB", "selected text is the first item");
  checkDisplay(menu, 9, "BOB   1/5", "item and position with padding between");
  checkDisplay(menu, 18, "BOB            1/5", "position right-aligned on a wider display");

  check(menu.next(), "next returns true");
  checkDisplay(menu, 9, "CAROL 2/5", "item and position fit exactly");
  menu.next();
  checkDisplay(menu, 9, "CHARLOTTE", "no room for position shows item alone");
  menu.next();
  checkDisplay(menu, 9, "ALEXANDRI", "long item is cut off");
  checkDisplay(menu, 4, "ALEX", "long item cut off on a narrow display");
  menu.next();
  checkDisplay(menu, 9, "ED    5/5", "last item");

  menu.next();
  check(menu.selectedIndex() == 0, "next wraps from last to first");
  check(menu.previous(), "previous returns true");
  check(menu.selectedIndex() == 4, "previous wraps from first to last");

  menu.setItems(&names);
  check(menu.selectedIndex() == 0, "setItems resets the selection");
  check(names.size() == 5 && names[0] == "BOB", "menu does not modify the list");

  menu.setItems(nullptr);
  check(menu.itemCount() == 0, "nullptr detaches the list");
  check(names.size() == 5, "detaching leaves the list intact");

  std::printf("\n%d checks, %d failed\n", checkCount, failureCount);
  return failureCount == 0 ? 0 : 1;
}