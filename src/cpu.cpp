#include "cpu.hpp"

CPU::CPU(size_t size) : mem_(size) {}

void CPU::write_reg(int index, uint32_t value) {
  if (index != 0) {
    registers_[index] = value;
  }
}

uint32_t CPU::reg(int index) const {
  return registers_[index];
}

bool CPU::execute(const Instruction& instr, uint32_t& next_pc) {
  switch (instr.opcode) {
    case 0x13: // I
      if (instr.funct3 == 0) {
        write_reg(instr.rd, reg(instr.rs1) + static_cast<uint32_t>(instr.imm));
        return true;
      }
      return false;
    case 0x33: // R
      if (instr.funct3 == 0 && instr.funct7 == 0) {
        write_reg(instr.rd, reg(instr.rs1) + reg(instr.rs2));
        return true;
      }
      return false;
    default:
      return false;
  }
}

bool CPU::step() {
  if (halted_) {
    return false;
  }
  auto word = mem_.read32(pc_);
  if (!word) {
    halted_ = true;
    return false;
  }
  auto instr = decode(*word);
  if (!instr) {
    halted_ = true;
    return false;
  }
  uint32_t next_pc = pc_ + 4;
  if (!execute(*instr, next_pc)) {
    halted_ = true;
    return false;
  }
  pc_ = next_pc;
  return true;
}

