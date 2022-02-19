#include <gtest/gtest.h>
#include "memtable.hpp"

TEST(MemtableTests, BasicInsertion)
{
  Memtable<string, int> memtable(3, "./_test_");
  EXPECT_EQ(memtable.get_capacity(), 3);
  EXPECT_EQ(memtable.get_size(), 0);

  memtable.insert("first", 1);
  memtable.insert("second", 2);

  // add two elements
  EXPECT_EQ(memtable.get_capacity(), 3);
  EXPECT_EQ(memtable.get_size(), 2);
}

TEST(MemtableTests, BasicFind)
{
  Memtable<string, int> memtable(3, "./_test_");
  EXPECT_EQ(memtable.get_capacity(), 3);
  EXPECT_EQ(memtable.get_size(), 0);

  memtable.insert("first", 1);
  memtable.insert("second", 2);

  auto result_find_first = memtable.find("first");
  auto result_find_second = memtable.find("second");
  EXPECT_EQ(result_find_first.value(), 1);
  EXPECT_EQ(result_find_second.value(), 2);

  auto result_find_non_existent = memtable.find("third");
  EXPECT_FALSE(result_find_non_existent.has_value());
  EXPECT_ANY_THROW(result_find_non_existent.value());
}

TEST(MemtableTests, Ordering)
{
  Memtable<int, string> memtable(3, "./_test_");
  EXPECT_EQ(memtable.get_capacity(), 3);
  EXPECT_EQ(memtable.get_size(), 0);

  // key insert order 2-1
  memtable.insert(2, "abc");
  memtable.insert(1, "def");

  // key retrieved order should be 1-2
  auto all_elements = memtable.get_all_elements();
  const auto first_element = all_elements.at(0);
  const auto second_element = all_elements.at(1);

  EXPECT_EQ(first_element.first, 1);
  EXPECT_EQ(first_element.second, "def");

  EXPECT_EQ(second_element.first, 2);
  EXPECT_EQ(second_element.second, "abc");
}

TEST(MemtableTests, InsertionBeyondCapacity)
{
  Memtable<string, int> memtable(2, "./_test_");
  EXPECT_EQ(memtable.get_capacity(), 2);
  EXPECT_EQ(memtable.get_size(), 0);

  memtable.insert("first", 1);
  memtable.insert("second", 2);

  // two elements added, table should be cleared now again as it reached max capacity
  // and have size = 0 with no elements as these should be written to file
  EXPECT_EQ(memtable.get_size(), 0);
  EXPECT_EQ(memtable.get_all_elements().size(), 0);
}

TEST(MemtableTests, LargeNumberOfElements)
{
  unsigned int large_capacity = 100000;
  Memtable<string, int> memtable(large_capacity, "./memtable_test_output");
  EXPECT_EQ(memtable.get_capacity(), large_capacity);
  EXPECT_EQ(memtable.get_size(), 0);

  // insert up to one below capacity and check
  for (int i = 0; i < large_capacity - 1; ++i)
  {
    memtable.insert(to_string(i), i);
  }
  EXPECT_EQ(memtable.get_capacity(), large_capacity);
  EXPECT_EQ(memtable.get_size(), large_capacity - 1);

  // insert max_capacity element to trigger writing the memtable to file
  memtable.insert(to_string(large_capacity), large_capacity);
  EXPECT_EQ(memtable.get_capacity(), large_capacity);
  EXPECT_EQ(memtable.get_size(), 0);
}