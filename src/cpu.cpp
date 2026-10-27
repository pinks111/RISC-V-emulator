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

bool CPU::exec_i(const Instruction& instr) {
  switch (instr.funct3) {
    case 0: // ADDI
      write_reg(instr.rd, reg(instr.rs1) + static_cast<uint32_t>(instr.imm));
      return true;
    case 4: // XORI
      write_reg(instr.rd, reg(instr.rs1) ^ static_cast<uint32_t>(instr.imm));
      return true;
    case 6: // ORI
      write_reg(instr.rd, reg(instr.rs1) | static_cast<uint32_t>(instr.imm));
      return true;
    case 7: // ANDI
      write_reg(instr.rd, reg(instr.rs1) & static_cast<uint32_t>(instr.imm));
      return true;
    default:
      return false;
  }
}

bool CPU::exec_r(const Instruction& instr) {
  switch (instr.funct3) {
    case 0:
      switch (instr.funct7) {
        case 0x00: // ADD
          write_reg(instr.rd, reg(instr.rs1) + reg(instr.rs2));
          return true;
        case 0x20: // SUB
          write_reg(instr.rd, reg(instr.rs1) - reg(instr.rs2));
          return true;
        default:
          return false;
      }
    case 4:
      if (instr.funct7 == 0x00) { // XOR
        write_reg(instr.rd, reg(instr.rs1) ^ reg(instr.rs2));
        return true;
      }
      return false;
    case 6:
      if (instr.funct7 == 0x00) { // OR
        write_reg(instr.rd, reg(instr.rs1) | reg(instr.rs2));
        return true;
      }
      return false;
    case 7:
      if (instr.funct7 == 0x00) { // AND
        write_reg(instr.rd, reg(instr.rs1) & reg(instr.rs2));
        return true;
      }
      return false;
    default:
      return false;
  }
}

bool CPU::execute(const Instruction& instr, uint32_t& next_pc) {
  switch (instr.opcode) {
    case 0x13: return exec_i(instr);
    case 0x33: return exec_r(instr);
    default: return false;
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

