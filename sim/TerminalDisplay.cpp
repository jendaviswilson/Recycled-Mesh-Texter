#include "TerminalDisplay.h"
#include <cstdio>
#include "core/FontSystem.h"

namespace mesh {

namespace {
const char *const clearScreen = "\x1b[H\x1b[2J";
const char *const lcdColors = "\x1b[30;48;5;151m";  // black text on pale green
const char *const resetColors = "\x1b[0m";
}  // namespace

TerminalDisplay::TerminalDisplay(const DisplayCaps &caps) : displayCaps(caps), lines(caps.rows) { clear(); }

void TerminalDisplay::setLine(uint8_t row, const char *text) {
  if (row >= displayCaps.rows) return;
  std::string line = FontSystem::foldText(text ? text : "", displayCaps.characterSet);
  line.resize(displayCaps.columns, ' ');
  lines[row] = line;
}

void TerminalDisplay::clear() {
  for (std::string &line : lines) line.assign(displayCaps.columns, ' ');
}

void TerminalDisplay::flush() {
  std::string border = "+" + std::string(displayCaps.columns, '-') + "+\n";
  std::string frame = clearScreen;
  frame += border;
  for (const std::string &line : lines) {
    frame += "|";
    frame += lcdColors;
    frame += line;
    frame += resetColors;
    frame += "|\n";
  }
  frame += border;
  frame += "\n" + statusText + "\n";
  std::fwrite(frame.data(), 1, frame.size(), stdout);
  std::fflush(stdout);
}

}  // namespace mesh
