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
    SortedStringTable(unsigned int memtable_capacity, string memtable_main_filepath) : memtable(memtable_capacity, memtable_main_filepath),
                                                                                       current_size(0),
                                                                                       memtable_main_filepath(memtable_main_filepath){};
    int get_size() { return current_size; }
    void insert(const K key, const V value);
    optional<V> find(const K key);

private:
    int current_size;
    string memtable_main_filepath;
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
    auto memtable_find_result = memtable.find(key);
    if (memtable_find_result.has_value())
    {
        return memtable_find_result;
    }

    return linear_search<K, V>(memtable_main_filepath, key);
}

template <typename K, typename V>
requires IsStringLike<K> && IsNumberOrStringLike<V>
    optional<V> linear_search(const string &memtable_file_path, const K key)
{
    //need to determine starting point first
    //where to read from
    //compare key to key offsets stored -> search from correct beginning
    ifstream most_recent_memtable_write(memtable_file_path);
    auto starting_point = 0;
    most_recent_memtable_write.seekg(starting_point, ios::beg);
    const char new_offset_beginning = '&';
    const auto search_result = search_stream_for_key_until<K, V>(most_recent_memtable_write, key, new_offset_beginning);
    return search_result;
}

template <typename K, typename V>
optional<V> search_stream_for_key_until(ifstream &stream, const K key, const char end_of_read)
{
    string input_line;
    while (getline(stream, input_line))
    {
        const auto starting_char = input_line[0];
        if (starting_char == end_of_read)
        {
            return nullopt;
        }
        const auto key_value_delimeter_position = input_line.find(':');
        if (compare_keys<K>(key, input_line, key_value_delimeter_position))
        {
            const auto value_read = input_line.substr(key_value_delimeter_position + 1);
            const auto return_value = convert_to_correct_numerical_type<V>(value_read);
            return optional<V>{return_value};
        }
    }
    return nullopt;
}

template <typename K>
requires IsStringLike<K>
bool compare_keys(const K key, const string &input_line, const size_t delimeter_position)
{
    string key_read = input_line.substr(0, delimeter_position);
    return key_read == key;
}

template <typename V>
requires IsNumber<V>
    V convert_to_correct_numerical_type(const string &value_as_string)
{
    return is_integral_v<V> ? stoi(value_as_string) : stod(value_as_string);
}