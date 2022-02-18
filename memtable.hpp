#include <map>
#include <optional>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>

using namespace std;

template <typename K, typename V>
class Memtable
{
public:
    Memtable(unsigned int input_capacity) : capacity(input_capacity), table(), current_size(0) {}
    int get_capacity() { return capacity; }
    int get_size() { return current_size; }
    void insert(const K key, const V value);
    optional<V> find(const K key);
    vector<pair<K, V>> get_all_elements();

private:
    const unsigned int capacity;
    int current_size;
    map<K, V> table;
    void write_to_file();
};

template <typename K, typename V>
void Memtable<K, V>::insert(const K key, const V value)
{
    if (current_size < capacity)
    {
        table.insert({key, value});
        ++current_size;
        if(current_size == capacity){
            write_to_file();
            table.clear();
            current_size = 0;
        }
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
void Memtable<K,V>::write_to_file(){
    auto all_elements = get_all_elements();
    ofstream output_file("./example.txt");
    for(const auto& [key, value] : all_elements){
        output_file << key << ":" << value << endl;
    }
}