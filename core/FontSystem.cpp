#include "FontSystem.h"

namespace mesh {
namespace FontSystem {

bool isSupported(char character, CharacterSet characterSet) {
  unsigned char code = static_cast<unsigned char>(character);
  switch (characterSet) {
    case CharacterSet::UppercaseOnly:
      return (code >= 0x20 && code <= 0x5F) || character == '|';
    case CharacterSet::Ascii:
      return code >= 0x20 && code <= 0x7E;
  }
  return false;
}

char fold(char character, CharacterSet characterSet) {
  if (isSupported(character, characterSet)) return character;
  if (characterSet == CharacterSet::UppercaseOnly) {
    if (character >= 'a' && character <= 'z') return static_cast<char>(character - 'a' + 'A');
    switch (character) {
      case '{': return '(';
      case '}': return ')';
      case '~': return '-';
      case '`': return '\'';
      case '\t': return ' ';
      default: break;
    }
  }
  if (character == '\t') return ' ';
  return unsupportedCharacter;
}

std::string foldText(const std::string &text, CharacterSet characterSet) {
  std::string folded;
  folded.reserve(text.size());
  for (char character : text) folded += fold(character, characterSet);
  return folded;
}

}  // namespace FontSystem
}  // namespace mesh
