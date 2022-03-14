#include "memtable_config.hpp"

namespace memtable_write_functions
{
    template <typename K, typename V>
    optional<vector<pair<K, int>>> write_data_to_file(MemtableConfig &memtable_config, vector<pair<K, V>> key_value_pairs)
    {
        const auto number_of_key_value_pairs = key_value_pairs.size();
        ofstream output_file(memtable_config.file_path);
        vector<pair<K, int>> key_offsets;

        auto index = 0;
        auto end_index = number_of_key_value_pairs - 1;
        auto accumulated_offset_from_start = 0;

        for (const auto &[key, value] : key_value_pairs)
        {
            string key_value_line_to_write;
            if (is_key_offset_index<K, V>(index, end_index, memtable_config.key_offset_frequency))
            {
                key_value_line_to_write = key_value_offset_string_to_write<K, V>(key, value,
                                                                                 memtable_config.key_offset_indicator,
                                                                                 memtable_config.key_value_delimeter);
                key_offsets.push_back(make_pair(key, accumulated_offset_from_start));
            }
            else
            {
                key_value_line_to_write = key_value_string_to_write<K, V>(key, value, memtable_config.key_value_delimeter);
            }
            auto size_of_line = key_value_line_to_write.size();
            accumulated_offset_from_start += size_of_line;
            ++index;
            output_file << key_value_line_to_write;
        }
        return key_offsets.size() > 0 ? optional{key_offsets} : nullopt;
    }

    template <typename K, typename V>
    string key_value_string_to_write(const K key, const V value, const char delimeter)
    {
        return fmt::format("{}{}{}\n", key, delimeter, value);
    }

    template <typename K, typename V>
    string key_value_offset_string_to_write(const K key, const V value, const char offset_indicator, const char delimeter)
    {
        return fmt::format("{}{}{}{}\n", offset_indicator, key, delimeter, value);
    }

    template <typename K, typename V>
    bool is_key_offset_index(const int index, const int end_index, const int offset_frequency)
    {
        return index != 0 && index != end_index && index % offset_frequency == 0;
    }
}