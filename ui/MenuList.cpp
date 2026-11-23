#include "MenuList.h"

// A list shown one item at a time, for single-line segment displays (about 9-18 characters).
// The items are owned elsewhere; the list only points at them.

MenuList::MenuList() : _items(nullptr), _selection(0) {}

// Points the menu at a list of items and selects the first one.
// nullptr detaches the menu, so it has no items. The list itself is never modified.
void MenuList::setItems(const std::vector<std::string> *items)
{
    _items = items; // nullptr is fine: it means "no list"
    _selection = 0;
}

size_t MenuList::itemCount() const
{
    if (_items == nullptr)
    {
        return 0; // no list attached, so zero items
    }
    else
    {
        return _items->size(); // safe: items points at a real vector
    }
}

// Is there 1 or more valid items
bool MenuList::hasSelection() const
{
    return itemCount() > 0;
}

size_t MenuList::selectedIndex() const
{
    if (!hasSelection())
    {
        return 0;
    }
    else
    {
        return _selection;
    }
}
// Text of the selected item, or an empty string when the list is empty.
std::string MenuList::selectedText() const
{
    if (hasSelection())
    {
        return _items->at(selectedIndex());
    }
    else
    {
        return "";
    }
}

// Moves the selection by one, wrapping from the last item to the first and back.
// Returns false when the list is empty.
bool MenuList::next()
{
    if (!hasSelection())
    {
        return false;
    }
    else
    {
        if (_selection >= (itemCount() - 1))
        {
            _selection = 0;
        }
        else
        {
            _selection++;
        }
        return true;
    }
}

bool MenuList::previous()
{
    if (!hasSelection())
    {
        return false;
    }
    else
    {
        if (_selection == 0)
        {
            _selection = (itemCount() - 1);
        }
        else
        {
            _selection--;
        }
        return true;
    }
}

// Exactly `width` characters: the selected item on the left and its position ("2/5") on the
// right, when both fit with at least one space between them. Otherwise the item alone,
// cut off or padded with spaces to width. An empty list gives width spaces.
std::string MenuList::displayText(size_t width) const
{
    if (hasSelection())
    {
        // unpad spaces
        std::string leftText = selectedText();
        while (!leftText.empty() && leftText.back() == ' ')
        {
            leftText.pop_back();
        }

        // create " X/N" right-justified menu position indicator.  Only show if it entirely fits the line.
        std::string suffix = " " + std::to_string(_selection + 1) + "/" + std::to_string(itemCount());
        size_t fullLength = leftText.length() + suffix.length();
        if (fullLength <= width)
        {
            // right-justify padding for suffix
            std::string padding((width - fullLength), ' ');
            return leftText + padding + suffix;
        }
        else if (leftText.length() < width)
        {
            return leftText + std::string(width - leftText.length(), ' ');
        }
        else
        {
            return leftText.substr(0, width);
        }
    }
