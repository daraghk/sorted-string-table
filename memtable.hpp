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
};

template <typename K, typename V>
class Memtable
{
public:
    Memtable(const unsigned int capacity, const string memtable_file_path, const MemtableConfig &memtable_config) : capacity(capacity),
                                                                                                                    table(),
                                                                                                                    current_size(0),
                                                                                                                    memtable_file_path(memtable_file_path),
                                                                                                                    current_key_offsets(nullopt),
                                                                                                                    memtable_config(memtable_config) {}
    int get_capacity() { return capacity; }
    int get_size() { return current_size; }
    optional<vector<pair<K, int>>> get_key_offsets() { return current_key_offsets; };
    void insert(const K key, const V value);
    optional<V> find(const K key);
    vector<pair<K, V>> get_all_elements();

private:
    const unsigned int capacity;
    int current_size;
    map<K, V> table;
    MemtableConfig memtable_config;
    string memtable_file_path;
    optional<vector<pair<K, int>>> current_key_offsets;
    void write_to_file();
};

template <typename K, typename V>
void Memtable<K, V>::insert(const K key, const V value)
{
    table.insert({key, value});
    ++current_size;
    if (current_size == capacity)
    {
        write_to_file();
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
vector<pair<K, V>> Memtable<K, V>::get_all_elements()
{
    vector<pair<K, V>> key_value_pairs;
    for (auto it = table.begin(); it != table.end(); ++it)
    {
        key_value_pairs.push_back(make_pair(it->first, it->second));
    }
    return key_value_pairs;
}

template <typename K, typename V>
void Memtable<K, V>::write_to_file()
{
    auto all_elements = get_all_elements();
    const auto number_of_elements = all_elements.size();

    ofstream output_file(memtable_file_path);
    vector<pair<K, int>> key_offsets;

    auto index = 0;
    auto accumulated_size = 0;

    for (const auto &[key, value] : all_elements)
    {
        string line_to_write;
        if (index != 0 && index != number_of_elements - 1 && index % memtable_config.key_offset_frequency == 0)
        {
            line_to_write = fmt::format("&{}:{}\n", key, value);
            // push accumulated size so far as offset
            key_offsets.push_back(make_pair(key, accumulated_size));
        }
        else
        {
            line_to_write = fmt::format("{}:{}\n", key, value);
        }
        auto size_of_line = line_to_write.size();
        accumulated_size += size_of_line;
        ++index;
        output_file << line_to_write;
    }
    current_key_offsets = key_offsets.size() > 0 ? optional{key_offsets} : nullopt;
}