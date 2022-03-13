#include "memtable.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include "memtable_search.hpp"

using namespace memtable_search_functions;

template <typename T>
concept IsStringLike = is_convertible_v<T, string_view>;

template <typename T>
concept IsNumber = is_arithmetic_v<T>;

template <typename T>
concept IsNumberOrStringLike = IsStringLike<T> || IsNumber<T>;

template <typename K, typename V>
requires IsStringLike<K> && IsNumberOrStringLike<V>
class SortedStringTable
{
public:
    SortedStringTable(const MemtableConfig memtable_config) : memtable(memtable_config),
                                                              current_size(0),
                                                              memtable_config(memtable_config){};
    int get_size() { return current_size; }
    void insert(const K key, const V value);
    optional<V> find(const K key);

private:
    int current_size;
    MemtableConfig memtable_config;
    Memtable<K, V> memtable;
};

template <typename K, typename V>
void SortedStringTable<K, V>::insert(const K key, const V value)
{
    memtable.insert(key, value);
    ++current_size;
}

template <typename K, typename V>
optional<V> SortedStringTable<K, V>::find(const K key)
{
    const auto memtable_find_result = memtable.find(key);
    if (memtable_find_result.has_value())
    {
        return memtable_find_result;
    }

    const auto offsets = memtable.get_key_offsets();
    if (offsets.has_value())
    {
        const auto starting_point = memtable_search_functions::determine_search_start_point<K, V>(key, offsets.value());
        return memtable_search_functions::linear_search_over_memtable_file_segment<K, V>(memtable_config, key, starting_point);
    }
    return memtable_search_functions::linear_search_over_memtable_file_segment<K,V>(memtable_config, key, 0);
}
