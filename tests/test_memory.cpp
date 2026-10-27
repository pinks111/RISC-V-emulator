#include <gtest/gtest.h>
#include "memory.hpp"

TEST(Memory, LittleEndian) {
  Memory mem(16);
  ASSERT_TRUE(mem.write32(0, 0x11223344));
  EXPECT_EQ(mem.read32(0), 0x11223344u);
  mem.write32(4, 0x000000AA);
  EXPECT_EQ(mem.read32(4), 0xAAu);
}

TEST(Memory, OutOfRange) {
  Memory mem(16);
  EXPECT_FALSE(mem.write32(14, 1));
  EXPECT_FALSE(mem.read32(16).has_value());
  EXPECT_FALSE(mem.read32(0xFFFFFFFF).has_value());
}