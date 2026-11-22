#pragma once
#include "KeyEvent.h"

namespace mesh {

// Any input device: Ace ASIC keyboard, GPIO matrix keypad, USB HID, serial, terminal.
class IKeySource {
public:
  virtual ~IKeySource() = default;
  virtual void begin() {}
  // Returns true and fills rawKey when a key event is waiting. Must not block.
  virtual bool poll(RawKey &rawKey) = 0;
};

}  // namespace mesh
