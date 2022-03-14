#include <map>
#include <optional>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <fmt/core.h>
#include "memtable_write.hpp"

using namespace memtable_write_functions;

template <typename K, typename V>
class Memtable
{
public:
    Memtable(const MemtableConfig &memtable_config) : table(),
                                                      current_size(0),
                                                      current_key_offsets(nullopt),
                                                      memtable_config(memtable_config) {}
    int get_capacity() { return memtable_config.capacity; }
    int get_size() { return current_size; }

    bool is_key_offset_index(int current_index, int end_index);
    optional<vector<pair<K, int>>> get_key_offsets() { return current_key_offsets; };

    void insert(const K key, const V value);
    optional<V> find(const K key);
    vector<pair<K, V>> get_all_key_value_pairs();

private:
    int current_size;
    map<K, V> table;
    MemtableConfig memtable_config;

    optional<vector<pair<K, int>>> current_key_offsets;
};

template <typename K, typename V>
void Memtable<K, V>::insert(const K key, const V value)
{
    table.insert({key, value});
    ++current_size;
    if (current_size == memtable_config.capacity)
    {
        const auto all_key_value_pairs = get_all_key_value_pairs();
        current_key_offsets = memtable_write_functions::write_data_to_file<K,V>(memtable_config, all_key_value_pairs);
        table.clear();
        current_size = 0;
    }
}

template <typename K, typename V>
optional<V> Memtable<K, V>::find(const K key)
{
    const auto find_result = table.find(key);
    if (find_result != table.end())
    {
        const auto value = find_result->second;
        return optional<V>{value};
    }
    return nullopt;
}

template <typename K, typename V>
vector<pair<K, V>> Memtable<K, V>::get_all_key_value_pairs()
{
    vector<pair<K, V>> key_value_pairs;
    for (auto it = table.begin(); it != table.end(); ++it)
    {
        key_value_pairs.push_back(make_pair(it->first, it->second));
    }
    return key_value_pairs;
}