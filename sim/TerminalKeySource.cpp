#include "TerminalKeySource.h"
#include <unistd.h>

namespace mesh {

const KeymapEntry terminalKeymap[] = {
    {'\r', LogicalKey::Enter, 0},
    {'\n', LogicalKey::Enter, 0},
    {0x7F, LogicalKey::Back, 0},  // Backspace on most terminals
    {0x08, LogicalKey::Back, 0},  // Ctrl-H
    {'\t', LogicalKey::Clear, 0},
    {TerminalKeySource::rawUp, LogicalKey::Up, 0},
    {TerminalKeySource::rawDown, LogicalKey::Down, 0},
    {TerminalKeySource::rawLeft, LogicalKey::Left, 0},
    {TerminalKeySource::rawRight, LogicalKey::Right, 0},
    {TerminalKeySource::rawEscape, LogicalKey::Menu, 0},
};
const size_t terminalKeymapSize = sizeof(terminalKeymap) / sizeof(terminalKeymap[0]);

TerminalKeySource::~TerminalKeySource() { restoreTerminal(); }

// Turns off line buffering and echo; reads return immediately when no key is waiting.
void TerminalKeySource::begin() {
  if (tcgetattr(STDIN_FILENO, &savedSettings) != 0) return;
  settingsSaved = true;
  termios rawSettings = savedSettings;
  rawSettings.c_lflag &= ~(ICANON | ECHO);
  rawSettings.c_cc[VMIN] = 0;
  rawSettings.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &rawSettings);
}

void TerminalKeySource::restoreTerminal() {
  if (settingsSaved) {
    tcsetattr(STDIN_FILENO, TCSANOW, &savedSettings);
    settingsSaved = false;
  }
}

bool TerminalKeySource::readByte(unsigned char &byte) { return read(STDIN_FILENO, &byte, 1) == 1; }

bool TerminalKeySource::poll(RawKey &rawKey) {
  unsigned char byte;
  if (!readByte(byte)) return false;
  rawKey.pressed = true;
  rawKey.code = byte;
  if (byte != 0x1B) return true;

  // Arrow keys arrive as ESC [ A..D; a lone ESC is the Escape key.
  unsigned char bracket;
  unsigned char finalByte;
  if (!readByte(bracket) || bracket != '[' || !readByte(finalByte)) {
    rawKey.code = rawEscape;
    return true;
  }
  switch (finalByte) {
    case 'A': rawKey.code = rawUp; break;
    case 'B': rawKey.code = rawDown; break;
    case 'C': rawKey.code = rawRight; break;
    case 'D': rawKey.code = rawLeft; break;
    default: rawKey.code = rawEscape; break;
  }
  return true;
}

}  // namespace mesh
