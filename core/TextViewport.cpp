#include "TextViewport.h"

namespace mesh {

namespace {
const char *const marqueeGap = "   ";
const size_t marqueeGapLength = 3;
}  // namespace

TextViewport::TextViewport(uint8_t columns, uint8_t rows) : columns(columns), rows(rows) { wrap(); }

void TextViewport::resize(uint8_t newColumns, uint8_t newRows) {
  columns = newColumns;
  rows = newRows;
  wrap();
}

void TextViewport::setText(const std::string &text) {
  sourceText = text;
  wrap();
}

size_t TextViewport::pageCount() const {
  size_t pages = (wrappedLines.size() + rows - 1) / rows;
  return pages == 0 ? 1 : pages;
}

bool TextViewport::nextPage() {
  if (isLastPage()) return false;
  currentPage++;
  return true;
}

bool TextViewport::previousPage() {
  if (currentPage == 0) return false;
  currentPage--;
  return true;
}

std::string TextViewport::visibleLine(uint8_t row) const {
  size_t lineIndex = currentPage * rows + row;
  std::string line = lineIndex < wrappedLines.size() ? wrappedLines[lineIndex] : std::string();
  line.resize(columns, ' ');
  return line;
}

bool TextViewport::fits(const std::string &text, uint8_t columns, uint8_t rows) {
  TextViewport probe(columns, rows);
  probe.setText(text);
  return probe.fits();
}

size_t TextViewport::marqueeLength() const {
  if (sourceText.size() <= columns) return sourceText.size();
  return sourceText.size() + marqueeGapLength;
}

std::string TextViewport::marqueeWindow(size_t offset) const {
  std::string window;
  if (sourceText.size() <= columns) {
    window = sourceText;
  } else {
    std::string loop = sourceText + marqueeGap;
    for (uint8_t position = 0; position < columns; position++) {
      window += loop[(offset + position) % loop.size()];
    }
  }
  for (char &character : window) {
    if (character == '\n') character = ' ';
  }
  window.resize(columns, ' ');
  return window;
}

// Greedy word wrap. Explicit newlines force a break; words longer than a line are hard-split.
void TextViewport::wrap() {
  wrappedLines.clear();
  currentPage = 0;
  if (columns == 0) return;

  std::string currentLine;
  size_t position = 0;
  while (position <= sourceText.size()) {
    size_t wordEnd = sourceText.find_first_of(" \n", position);
    if (wordEnd == std::string::npos) wordEnd = sourceText.size();
    std::string word = sourceText.substr(position, wordEnd - position);

    while (word.size() > columns) {
      if (!currentLine.empty()) {
        wrappedLines.push_back(currentLine);
        currentLine.clear();
      }
      wrappedLines.push_back(word.substr(0, columns));
      word.erase(0, columns);
    }
    if (!word.empty()) {
      size_t neededWidth = currentLine.empty() ? word.size() : currentLine.size() + 1 + word.size();
      if (neededWidth > columns) {
        wrappedLines.push_back(currentLine);
        currentLine = word;
      } else {
        if (!currentLine.empty()) currentLine += ' ';
        currentLine += word;
      }
    }
    if (wordEnd < sourceText.size() && sourceText[wordEnd] == '\n') {
      wrappedLines.push_back(currentLine);
      currentLine.clear();
    }
    position = wordEnd + 1;
  }
  if (!currentLine.empty() || wrappedLines.empty()) wrappedLines.push_back(currentLine);
}

}  // namespace mesh
