#pragma once
#include <string>
#include "ITextDisplay.h"

namespace mesh {

// Shared character rules so every display shows the same text the same way.
// Segment glyph masks (canonical 16-segment table plus per-display SegmentMap) are added
// with the first segment-display driver, where they can be verified against real hardware.
namespace FontSystem {

constexpr char unsupportedCharacter = '?';

bool isSupported(char character, CharacterSet characterSet);
// Maps a character to the closest one the character set can show.
char fold(char character, CharacterSet characterSet);
std::string foldText(const std::string &text, CharacterSet characterSet);

}  // namespace FontSystem
}  // namespace mesh
