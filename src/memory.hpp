#pragma once

#include <cstdint>
#include <optional>
#include <vector>

class Memory {
  std::vector<uint8_t> data_;
  
  std::optional<uint32_t> read(uint32_t addr, unsigned size) const;
  bool write(uint32_t addr, uint32_t value, unsigned size);
public:
  explicit Memory(size_t size) : data_(size, 0) {}
  

  std::optional<uint8_t> read8(uint32_t addr) const;
  std::optional<uint16_t> read16(uint32_t addr) const;
  std::optional<uint32_t> read32(uint32_t addr) const;

  bool write8(uint32_t addr, uint8_t value);
  bool write16(uint32_t addr, uint16_t value);
  bool write32(uint32_t addr, uint32_t value);

  size_t size() const { return data_.size(); }

  bool load_words(const std::vector<uint32_t>& words, uint32_t base = 0);
  bool load_bytes(const std::vector<uint8_t>& bytes, uint32_t base = 0);
};