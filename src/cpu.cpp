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

bool CPU::exec_b(const Instruction& instr, uint32_t& next_pc) {
  const uint32_t a = reg(instr.rs1);
  const uint32_t b = reg(instr.rs2);
  bool cond = false;
  switch(instr.funct3) {
    case 0: cond = (a == b); break; // BEQ
    case 1: cond = (a != b); break; // BNE
    case 4: cond = (static_cast<int32_t>(a) < static_cast<int32_t>(b)); break; // BLT
    case 5: cond = (static_cast<int32_t>(a) >= static_cast<int32_t>(b)); break; // BGE
    case 6: cond = (a < b); break; // BLTU
    case 7: cond = (a >= b); break; // BGEU
    default: return false;
  }
  if (cond) {
    next_pc = pc_ + static_cast<uint32_t>(instr.imm);
  }
  return true;
}

bool CPU::exec_load(const Instruction& instr) {
  const uint32_t addr = reg(instr.rs1) + static_cast<uint32_t>(instr.imm);
  uint32_t value = 0;
  switch(instr.funct3) {
    case 0: { // LB
      auto v = mem_.read8(addr);
      if (!v) return false;
      value = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(*v)));
      break;
    }
    case 1: {
      auto v = mem_.read16(addr);
      if (!v) return false;
      value = static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(*v)));
      break;
    }
    case 2: { // LW
      auto v = mem_.read32(addr);
      if (!v) return false;
      value = *v;
      break;
    }
    case 4: { // LBU
      auto v = mem_.read8(addr);
      if (!v) return false;
      value = *v;
      break;
    }
    case 5: { // LHU
      auto v = mem_.read16(addr);
      if (!v) return false;
      value = *v;
      break;
    }
    default:
      return false;
  }
  write_reg(instr.rd, value);
  return true;
}

bool CPU::exec_store(const Instruction& instr) {
  const uint32_t addr = reg(instr.rs1) + static_cast<uint32_t>(instr.imm);
  const uint32_t value = reg(instr.rs2);
  switch (instr.funct3) {
    case 0: return mem_.write8(addr, static_cast<uint8_t>(value));    // SB
    case 1: return mem_.write16(addr, static_cast<uint16_t>(value));  // SH
    case 2: return mem_.write32(addr, value);                         // SW
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
    case 0x6F: // JAL
      write_reg(instr.rd, next_pc);
      next_pc = pc_ + static_cast<uint32_t>(instr.imm);
      return true;
    case 0x67: { // JALR
      if (instr.funct3 != 0) return false;
      const uint32_t target = (reg(instr.rs1) + static_cast<uint32_t>(instr.imm)) & ~1u;
      write_reg(instr.rd, next_pc);
      next_pc = target;
      return true;
    }
    default: return false;
    case 0x63: return exec_b(instr, next_pc);
    case 0x03: return exec_load(instr);
    case 0x23: return exec_store(instr);
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
