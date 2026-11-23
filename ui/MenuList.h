#pragma once
#include <cstddef>
#include <string>
#include <vector>

// A list shown one item at a time, for single-line segment displays (about 9-18 characters).
// The items are owned elsewhere; the list only points at them.
class MenuList {
public:
  MenuList();

  // Points the menu at a list of items and selects the first one.
  // nullptr detaches the menu, so it has no items. The list itself is never modified.
  void setItems(const std::vector<std::string> *items);

  size_t itemCount() const;   // 0 when no items are set
  bool hasSelection() const;  // false when the list is empty
  size_t selectedIndex() const;
  // Text of the selected item, or an empty string when the list is empty.
  std::string selectedText() const;

  // Moves the selection by one, wrapping from the last item to the first and back.
  // Returns false when the list is empty.
  bool next();
  bool previous();

  // Exactly `width` characters: the selected item on the left and its position ("2/5") on the
  // right, when both fit with at least one space between them. Otherwise the item alone,
  // cut off or padded with spaces to width. An empty list gives width spaces.
  std::string displayText(size_t width) const;

private:
  const std::vector<std::string> *_items;
  size_t _selection;
};