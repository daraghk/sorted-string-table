#include <map>
#include <optional>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <fmt/core.h>

using namespace std;

struct MemtableConfig
{
    char key_value_delimeter = ':';
    char key_offset_indicator = '&';
    unsigned int key_offset_frequency = 5;
    unsigned int capacity;
    string file_path;
};

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

    void write_data_to_file();
    string key_value_string_to_write(K key, V value);
    string key_value_offset_string_to_write(K key, V value);
};

template <typename K, typename V>
void Memtable<K, V>::insert(const K key, const V value)
{
    table.insert({key, value});
    ++current_size;
    if (current_size == memtable_config.capacity)
    {
        write_data_to_file();
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

template <typename K, typename V>
void Memtable<K, V>::write_data_to_file()
{
    const auto all_key_value_pairs = get_all_key_value_pairs();
    const auto number_of_key_value_pairs = all_key_value_pairs.size();

    ofstream output_file(memtable_config.file_path);
    vector<pair<K, int>> key_offsets;

    auto index = 0;
    auto end_index = number_of_key_value_pairs - 1;
    auto accumulated_offset_from_start = 0;

    for (const auto &[key, value] : all_key_value_pairs)
    {
        string key_value_line_to_write;
        if (is_key_offset_index(index, end_index))
        {
            key_value_line_to_write = key_value_offset_string_to_write(key, value);
            key_offsets.push_back(make_pair(key, accumulated_offset_from_start));
        }
        else
        {
            key_value_line_to_write = key_value_string_to_write(key, value);
        }
        auto size_of_line = key_value_line_to_write.size();
        accumulated_offset_from_start += size_of_line;
        ++index;
        output_file << key_value_line_to_write;
    }
    current_key_offsets = key_offsets.size() > 0 ? optional{key_offsets} : nullopt;
}

template <typename K, typename V>
string Memtable<K, V>::key_value_string_to_write(K key, V value)
{
    return fmt::format("{}{}{}\n", key, memtable_config.key_value_delimeter, value);
}

template <typename K, typename V>
string Memtable<K, V>::key_value_offset_string_to_write(K key, V value)
{
    return fmt::format("{}{}{}{}\n", memtable_config.key_offset_indicator, key, memtable_config.key_value_delimeter, value);
}

template <typename K, typename V>
bool Memtable<K, V>::is_key_offset_index(int index, int end_index)
{
    return index != 0 && index != end_index && index % memtable_config.key_offset_frequency == 0;
}