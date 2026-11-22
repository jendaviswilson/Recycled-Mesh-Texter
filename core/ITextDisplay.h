#pragma once
#include <cstdint>

namespace mesh {

enum class DisplayTechnology : uint8_t {
  Segment14,      // e.g. Franklin Spelling Ace LCD
  Segment16,
  CharacterCell,  // HD44780-style character LCD
  Pixel,          // graphic LCD/OLED drawing a fixed-width font
};

enum class CharacterSet : uint8_t {
  UppercaseOnly,  // letters fold to uppercase; limited punctuation
  Ascii,          // printable ASCII 0x20-0x7E
};

// What the UI needs to know to lay out text for a display.
struct DisplayCaps {
  uint8_t columns;
  uint8_t rows;
  DisplayTechnology technology;
  CharacterSet characterSet;
};

// Any small text display. Text passed in is folded to the display's character set by the driver.
class ITextDisplay {
public:
  virtual ~ITextDisplay() = default;
  virtual const DisplayCaps &caps() const = 0;
  virtual void begin() {}
  // Stages one row of text; text longer than caps().columns is truncated, shorter is padded.
  virtual void setLine(uint8_t row, const char *text) = 0;
  virtual void clear() = 0;
  // Sends staged rows to the hardware.
  virtual void flush() = 0;
};

}  // namespace mesh
