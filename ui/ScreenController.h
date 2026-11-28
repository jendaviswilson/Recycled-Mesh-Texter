#pragma once
#include "core/KeyEvent.h"
#include <vector>

// Top-level screens cycle with Left/Right, like Meshtastic device frames.
// Reader and NodeActions open from a top-level screen; Clear returns to it.
enum class Screen {
  Home,         // status: unread count
  Channels,     // channel list
  Direct,       // direct-message conversation list
  Nodes,        // known nodes / contacts
  Reader,       // messages in one conversation
  NodeActions,  // actions for one node
};

// What the caller should do after a key press. The caller owns the lists and data.
enum class UiAction {
  None,
  ScreenChanged,   // Left/Right moved to another top-level screen
  SelectPrevious,  // Up in a list
  SelectNext,      // Down in a list
  OpenSelected,    // Enter in a list: open the selected item
  OpenNewest,      // Enter on Home: open the newest unread conversation
  Back,            // Clear: return to the screen that opened this one
};

class ScreenController {
public:
  ScreenController();  // starts on Home

  // Moves between screens for one key press and returns what the caller should do.
  // Key releases (pressed == false) are ignored and return None.
  UiAction handleKey(const mesh::KeyEvent &event);

  Screen screen() const;
  // True for Home, Channels, Direct and Nodes.
  bool isTopLevel() const;

private:
  Screen _screen;
  Screen _returnScreen;  // top-level screen that opened Reader or NodeActions

  std::vector<Screen> _topLevelScreens;  //list of screens considered top-level
  Screen screenRing(Screen thisScreen, int direction) const;  //gives the screen that returns when you move a number of positions in the ring
};