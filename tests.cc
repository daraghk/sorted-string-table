#include <gtest/gtest.h>
#include "sorted_string_table.hpp"

TEST(MemtableTests, BasicInsertion)
{
  MemtableConfig memtable_config;
  memtable_config.file_path = "./_test_";
  memtable_config.capacity = 3;
  Memtable<string, int> memtable(memtable_config);
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
  MemtableConfig memtable_config;
  memtable_config.file_path = "./_test_";
  memtable_config.capacity = 3;

  Memtable<string, int> memtable(memtable_config);
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
  MemtableConfig memtable_config;
  memtable_config.file_path = "./_test_";
  memtable_config.capacity = 3;

  Memtable<int, string> memtable(memtable_config);
  EXPECT_EQ(memtable.get_capacity(), 3);
  EXPECT_EQ(memtable.get_size(), 0);

  // key insert order 2-1
  memtable.insert(2, "abc");
  memtable.insert(1, "def");

  // key retrieved order should be 1-2
  auto all_elements = memtable.get_all_key_value_pairs();
  const auto first_element = all_elements.at(0);
  const auto second_element = all_elements.at(1);

  EXPECT_EQ(first_element.first, 1);
  EXPECT_EQ(first_element.second, "def");

  EXPECT_EQ(second_element.first, 2);
  EXPECT_EQ(second_element.second, "abc");
}

TEST(MemtableTests, InsertionBeyondCapacity)
{
  MemtableConfig memtable_config;
  memtable_config.file_path = "./_test_";
  memtable_config.capacity = 2;

  Memtable<string, int> memtable(memtable_config);
  EXPECT_EQ(memtable.get_capacity(), 2);
  EXPECT_EQ(memtable.get_size(), 0);

  memtable.insert("first", 1);
  memtable.insert("second", 2);

  // two elements added, table should be cleared now again as it reached max capacity
  // and have size = 0 with no elements as these should be written to file
  EXPECT_EQ(memtable.get_size(), 0);
  EXPECT_EQ(memtable.get_all_key_value_pairs().size(), 0);
}

TEST(MemtableTests, LargeNumberOfElements)
{
  unsigned int large_capacity = 100000;
  MemtableConfig memtable_config;
  memtable_config.file_path = "./memtable_test_output";
  memtable_config.capacity = large_capacity;

  Memtable<string, int> memtable(memtable_config);
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

TEST(SortedStringTableTests, BasicInsertion)
{
  MemtableConfig memtable_config;
  memtable_config.file_path = "./_test_";
  memtable_config.capacity = 10;

  SortedStringTable<string, int> ss_table(memtable_config);

  ss_table.insert("first", 1);
  ss_table.insert("second", 2);

  EXPECT_EQ(ss_table.get_size(), 2);
}

TEST(SortedStringTableTests, BasicFindOnMemtable)
{
  MemtableConfig memtable_config;
  memtable_config.file_path = "./_test_";
  memtable_config.capacity = 10;

  SortedStringTable<string, int> ss_table(memtable_config);

  ss_table.insert("first", 1);
  ss_table.insert("second", 2);

  EXPECT_EQ(ss_table.get_size(), 2);

  auto find_result = ss_table.find("first");
  EXPECT_TRUE(find_result.has_value());
  EXPECT_EQ(find_result.value(), 1);
}

TEST(SortedStringTableTests, BasicFindsOnMostRecentMemtableWrite)
{
  MemtableConfig memtable_config;
  memtable_config.file_path = "./_test_";
  memtable_config.capacity = 2;

  SortedStringTable<string, int> ss_table(memtable_config);

  ss_table.insert("first", 1);
  ss_table.insert("second", 2);

  EXPECT_EQ(ss_table.get_size(), 2);

  auto first_find_result = ss_table.find("first");
  EXPECT_TRUE(first_find_result.has_value());
  EXPECT_EQ(first_find_result.value(), 1);

  auto second_find_result = ss_table.find("second");
  EXPECT_TRUE(second_find_result.has_value());
  EXPECT_EQ(second_find_result.value(), 2);
}

TEST(MemtableTests, RetrieveKeyOffsets)
{
  unsigned int large_capacity = 100'000;
  MemtableConfig memtable_config;
  memtable_config.file_path = "./memtable_test_output";
  memtable_config.capacity = large_capacity;

  Memtable<string, int> memtable(memtable_config);
  EXPECT_EQ(memtable.get_capacity(), large_capacity);
  EXPECT_EQ(memtable.get_size(), 0);

  // insert up to one below capacity
  for (int i = 0; i < large_capacity - 1; ++i)
  {
    memtable.insert(to_string(i), i);
  }

  // insert max_capacity element to trigger writing the memtable to file
  memtable.insert(to_string(large_capacity), large_capacity);
  EXPECT_EQ(memtable.get_capacity(), large_capacity);
  EXPECT_EQ(memtable.get_size(), 0);

  auto key_offsets = memtable.get_key_offsets();
  EXPECT_TRUE(key_offsets.has_value());
}

TEST(SortedStringTableTests, SearchSegmentsOnWrittenMemtableFile)
{
  unsigned int large_capacity = 100'000;
  MemtableConfig memtable_config;
  memtable_config.file_path = "./memtable_test_output";
  memtable_config.capacity = large_capacity;
  
  SortedStringTable<string, int> ss_table(memtable_config);

  // insert up to one below capacity
  for (int i = 0; i < large_capacity - 1; ++i)
  {
    ss_table.insert(to_string(i), i);
  }

  // insert max_capacity element to trigger writing the memtable to file
  ss_table.insert(to_string(large_capacity), large_capacity);

  // assert correct size
  EXPECT_EQ(ss_table.get_size(), large_capacity);

  // perform find for key which should be first in the memtable file
  auto zero_find = ss_table.find("0");
  EXPECT_TRUE(zero_find.has_value());
  EXPECT_EQ(zero_find.value(), 0);

  // perform find for key which should be in first segment of memtable file
  auto first_find = ss_table.find("100");
  EXPECT_TRUE(first_find.has_value());
  EXPECT_EQ(first_find.value(), 100);

  // perform find for key which should not be in first segment of memtable file
  auto second_find = ss_table.find("2");
  EXPECT_TRUE(second_find.has_value());
  EXPECT_EQ(second_find.value(), 2);

  // perform find for key which will be used as a key offset
  auto third_find = ss_table.find("200");
  EXPECT_TRUE(third_find.has_value());
  EXPECT_EQ(third_find.value(), 200);
}