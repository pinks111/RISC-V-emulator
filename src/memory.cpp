#include "memory.hpp"


std::optional<uint32_t> Memory::read32(uint32_t addr) const {
  if (static_cast<std::size_t>(addr) + 4 > data_.size()) {
    return std::nullopt;
  }
  uint32_t value = 0;
  for (int i = 0; i < 4; ++i) {
    value |= static_cast<uint32_t>(data_[addr + i]) << (8 * i);
  }
  return value;
}

bool Memory::write32(uint32_t addr, uint32_t value) {
  if (static_cast<std::size_t>(addr) + 4 > data_.size()) {
    return false;
  }
  for (int i = 0; i < 4; ++i) {
    data_[addr + i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
  }
  return true;
}