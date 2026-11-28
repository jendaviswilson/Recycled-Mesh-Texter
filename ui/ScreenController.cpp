#include "ScreenController.h"
ScreenController::ScreenController() : _screen(Screen::Home),
                                       _returnScreen(Screen::Home),
                                       _topLevelScreens{Screen::Home, Screen::Channels, Screen::Direct, Screen::Nodes}
{
}

bool ScreenController::isTopLevel() const
{
  for (Screen thisScreen : _topLevelScreens)
  {
    if (thisScreen == _screen)
    {
      return true;
    }
  }
  return false;
}

Screen ScreenController::screen() const
{
  return _screen;
}

Screen ScreenController::screenRing(Screen thisScreen, int direction) const
{
  size_t ringSize = _topLevelScreens.size();

  for (size_t i = 0; i < ringSize; ++i)
  {
    if (thisScreen == _topLevelScreens.at(i))
    {
      size_t index = ringSize + i + direction;
      index = index % ringSize;
      return _topLevelScreens.at(index);
    }
  }
  return Screen::Home;  //default if screen doesn't match any in Ring
}

//Decides what each keypress action does depending on what screen we are on.
//TODO:  abstract this out into a navigation stack that replaces _returnScreen

UiAction ScreenController::handleKey(const mesh::KeyEvent &event)
{
  if (!event.pressed) // ignore key release events
  {
    return UiAction::None;
  }

  if (isTopLevel())
  {
    switch (event.key) // All Top-Level screens
    {
      //Ring navigation: Left-Right
      case mesh::LogicalKey::Left:
        _screen = screenRing(_screen, -1);
        return UiAction::ScreenChanged;

      case mesh::LogicalKey::Right:
        _screen = screenRing(_screen, 1);
        return UiAction::ScreenChanged;

      //Scrolls lists: Up-Down
      case mesh::LogicalKey::Up:
        if (_screen == Screen::Home)
        {
          return UiAction::None;
        }
        return UiAction::SelectPrevious;

      case mesh::LogicalKey::Down:
        if (_screen == Screen::Home)
        {
          return UiAction::None;
        }
        return UiAction::SelectNext;

      //Go down a level: Enter
      case mesh::LogicalKey::Enter:
        if (_screen == Screen::Home)
        {
          _returnScreen = _screen;
          _screen = Screen::Reader;
          return UiAction::OpenNewest;
        }
        else if(_screen == Screen::Channels || _screen == Screen::Direct)
        {
          _returnScreen = _screen;
          _screen = Screen::Reader;
          return UiAction::OpenSelected;
        }
        else if (_screen == Screen::Nodes)
        {
          _returnScreen = _screen;
          _screen = Screen::NodeActions;
          return UiAction::OpenSelected;
        }
        else
        {
          return UiAction::None; //default fallback if none match
        }
        
      //Go to Home from any Top-Level screen on Clear  
      case mesh::LogicalKey::Clear:
        if (_screen == Screen::Home)
        {
          return UiAction::None;
        }
        _screen = Screen::Home;
        return UiAction::Back;

      default:
        return UiAction::None;
    }
  }  //End list screens
  else
  {
    switch (event.key) // item screens
    {
      case mesh::LogicalKey::Up:
        return UiAction::None; // TODO:  what does reader and Node Action do?

      case mesh::LogicalKey::Down:
        return UiAction::None; // TODO:  what does reader and Node Action do?

      case mesh::LogicalKey::Enter:
        return UiAction::None; // TODO:  what does reader and Node Action do?

      case mesh::LogicalKey::Clear:
        _screen = _returnScreen;
        return UiAction::Back;

      case mesh::LogicalKey::Left:
      case mesh::LogicalKey::Right:
      default:
        return UiAction::None;
    }
  }//end item screens
}
