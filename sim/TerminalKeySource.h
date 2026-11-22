#pragma once
#include <termios.h>
#include "core/IKeySource.h"
#include "core/KeyMapper.h"

namespace mesh {

// Reads keys from a POSIX terminal without waiting for Enter.
// Raw codes are the byte values typed, plus the codes below for escape sequences.
class TerminalKeySource : public IKeySource {
public:
  static constexpr uint16_t rawUp = 0x100;
  static constexpr uint16_t rawDown = 0x101;
  static constexpr uint16_t rawRight = 0x102;
  static constexpr uint16_t rawLeft = 0x103;
  static constexpr uint16_t rawEscape = 0x104;

  ~TerminalKeySource() override;
  void begin() override;
  bool poll(RawKey &rawKey) override;
  void restoreTerminal();

private:
  bool readByte(unsigned char &byte);

  termios savedSettings{};
  bool settingsSaved = false;
};

// Keymap for TerminalKeySource; use with passPrintableAscii = true.
extern const KeymapEntry terminalKeymap[];
extern const size_t terminalKeymapSize;

}  // namespace mesh
