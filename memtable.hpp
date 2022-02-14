#include <map>
#include <optional>

using namespace std;

template <typename K, typename V>
class Memtable
{
public:
    Memtable(unsigned int input_capacity) : capacity(input_capacity), table(), current_size(0) {}
    int get_capacity() { return capacity; }
    int get_size() { return current_size; }
    void insert(K key, V value);
    optional<V> find(K key);

private:
    unsigned int capacity;
    int current_size;
    map<K, V> table;
    void write_to_file();
};

template <typename K, typename V>
void Memtable<K, V>::insert(K key, V value)
{
    if (current_size < capacity)
    {
        table.insert({key, value});
        ++current_size;
    }
}

template <typename K, typename V>
optional<V> Memtable<K, V>::find(K key)
{
    auto result = table.find(key);
    if (result != table.end())
    {
        return optional<V>{result->second};
    }
    return nullopt;
}