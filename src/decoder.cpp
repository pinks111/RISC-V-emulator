#include <optional>

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

int32_t imm_r(uint32_t raw) {
  return 0;
}
int32_t imm_i(uint32_t raw) {
  return static_cast<int32_t>(raw) >> 20;
}
int32_t imm_s(uint32_t raw) {
  return static_cast<int32_t>((raw & 0xFE000000) >> 20 | ((raw >> 7) & 0x1F));
}
int32_t imm_b(uint32_t raw) {
  return static_cast<int32_t>(((raw >> 31) << 12) | 
                             (((raw >> 7) & 0x1) << 11) | 
                             (((raw >> 25) & 0x3F) << 5) | 
                             (((raw >> 8) & 0xF) << 1));
}
int32_t imm_u(uint32_t raw) {
  return static_cast<int32_t>(raw & 0xFFFFF000);
}
int32_t imm_j(uint32_t raw) {
  return static_cast<int32_t>(((raw >> 31) << 20) | 
                             (((raw >> 12) & 0xFF) << 12) | 
                             (((raw >> 20) & 0x1) << 11) | 
                             (((raw >> 21) & 0x3FF) << 1));
}

std::optional<Instruction> decode(uint32_t raw) {
  Instruction inst{};
  constexpr uint32_t opcode = raw & 0x7F;
  constexpr uint32_t rd = (raw >> 7) & 0x1F;
  constexpr uint32_t rs1 = (raw >> 15) & 0x1F;
  constexpr uint32_t rs2 = (raw >> 20) & 0x1F;
  constexpr uint32_t funct3 = (raw >> 12) & 0x7;
  constexpr uint32_t funct7 = (raw >> 25) & 0x7F;
  auto type = get_type(inst.opcode);

}