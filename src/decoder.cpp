#include <optional>
#include "decoder.hpp"

std::optional<Type> get_type(uint32_t opcode) {
  switch (opcode) {
    case 0x33: return Type::R;
    case 0x13: return Type::I;
    case 0x03: return Type::I;
    case 0x67: return Type::I;
    case 0x73: return Type::I;
    case 0x23: return Type::S;
    case 0x63: return Type::B;
    case 0x37: return Type::U;
    case 0x17: return Type::U;
    case 0x6F: return Type::J;
    default: return std::nullopt;
  }
}


int32_t imm_i(uint32_t raw) {
  return static_cast<int32_t>(raw) >> 20;
}
int32_t imm_s(uint32_t raw) {
  return static_cast<int32_t>(((raw & 0xFE000000) >> 20 | ((raw >> 7) & 0x1F)) << 20) >> 20;
}
int32_t imm_b(uint32_t raw) {
  uint32_t b = ((raw >> 31) << 12)
             | (((raw >> 7)  & 0x1)  << 11)
             | (((raw >> 25) & 0x3F) << 5)
             | (((raw >> 8)  & 0xF)  << 1);
  return static_cast<int32_t>(b << 19) >> 19; 
}
int32_t imm_u(uint32_t raw) {
  return static_cast<int32_t>(raw & 0xFFFFF000);
}
int32_t imm_j(uint32_t raw) {
  uint32_t j = ((raw >> 31) << 20)
             | (((raw >> 12) & 0xFF) << 12)
             | (((raw >> 20) & 0x1) << 11)
             | (((raw >> 21) & 0x3FF) << 1);
  return static_cast<int32_t>(j << 11) >> 11;
}

std::optional<Instruction> decode(uint32_t raw) {
  Instruction inst{};
  inst.opcode = raw & 0x7F;
  inst.rd = (raw >> 7) & 0x1F;
  inst.funct3 = (raw >> 12) & 0x7;
  inst.rs1 = (raw >> 15) & 0x1F;
  inst.rs2 = (raw >> 20) & 0x1F;
  inst.funct7 = (raw >> 25) & 0x7F;
  auto type = get_type(inst.opcode);
  if (!type) {
    return std::nullopt;
  }
  inst.type = *type;
  switch (*type) {
    case Type::R:
      inst.imm = 0;
      break;
    case Type::I:
      inst.imm = imm_i(raw);
      break;
    case Type::S:
      inst.imm = imm_s(raw);
      break;
    case Type::B:
      inst.imm = imm_b(raw);
      break;
    case Type::U:
      inst.imm = imm_u(raw);
      break;
    case Type::J:
      inst.imm = imm_j(raw);
      break;
  }
  return inst;
}