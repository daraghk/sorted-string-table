#include <gtest/gtest.h>
#include "memtable.hpp"

TEST(MemtableTests, BasicInsertion)
{
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

TEST(MemtableTests, BasicFind)
{
  Memtable<string, int> memtable(2);
  EXPECT_EQ(memtable.get_capacity(), 2);
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
  Memtable<int, string> memtable(2);
  EXPECT_EQ(memtable.get_capacity(), 2);
  EXPECT_EQ(memtable.get_size(), 0);

  //key insert order 2-1
  memtable.insert(2, "abc");
  memtable.insert(1, "def");

  //key retrieved order should be 1-2
  auto all_elements = memtable.get_all_elements();
  const auto first_element = all_elements.at(0);
  const auto second_element = all_elements.at(1);

  EXPECT_EQ(first_element.first, 1);
  EXPECT_EQ(first_element.second, "def");

  EXPECT_EQ(second_element.first, 2);
  EXPECT_EQ(second_element.second, "abc");
}