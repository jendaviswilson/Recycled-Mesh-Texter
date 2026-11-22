#pragma once
#include <cstddef>
#include "KeyEvent.h"

namespace mesh {

// One row of a keymap table: a raw device code and what it means.
struct KeymapEntry {
  uint16_t rawCode;
  LogicalKey key;
  char character;  // used when key == LogicalKey::Character
};

// Translates RawKey codes to KeyEvents using a per-device table.
class KeyMapper {
public:
  // passPrintableAscii: raw codes 0x20-0x7E that are not in the table map to themselves as characters.
  KeyMapper(const KeymapEntry *entries, size_t entryCount, bool passPrintableAscii = false);

  KeyEvent map(const RawKey &rawKey) const;

private:
  const KeymapEntry *entries;
  size_t entryCount;
  bool passPrintableAscii;
};

}  // namespace mesh
