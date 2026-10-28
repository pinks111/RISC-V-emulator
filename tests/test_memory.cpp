#include <gtest/gtest.h>
#include "memory.hpp"

TEST(Memory, LittleEndian) {
  Memory mem(16);
  ASSERT_TRUE(mem.write32(0, 0x11223344));
  EXPECT_EQ(mem.read32(0), 0x11223344u);
  mem.write32(4, 0x000000AA);
  EXPECT_EQ(mem.read32(4), 0xAAu);
}

TEST(Memory, ByteOrderIsLittleEndian) {
  Memory mem(16);
  ASSERT_TRUE(mem.write32(0, 0x11223344));
  EXPECT_EQ(mem.read8(0).value(), 0x44u);
  EXPECT_EQ(mem.read8(1).value(), 0x33u);
  EXPECT_EQ(mem.read8(2).value(), 0x22u);
  EXPECT_EQ(mem.read8(3).value(), 0x11u);
  EXPECT_EQ(mem.read16(0).value(), 0x3344u);
  EXPECT_EQ(mem.read16(2).value(), 0x1122u);
}

TEST(Memory, Write8DoesNotTouchNeighbours) {
  Memory mem(16);
  ASSERT_TRUE(mem.write32(0, 0xFFFFFFFF));
  ASSERT_TRUE(mem.write8(1, 0x00));
  EXPECT_EQ(mem.read32(0).value(), 0xFFFF00FFu);
}

TEST(Memory, Write16) {
  Memory mem(16);
  ASSERT_TRUE(mem.write16(2, 0xABCD));
  EXPECT_EQ(mem.read32(0).value(), 0xABCD0000u);
}

TEST(Memory, UnalignedAccessWorks) {
  Memory mem(16);
  ASSERT_TRUE(mem.write32(1, 0xAABBCCDD));
  EXPECT_EQ(mem.read32(1).value(), 0xAABBCCDDu);
  EXPECT_EQ(mem.read8(1).value(), 0xDDu);
}

TEST(Memory, OutOfRange) {
  Memory mem(16);
  EXPECT_TRUE(mem.read8(15).has_value());
  EXPECT_FALSE(mem.read8(16).has_value());
  EXPECT_FALSE(mem.read16(15).has_value());
  EXPECT_TRUE(mem.read32(12).has_value());
  EXPECT_FALSE(mem.read32(13).has_value());
  EXPECT_FALSE(mem.read32(0xFFFFFFFF).has_value());
  EXPECT_FALSE(mem.write16(15, 1));
}

TEST(Memory, FailedWriteChangesNothing) {
  Memory mem(16);
  EXPECT_FALSE(mem.write32(14, 0xFFFFFFFF));
  EXPECT_EQ(mem.read8(14).value(), 0u);
  EXPECT_EQ(mem.read8(15).value(), 0u);
}

TEST(Memory, LoadBytes) {
  Memory mem(16);
  ASSERT_TRUE(mem.load_bytes({1, 2, 3, 4, 5}, 2));
  EXPECT_EQ(mem.read8(2).value(), 1u);
  EXPECT_EQ(mem.read32(2).value(), 0x04030201u);
  EXPECT_FALSE(mem.load_bytes({1, 2, 3, 4, 5}, 14));
}