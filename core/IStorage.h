#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace mesh {

// Persistent key/value blobs (flash filesystem, SD card, or a file on the host).
class IStorage {
public:
  virtual ~IStorage() = default;
  virtual bool write(const std::string &key, const std::vector<uint8_t> &data) = 0;
  virtual bool read(const std::string &key, std::vector<uint8_t> &data) = 0;
  virtual bool remove(const std::string &key) = 0;
};

// RAM-only storage for the simulator and tests; contents are lost on exit.
class MemoryStorage : public IStorage {
public:
  bool write(const std::string &key, const std::vector<uint8_t> &data) override {
    entries[key] = data;
    return true;
  }
  bool read(const std::string &key, std::vector<uint8_t> &data) override {
    auto found = entries.find(key);
    if (found == entries.end()) return false;
    data = found->second;
    return true;
  }
  bool remove(const std::string &key) override { return entries.erase(key) > 0; }

private:
  std::map<std::string, std::vector<uint8_t>> entries;
};

}  // namespace mesh
