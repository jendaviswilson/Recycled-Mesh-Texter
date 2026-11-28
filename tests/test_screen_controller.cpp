// Checks ScreenController against the task 3 rules table.

#include <cstdio>
#include "ui/ScreenController.h"

using mesh::KeyEvent;
using mesh::LogicalKey;

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

KeyEvent press(LogicalKey key) { return KeyEvent{key, 0, true}; }
KeyEvent release(LogicalKey key) { return KeyEvent{key, 0, false}; }

int main() {
  ScreenController controller;
  check(controller.screen() == Screen::Home, "starts on Home");
  check(controller.isTopLevel(), "Home is top-level");

  // Ring: Home -> Channels -> Direct -> Nodes -> Home
  check(controller.handleKey(press(LogicalKey::Right)) == UiAction::ScreenChanged, "Right returns ScreenChanged");
  check(controller.screen() == Screen::Channels, "Right from Home goes to Channels");
  controller.handleKey(press(LogicalKey::Right));
  check(controller.screen() == Screen::Direct, "Right from Channels goes to Direct");
  controller.handleKey(press(LogicalKey::Right));
  check(controller.screen() == Screen::Nodes, "Right from Direct goes to Nodes");
  controller.handleKey(press(LogicalKey::Right));
  check(controller.screen() == Screen::Home, "Right from Nodes wraps to Home");
  check(controller.handleKey(press(LogicalKey::Left)) == UiAction::ScreenChanged, "Left returns ScreenChanged");
  check(controller.screen() == Screen::Nodes, "Left from Home wraps to Nodes");
  controller.handleKey(press(LogicalKey::Left));
  check(controller.screen() == Screen::Direct, "Left from Nodes goes to Direct");

  // Releases are ignored
  check(controller.handleKey(release(LogicalKey::Right)) == UiAction::None, "key release returns None");
  check(controller.screen() == Screen::Direct, "key release does not move");

  // Home keys
  ScreenController homeController;
  check(homeController.handleKey(press(LogicalKey::Up)) == UiAction::None, "Up on Home does nothing");
  check(homeController.handleKey(press(LogicalKey::Clear)) == UiAction::None, "Clear on Home does nothing");
  check(homeController.screen() == Screen::Home, "Home stays Home");
  check(homeController.handleKey(press(LogicalKey::Enter)) == UiAction::OpenNewest, "Enter on Home returns OpenNewest");
  check(homeController.screen() == Screen::Reader, "Enter on Home opens Reader");
  check(!homeController.isTopLevel(), "Reader is not top-level");
  check(homeController.handleKey(press(LogicalKey::Clear)) == UiAction::Back, "Clear in Reader returns Back");
  check(homeController.screen() == Screen::Home, "Back from Reader returns to Home when opened from Home");

  // Lists: Up/Down select, Enter opens, Clear goes Home
  ScreenController listController;
  listController.handleKey(press(LogicalKey::Right));  // Channels
  check(listController.handleKey(press(LogicalKey::Down)) == UiAction::SelectNext, "Down in a list returns SelectNext");
  check(listController.handleKey(press(LogicalKey::Up)) == UiAction::SelectPrevious, "Up in a list returns SelectPrevious");
  check(listController.screen() == Screen::Channels, "Up/Down stay on the list");
  check(listController.handleKey(press(LogicalKey::Enter)) == UiAction::OpenSelected, "Enter in Channels returns OpenSelected");
  check(listController.screen() == Screen::Reader, "Enter in Channels opens Reader");

  // Reader ignores other keys
  check(listController.handleKey(press(LogicalKey::Left)) == UiAction::None, "Left in Reader does nothing");
  check(listController.handleKey(press(LogicalKey::Enter)) == UiAction::None, "Enter in Reader does nothing yet");
  check(listController.screen() == Screen::Reader, "Reader stays Reader");
  listController.handleKey(press(LogicalKey::Clear));
  check(listController.screen() == Screen::Channels, "Back from Reader returns to Channels");

  // Direct -> Reader -> back to Direct
  listController.handleKey(press(LogicalKey::Right));  // Direct
  listController.handleKey(press(LogicalKey::Enter));
  check(listController.screen() == Screen::Reader, "Enter in Direct opens Reader");
  listController.handleKey(press(LogicalKey::Clear));
  check(listController.screen() == Screen::Direct, "Back from Reader returns to Direct");

  // Nodes -> NodeActions -> back to Nodes
  listController.handleKey(press(LogicalKey::Right));  // Nodes
  check(listController.handleKey(press(LogicalKey::Enter)) == UiAction::OpenSelected, "Enter in Nodes returns OpenSelected");
  check(listController.screen() == Screen::NodeActions, "Enter in Nodes opens NodeActions");
  check(listController.handleKey(press(LogicalKey::Right)) == UiAction::None, "Right in NodeActions does nothing");
  check(listController.handleKey(press(LogicalKey::Clear)) == UiAction::Back, "Clear in NodeActions returns Back");
  check(listController.screen() == Screen::Nodes, "Back from NodeActions returns to Nodes");

  // Clear on a top-level list goes Home
  check(listController.handleKey(press(LogicalKey::Clear)) == UiAction::Back, "Clear on a list returns Back");
  check(listController.screen() == Screen::Home, "Clear on a list goes to Home");

  std::printf("\n%d checks, %d failed\n", checkCount, failureCount);
  return failureCount == 0 ? 0 : 1;
}