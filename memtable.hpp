#include <map>
#include <optional>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <fmt/core.h>

using namespace std;

template <typename K, typename V>
class Memtable
{
public:
    Memtable(unsigned int capacity, string memtable_file_path) : capacity(capacity),
                                                                 table(),
                                                                 current_size(0),
                                                                 memtable_file_path(memtable_file_path) {}
    int get_capacity() { return capacity; }
    int get_size() { return current_size; }
    void insert(const K key, const V value);
    optional<V> find(const K key);
    vector<pair<K, V>> get_all_elements();

private:
    const unsigned int capacity;

    //TODO update below so this can be passed in the constructor.
    const unsigned int key_offset_frequency = 5;
    int current_size;
    map<K, V> table;
    string memtable_file_path;
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

    auto index = 0;
    vector<pair<K, int>> key_offsets;
    auto accumulated_size = 0;
    for (const auto &[key, value] : all_elements)
    {
        string line_to_write;
        if (index != 0 && index != number_of_elements - 1 && index % key_offset_frequency == 0)
        {
            line_to_write = fmt::format("&{}:{}\n", key, value);
            auto size_of_line = line_to_write.size();
            accumulated_size += size_of_line;
            key_offsets.push_back(make_pair(key, accumulated_size));
        }
        else
        {
            line_to_write = fmt::format("{}:{}\n", key, value);
            auto size_of_line = line_to_write.size();
            accumulated_size += size_of_line;
        }
        ++index;
        output_file << line_to_write;
    }
}