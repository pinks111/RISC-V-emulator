#pragma once

#include <array>
#include <cstdint>

#include "memory.hpp"
#include "decoder.hpp"

class CPU {
  std::array<uint32_t, 32> registers_{};
  uint32_t pc_ = 0;
  bool halted_ = false;
  Memory mem_;
  
  void write_reg(int index, uint32_t value);
  bool execute(const Instruction& instr, uint32_t& next_pc);
  bool exec_i(const Instruction& instr);
  bool exec_r(const Instruction& instr);
public:
  explicit CPU(size_t size = 64 * 1024);
  bool step();
  uint32_t reg(int index) const;
  uint32_t pc() const { return pc_; }
  bool is_halted() const { return halted_; }
  Memory& memory() { return mem_; }
};