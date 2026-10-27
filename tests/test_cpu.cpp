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