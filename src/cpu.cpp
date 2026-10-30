#include "cpu.hpp"

CPU::CPU(size_t size, std::ostream& out) : mem_(size), out_(out) {}

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
      case 0x01: return exec_m(instr);
    default: return false;
  }
}

bool CPU::exec_m(const Instruction& instr) {
  const uint32_t a = reg(instr.rs1);
  const uint32_t b = reg(instr.rs2);
  const int32_t sa = static_cast<int32_t>(a);
  const int32_t sb = static_cast<int32_t>(b);
  uint32_t res = 0;
  switch (instr.funct3) {
    case 0: // MUL
      res = a * b;
      break;
    case 1: //MULH
      res = static_cast<uint32_t>((static_cast<int64_t>(sa) * sb) >> 32);
      break;
    case 2: // MULHSU
      res = static_cast<uint32_t>((static_cast<int64_t>(sa) * static_cast<int64_t>(b)) >> 32);
      break;
    case 3: // MULHU
      res = static_cast<uint32_t>((static_cast<uint64_t>(a) * b) >> 32);
      break;
    case 4: // DIV
      if (b == 0) {
        res = 0xFFFFFFFFu;
      }
      else if (sa == INT32_MIN && sb == -1) {
        res = a;
      }
      else {
        res = static_cast<uint32_t>(sa / sb);
      }
      break;
    case 5: // DIVU
      res = (b == 0) ? 0xFFFFFFFFu : a / b;
      break;
    case 6: // REM
      if (b == 0) {
        res = a;
      }
      else if (sa == INT32_MIN && sb == -1) {
        res = 0;
      }
      else {
        res = static_cast<uint32_t>(sa % sb);
      }
      break;
    case 7: // REMU
      res = (b == 0) ? a : a % b;
      break;
  }
  write_reg(instr.rd, res);
  return true;
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

void CPU::sys_write() {
  const uint32_t fd = reg(10);
  const uint32_t buf = reg(11);
  const uint32_t count = reg(12);
  const uint32_t error = 0xFFFFFFFFu;
  const size_t size = mem_.size();
  if (fd != 1 || buf >= size || count > size - buf) {
    write_reg(10, error);
    return;
  }
  std::string data;
  data.reserve(count);
  for (uint32_t i = 0; i < count; ++i) {
    auto byte = mem_.read8(buf + i);
    if (!byte) {
      write_reg(10, error);
      return;
    }
    data.push_back(static_cast<char>(*byte));
  }
  std::ostream& os = (fd == 1) ? out_ : std::cerr;
  os << data;
  write_reg(10, count);
}

bool CPU::exec_sys(const Instruction& instr) {
  if (instr.funct3 != 0 || instr.rd != 0 || instr.rs1 != 0 || instr.imm != 0) {
    return false;
  }
  switch (reg(17)) {
    case 93: // exit
      exit_code_ = static_cast<int>(reg(10));
      exited_ = true;
      halted_ = true;
      return true;
    case 64: // write
      sys_write();
      return true;
    default:
      return false;
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
    case 0x73: return exec_sys(instr);
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
