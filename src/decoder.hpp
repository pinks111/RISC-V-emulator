#pragma once

#include <cstdint>

enum class Type {
    R,
    I,
    S,
    B,
    U,
    J
};

struct Instruction {
    uint32_t opcode;
    uint32_t rd;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t funct3;
    uint32_t funct7;
    uint32_t imm;
    Type type;
};

std::optional<Type> get_type(uint32_t opcode);

int32_t imm_i(uint32_t raw);
int32_t imm_s(uint32_t raw);
int32_t imm_b(uint32_t raw);
int32_t imm_u(uint32_t raw);
int32_t imm_j(uint32_t raw);

std::optional<Instruction> decode(uint32_t raw);