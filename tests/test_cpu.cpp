#include <gtest/gtest.h>
#include "cpu.hpp"

TEST(Cpu, InitialState) {
  CPU cpu;
  EXPECT_EQ(cpu.pc(), 0u);
  EXPECT_FALSE(cpu.is_halted());
  for (int i = 0; i < 32; ++i) {
    EXPECT_EQ(cpu.reg(i), 0u);
  }
}

TEST(Cpu, AddiAndAdd) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0x00500093,  // addi x1, x0, 5
      0x00700113,  // addi x2, x0, 7
      0x002081B3   // add  x3, x1, x2
  }));
  EXPECT_TRUE(cpu.step());
  EXPECT_TRUE(cpu.step());
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(1), 5u);
  EXPECT_EQ(cpu.reg(2), 7u);
  EXPECT_EQ(cpu.reg(3), 12u);
  EXPECT_EQ(cpu.pc(), 12u);
}

TEST(Cpu, PcAdvancesByFour) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({0x00500093, 0x00700113}));
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.pc(), 4u);
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.pc(), 8u);
}

TEST(Cpu, X0StaysZero) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({0x00500013}));  // addi x0, x0, 5
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(0), 0u);
}

TEST(Cpu, NegativeImmediate) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0xFFF00093,  // addi x1, x0, -1
      0x00108133   // add  x2, x1, x1
  }));
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(1), 0xFFFFFFFFu);
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(2), 0xFFFFFFFEu);  // -2 по модулю 2^32
}

TEST(Cpu, AdditionWrapsAround) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0xFFF00093,  // addi x1, x0, -1   (x1 = 0xFFFFFFFF)
      0x00108093   // addi x1, x1, 1
  }));
  EXPECT_TRUE(cpu.step());
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(1), 0u);
}

TEST(Cpu, HaltsOnInvalidInstruction) {
  CPU cpu;
  EXPECT_FALSE(cpu.step());
  EXPECT_TRUE(cpu.is_halted());
}

TEST(Cpu, StaysHaltedAfterStop) {
  CPU cpu;
  EXPECT_FALSE(cpu.step());
  EXPECT_TRUE(cpu.is_halted());
  EXPECT_FALSE(cpu.step());
  EXPECT_EQ(cpu.pc(), 0u);
}

TEST(Cpu, HaltsWhenPcLeavesMemory) {
  CPU cpu(4);
  ASSERT_TRUE(cpu.memory().load_words({0x00500093}));
  EXPECT_TRUE(cpu.step());
  EXPECT_FALSE(cpu.step());
  EXPECT_TRUE(cpu.is_halted());
}

// Execute Tests

TEST(Cpu, SubAndLogic) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0x00500093,  // addi x1, x0, 5
      0x00300113,  // addi x2, x0, 3
      0x402081B3,  // sub  x3, x1, x2   -> 2
      0x0020F233,  // and  x4, x1, x2   -> 1
      0x0020E2B3,  // or   x5, x1, x2   -> 7
      0x0020C333   // xor  x6, x1, x2   -> 6
  }));
  for (int i = 0; i < 6; ++i) EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(3), 2u);
  EXPECT_EQ(cpu.reg(4), 1u);
  EXPECT_EQ(cpu.reg(5), 7u);
  EXPECT_EQ(cpu.reg(6), 6u);
}

TEST(Cpu, ShiftRightLogicalVsArithmetic) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0xFF800093,  // addi x1, x0, -8
      0x4010D113,  // srai x2, x1, 1   -> -4
      0x0010D193   // srli x3, x1, 1   -> 0x7FFFFFFC
  }));
  for (int i = 0; i < 3; ++i) EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(2), 0xFFFFFFFCu);
  EXPECT_EQ(cpu.reg(3), 0x7FFFFFFCu);
}

TEST(Cpu, SltSignedVsUnsigned) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0xFFF00093,  // addi x1, x0, -1
      0x00100113,  // addi x2, x0, 1
      0x0020A1B3,  // slt  x3, x1, x2   -> 1 (-1 < 1)
      0x0020B233   // sltu x4, x1, x2   -> 0 (0xFFFFFFFF > 1)
  }));
  for (int i = 0; i < 4; ++i) EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(3), 1u);
  EXPECT_EQ(cpu.reg(4), 0u);
}

TEST(Cpu, ShiftAmountIsMasked) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0x00100093,  // addi x1, x0, 1
      0x02000113,  // addi x2, x0, 32
      0x002091B3   // sll  x3, x1, x2   -> 1 (сдвиг 32 & 0x1F = 0)
  }));
  for (int i = 0; i < 3; ++i) EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(3), 1u);
}

TEST(Cpu, LuiAndAuipc) {
  CPU cpu;
  ASSERT_TRUE(cpu.memory().load_words({
      0x123450B7,  // lui   x1, 0x12345  -> 0x12345000
      0x00001117   // auipc x2, 1        -> pc(4) + 0x1000 = 0x1004
  }));
  EXPECT_TRUE(cpu.step());
  EXPECT_TRUE(cpu.step());
  EXPECT_EQ(cpu.reg(1), 0x12345000u);
  EXPECT_EQ(cpu.reg(2), 0x1004u);
}