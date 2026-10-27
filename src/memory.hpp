#pragma once

#include <cstdint>
#include <optional>
#include <vector>

class Memory {
  std::vector<uint8_t> data_;
public:
  explicit Memory(size_t size) : data_(size, 0) {}
  std::optional<uint32_t> read32(uint32_t addr) const;
  bool write32(uint32_t addr, uint32_t value);
  size_t size() const { return data_.size(); }
  bool load_words(const std::vector<uint32_t>& words, uint32_t base = 0);
};