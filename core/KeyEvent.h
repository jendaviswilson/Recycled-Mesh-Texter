#pragma once
#include <cstdint>

namespace mesh {

// Device-independent key meanings. KeyMapper turns hardware-specific RawKey codes into these.
enum class LogicalKey : uint8_t {
  None,
  Character,  // printable character, carried in KeyEvent::character
  Enter,
  Back,       // delete one character / go back
  Clear,      // cancel / clear the current field
  Up,
  Down,
  Left,
  Right,
  Menu,
  Power,
};

// A key exactly as an input device reports it. The meaning of `code` is defined by each IKeySource.
struct RawKey {
  uint16_t code = 0;
  bool pressed = true;  // false for a release, on devices that report releases
};

struct KeyEvent {
  LogicalKey key = LogicalKey::None;
  char character = 0;
  bool pressed = true;
};

}  // namespace mesh
