#pragma once
#include <string>
#include <vector>
#include "core/ITextDisplay.h"

namespace mesh {

// Draws a text display as a box in the terminal, using the same character folding as real hardware.
class TerminalDisplay : public ITextDisplay {
public:
  explicit TerminalDisplay(const DisplayCaps &caps);
  const DisplayCaps &caps() const override { return displayCaps; }
  void setLine(uint8_t row, const char *text) override;
  void clear() override;
  void flush() override;

  // Simulator-only text printed under the display (debug state, key help).
  void setStatus(const std::string &text) { statusText = text; }

private:
  DisplayCaps displayCaps;
  std::vector<std::string> lines;
  std::string statusText;
};

}  // namespace mesh
