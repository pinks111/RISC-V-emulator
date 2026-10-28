#include "memory.hpp"

#include <algorithm>

std::optional<uint32_t> Memory::read(uint32_t addr, unsigned size) const {
  if (!(size <= data_.size() && addr <= data_.size() - size)) {
    return std::nullopt;
  }
  uint32_t value = 0;
  for (unsigned i = 0; i < size; ++i) {
    value |= static_cast<uint32_t>(data_[addr + i]) << (8 * i);
  }
  return value;
}

bool Memory::write(uint32_t addr, uint32_t value, unsigned size) {
  if (!(size <= data_.size() && addr <= data_.size() - size)) {
    return false;
  }
  for (unsigned i = 0; i < size; ++i) {
    data_[addr + i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
  }
  return true;
}

std::optional<uint8_t> Memory::read8(uint32_t addr) const {
  auto result = read(addr, 1);
  if (!result) {
    return std::nullopt;
  }
  return static_cast<uint8_t>(*result);
}

std::optional<uint16_t> Memory::read16(uint32_t addr) const {
  auto result = read(addr, 2);
  if (!result) {
    return std::nullopt;
  }
  return static_cast<uint16_t>(*result);
}

std::optional<uint32_t> Memory::read32(uint32_t addr) const {
  return read(addr, 4);
}

bool Memory::write8(uint32_t addr, uint8_t value)   { return write(addr, value, 1); }
bool Memory::write16(uint32_t addr, uint16_t value) { return write(addr, value, 2); }
bool Memory::write32(uint32_t addr, uint32_t value) { return write(addr, value, 4); }

bool Memory::load_words(const std::vector<uint32_t>& words, uint32_t base) {
  for (size_t i = 0; i < words.size(); ++i) {
    if (!write32(base + static_cast<uint32_t>(i * 4), words[i])) {
      return false;
    }
  }
  return true;
}

bool Memory::load_bytes(const std::vector<uint8_t>& bytes, uint32_t base) {
  if (!(bytes.size() <= data_.size() && base <= data_.size() - bytes.size())) {
    return false;
  }
  std::copy(bytes.begin(), bytes.end(), data_.begin() + static_cast<std::ptrdiff_t>(base));
  return true;
}