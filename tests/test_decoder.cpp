#include <gtest/gtest.h>
#include "decoder.hpp"

TEST(Decoder, AddiPositive) {
  auto inst = decode(0x00500093);   // addi x1, x0, 5
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->rd, 1);
  EXPECT_EQ(inst->rs1, 0);
  EXPECT_EQ(inst->imm, 5);
}

TEST(Decoder, AddiNegative) {
  auto inst = decode(0xFFF00093);   // addi x1, x0, -1
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->imm, -1);
}

TEST(Decoder, BranchNegative) {
  auto inst = decode(0xFE000EE3);   // beq x0, x0, -4
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->imm, -4);
}

TEST(Decoder, InvalidOpcode) {
  EXPECT_FALSE(decode(0x00000000));   // opcode 0 не существует
}

TEST(Decoder, AddRType) {
  auto inst = decode(0x002081B3);   // add x3, x1, x2
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->rd, 3);
  EXPECT_EQ(inst->rs1, 1);
  EXPECT_EQ(inst->rs2, 2);
  EXPECT_EQ(inst->funct3, 0);
  EXPECT_EQ(inst->funct7, 0);
}

TEST(Decoder, StorePositive) {
  auto inst = decode(0x0020A223);   // sw x2, 4(x1)
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->rs1, 1);
  EXPECT_EQ(inst->rs2, 2);
  EXPECT_EQ(inst->imm, 4);
}

TEST(Decoder, StoreNegative) {
  auto inst = decode(0xFE20AE23);   // sw x2, -4(x1)
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->imm, -4);
}

TEST(Decoder, Lui) {
  auto inst = decode(0x123450B7);   // lui x1, 0x12345
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->rd, 1);
  EXPECT_EQ(inst->imm, 0x12345000);
}

TEST(Decoder, JalPositive) {
  auto inst = decode(0x008000EF);   // jal x1, 8
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->rd, 1);
  EXPECT_EQ(inst->imm, 8);
}

TEST(Decoder, JalNegative) {
  auto inst = decode(0xFF9FF0EF);   // jal x1, -8
  ASSERT_TRUE(inst);
  EXPECT_EQ(inst->imm, -8);
}