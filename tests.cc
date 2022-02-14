#include <gtest/gtest.h>
#include "memtable.hpp"

TEST(MemTableTests, BasicInsertion) {
  Memtable<string, int> memtable(2);
  EXPECT_EQ(memtable.get_capacity(), 2);
  EXPECT_EQ(memtable.get_size(), 0);

  memtable.insert("first", 1);
  memtable.insert("second", 2);

  //add two elements, below capacity
  EXPECT_EQ(memtable.get_capacity(), 2);
  EXPECT_EQ(memtable.get_size(), 2);

  //add another element, above capacity now, get_size should stay 2
  memtable.insert("third", 3);
  EXPECT_EQ(memtable.get_size(), 2);
}