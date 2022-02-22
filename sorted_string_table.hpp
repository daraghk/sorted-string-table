#include "memtable.hpp"
#include <string>

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
    // check memtable
    auto memtable_find_result = memtable.find(key);
    if (memtable_find_result.has_value())
    {
        return memtable_find_result;
    }
    else
    {
        // search over most recent memtable write
        // do a basic linear search first over whole file
        ifstream most_recent_memtable_write(memtable_main_filepath);
        string input_line;
        while (getline(most_recent_memtable_write, input_line))
        {
            size_t key_value_delimeter_position = input_line.find(':');
            string key_read = input_line.substr(0, key_value_delimeter_position);
            string value_read = input_line.substr(key_value_delimeter_position + 1);
            if (key == key_read)
            {
                V return_value = convert_to_correct_numerical_type<V>(value_read);
                return optional<V>{return_value};
            }
        }
    }
    return nullopt;
}

template <typename T>
requires IsNumber<T>
    T convert_to_correct_numerical_type(const string &input)
{
    return is_integral_v<T> ? stoi(input) : stod(input);
}