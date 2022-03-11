#include "memtable.hpp"
#include <string>
#include <sstream>
#include <iostream>

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
    SortedStringTable(unsigned int memtable_capacity, const string memtable_main_filepath, const MemtableConfig memtable_config) : memtable(memtable_capacity, memtable_main_filepath, memtable_config),
                                                                                                                                   current_size(0),
                                                                                                                                   memtable_main_filepath(memtable_main_filepath),
                                                                                                                                   memtable_config(memtable_config){};
    int get_size() { return current_size; }
    void insert(const K key, const V value);
    optional<V> find(const K key);

private:
    int current_size;
    string memtable_main_filepath;
    MemtableConfig memtable_config;
    Memtable<K, V> memtable;

    int determine_search_start_point(const K key, const vector<pair<K, int>> &memtable_offsets);
    optional<V> linear_search_over_memtable_file_segment(const string &memtable_file_path, const K key, int starting_point);
    optional<V> search_stream_for_key_until_next_offset(ifstream &stream, const K key);

    bool check_key_equality(const K key, const string &input_line, const size_t delimeter_position);
    V convert_to_correct_numerical_type(const string &value_as_string);
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
    auto memtable_find_result = memtable.find(key);
    if (memtable_find_result.has_value())
    {
        return memtable_find_result;
    }

    const auto offsets = memtable.get_key_offsets();
    if (offsets.has_value())
    {
        const auto starting_point = determine_search_start_point(key, offsets.value());
        return linear_search_over_memtable_file_segment(memtable_main_filepath, key, starting_point);
    }
    return linear_search_over_memtable_file_segment(memtable_main_filepath, key, 0);
}

template <typename K, typename V>
int SortedStringTable<K, V>::determine_search_start_point(const K key, const vector<pair<K, int>> &memtable_offsets)
{
    const K first_offset_key = memtable_offsets[0].first;
    if (key < first_offset_key)
    {
        return 0;
    }

    for (auto i = 1; i < memtable_offsets.size(); ++i)
    {
        const K current_offset_key = memtable_offsets[i].first;
        if (key < current_offset_key)
        {
            return memtable_offsets[i - 1].second;
        }
    }
    return 0;
}

template <typename K, typename V>
requires IsStringLike<K> && IsNumberOrStringLike<V>
    optional<V> SortedStringTable<K, V>::linear_search_over_memtable_file_segment(const string &memtable_file_path, const K key, int starting_point)
{
    ifstream most_recent_memtable_write(memtable_file_path);
    most_recent_memtable_write.seekg(starting_point);
    const auto search_result = search_stream_for_key_until_next_offset(most_recent_memtable_write, key);
    return search_result;
}

template <typename K, typename V>
optional<V> SortedStringTable<K, V>::search_stream_for_key_until_next_offset(ifstream &stream, const K key)
{
    string input_line;
    int line_number = 0;
    while (getline(stream, input_line))
    {
        const auto starting_char = input_line[0];
        bool reached_end = starting_char == memtable_config.key_offset_indicator && line_number != 0;
        if (reached_end)
        {
            return nullopt;
        }
        const auto key_value_delimeter_position = input_line.find(memtable_config.key_value_delimeter);
        if (check_key_equality(key, input_line, key_value_delimeter_position))
        {
            const auto value_read = input_line.substr(key_value_delimeter_position + 1);
            const auto return_value = convert_to_correct_numerical_type(value_read);
            return optional<V>{return_value};
        }
        ++line_number;
    }
    return nullopt;
}

template <typename K, typename V>
requires IsStringLike<K>
bool SortedStringTable<K, V>::check_key_equality(const K key, const string &input_line, const size_t delimeter_position)
{
    return input_line[0] == memtable_config.key_offset_indicator ? input_line.substr(1, delimeter_position - 1) == key
                                                                 : input_line.substr(0, delimeter_position) == key;
}

template <typename K, typename V>
requires IsNumber<V>
    V SortedStringTable<K, V>::convert_to_correct_numerical_type(const string &value_as_string)
{
    return is_integral_v<V> ? stoi(value_as_string) : stod(value_as_string);
}