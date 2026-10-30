#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <iostream>

#include "memory.hpp"
#include "decoder.hpp"

class CPU {
  std::array<uint32_t, 32> registers_{};
  uint32_t pc_ = 0;
  bool halted_ = false;
  Memory mem_;
  std::ostream& out_;
  int exit_code_ = 0;
  bool exited_ = false;

  
  void write_reg(int index, uint32_t value);
  bool execute(const Instruction& instr, uint32_t& next_pc);
  bool exec_i(const Instruction& instr);
  bool exec_r(const Instruction& instr);
  bool exec_b(const Instruction& instr, uint32_t& next_pc);
  bool exec_m(const Instruction& instr);

  bool exec_load(const Instruction& instr);
  bool exec_store(const Instruction& instr);

  bool exec_sys(const Instruction& instr);
  void sys_write();
public:
  explicit CPU(size_t size = 64 * 1024, std::ostream& out = std::cout);
  bool step();
  uint32_t reg(int index) const;
  uint32_t pc() const { return pc_; }
  bool is_halted() const { return halted_; }
  bool exited() const { return exited_; }
  int exit_code() const { return exit_code_; }
  Memory& memory() { return mem_; }
};