namespace memtable_search_functions
{
    template <typename K, typename V>
    int determine_search_start_point(const K key, const vector<pair<K, int>> &memtable_offsets)
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
    optional<V> linear_search_over_memtable_file_segment(const MemtableConfig &memtable_config, const K key, const int starting_point)
    {
        ifstream most_recent_memtable_write(memtable_config.file_path);
        most_recent_memtable_write.seekg(starting_point);
        const auto search_result = search_stream_for_key_until_next_offset<K, V>(memtable_config, most_recent_memtable_write, key);
        return search_result;
    }

    template <typename K, typename V>
    optional<V> search_stream_for_key_until_next_offset(const MemtableConfig &memtable_config, ifstream &stream, const K key)
    {
        string input_line;
        int line_number = 0;
        while (getline(stream, input_line))
        {
            const auto starting_char = input_line[0];
            bool reached_end_of_segment = starting_char == memtable_config.key_offset_indicator && line_number != 0;
            if (reached_end_of_segment)
            {
                return nullopt;
            }
            const auto key_value_delimeter_position = input_line.find(memtable_config.key_value_delimeter);
            if (check_key_equality<K, V>(key, input_line, key_value_delimeter_position, memtable_config.key_offset_indicator))
            {
                const auto value_read = input_line.substr(key_value_delimeter_position + 1);
                const auto return_value = convert_value_to_correct_numerical_type<K, V>(value_read);
                return optional<V>{return_value};
            }
            ++line_number;
        }
        return nullopt;
    }

    template <typename K, typename V>
    bool check_key_equality(const K key, const string &input_line, const size_t delimeter_position, const char key_offset_indicator)
    {
        return input_line[0] == key_offset_indicator ? input_line.substr(1, delimeter_position - 1) == key
                                                     : input_line.substr(0, delimeter_position) == key;
    }

    template <typename K, typename V>
    V convert_value_to_correct_numerical_type(const string &value_as_string)
    {
        return is_integral_v<V> ? stoi(value_as_string) : stod(value_as_string);
    }
}