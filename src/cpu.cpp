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
    case 1: // SLLI
      if (instr.funct7 != 0) return false;
      write_reg(instr.rd, reg(instr.rs1) << (instr.imm & 0x1F));
      return true;
    case 2: // SLTI
      write_reg(instr.rd, (static_cast<int32_t>(reg(instr.rs1)) < instr.imm) ? 1 : 0);
      return true;
    case 3: // SLTIU
      write_reg(instr.rd, (reg(instr.rs1) < static_cast<uint32_t>(instr.imm)) ? 1 : 0);
      return true;
    case 4: // XORI
      write_reg(instr.rd, reg(instr.rs1) ^ static_cast<uint32_t>(instr.imm));
      return true;
    case 5: // SRLI or SRAI
      if (instr.funct7 == 0) {
        write_reg(instr.rd, reg(instr.rs1) >> (instr.imm & 0x1F));
        return true;
      }
      if (instr.funct7 == 0x20) {
        write_reg(instr.rd, static_cast<uint32_t>(static_cast<int32_t>(reg(instr.rs1)) >> (instr.imm & 0x1F)));
        return true;
      }
      return false;
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
  switch (instr.funct7) {
    case 0x00:
      switch (instr.funct3) {
        case 0: // ADD
          write_reg(instr.rd, reg(instr.rs1) + reg(instr.rs2));
          return true;
        case 1: // SLL
          write_reg(instr.rd, reg(instr.rs1) << (reg(instr.rs2) & 0x1F));
          return true;
        case 2: // SLT
          write_reg(instr.rd, (static_cast<int32_t>(reg(instr.rs1)) < static_cast<int32_t>(reg(instr.rs2))) ? 1 : 0);
          return true;
        case 3: // SLTU
          write_reg(instr.rd, (reg(instr.rs1) < reg(instr.rs2)) ? 1 : 0);
          return true;
        case 4: // XOR
          write_reg(instr.rd, reg(instr.rs1) ^ reg(instr.rs2));
          return true;
        case 5: // SRL
          write_reg(instr.rd, reg(instr.rs1) >> (reg(instr.rs2) & 0x1F));
          return true;
        case 6: // OR
          write_reg(instr.rd, reg(instr.rs1) | reg(instr.rs2));
          return true;
        case 7: // AND
          write_reg(instr.rd, reg(instr.rs1) & reg(instr.rs2));
          return true;
        default: return false;
      }
    case 0x20:
      if (instr.funct3 == 0) { // SUB
        write_reg(instr.rd, reg(instr.rs1) - reg(instr.rs2));
        return true;
      }
      if (instr.funct3 == 5) { // SRA
        write_reg(instr.rd, static_cast<uint32_t>(static_cast<int32_t>(reg(instr.rs1)) >> (reg(instr.rs2) & 0x1F)));
        return true;
      }
      return false;
    default: return false;
  }
}

bool CPU::execute(const Instruction& instr, uint32_t& next_pc) {
  switch (instr.opcode) {
    case 0x13: return exec_i(instr);
    case 0x33: return exec_r(instr);
    case 0x37: // LUI
      write_reg(instr.rd, static_cast<uint32_t>(instr.imm));
      return true;
    case 0x17: // AUIPC
      write_reg(instr.rd, pc_ + static_cast<uint32_t>(instr.imm));
      return true;
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
