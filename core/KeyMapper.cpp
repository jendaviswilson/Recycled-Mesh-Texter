#include "KeyMapper.h"

namespace mesh {

KeyMapper::KeyMapper(const KeymapEntry *entries, size_t entryCount, bool passPrintableAscii)
    : entries(entries), entryCount(entryCount), passPrintableAscii(passPrintableAscii) {}

KeyEvent KeyMapper::map(const RawKey &rawKey) const {
  KeyEvent event;
  event.pressed = rawKey.pressed;
  for (size_t index = 0; index < entryCount; index++) {
    if (entries[index].rawCode == rawKey.code) {
      event.key = entries[index].key;
      event.character = entries[index].character;
      return event;
    }
  }
  if (passPrintableAscii && rawKey.code >= 0x20 && rawKey.code <= 0x7E) {
    event.key = LogicalKey::Character;
    event.character = static_cast<char>(rawKey.code);
  }
  return event;
}

}  // namespace mesh
