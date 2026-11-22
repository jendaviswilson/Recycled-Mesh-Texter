#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace mesh {

// Fits text into a columns x rows window: word wrap, paging, and single-line marquee.
// Display-independent; the UI owns one per text area and copies visibleLine() rows to a display.
class TextViewport {
public:
  TextViewport(uint8_t columns, uint8_t rows);

  void resize(uint8_t columns, uint8_t rows);
  void setText(const std::string &text);
  const std::string &text() const { return sourceText; }

  // Wrapping and paging
  size_t lineCount() const { return wrappedLines.size(); }
  size_t pageCount() const;
  size_t pageIndex() const { return currentPage; }
  bool isFirstPage() const { return currentPage == 0; }
  bool isLastPage() const { return currentPage + 1 >= pageCount(); }
  bool nextPage();
  bool previousPage();
  void firstPage() { currentPage = 0; }
  void lastPage() { currentPage = pageCount() - 1; }
  // Row of the current page, padded with spaces to the full width.
  std::string visibleLine(uint8_t row) const;

  // True when the whole text fits on one page.
  bool fits() const { return lineCount() <= rows; }
  static bool fits(const std::string &text, uint8_t columns, uint8_t rows);

  // Marquee: the text on one line, scrolled horizontally. Offsets wrap around after marqueeLength().
  size_t marqueeLength() const;
  std::string marqueeWindow(size_t offset) const;

private:
  void wrap();

  uint8_t columns;
  uint8_t rows;
  std::string sourceText;
  std::vector<std::string> wrappedLines;
  size_t currentPage = 0;
};

}  // namespace mesh
