#include <iostream>
#include <string>
#include "memtable.hpp"

using namespace std;

int main()
{
    Memtable<string, int> memtable(10);
    memtable.insert("first", 1);
    memtable.insert("second", 2);
    memtable.insert("third", 3);

    auto all_elements = memtable.get_all_elements();
    for (const auto &[key, value] : all_elements)
    {
        cout << key << " " << value << endl;
    }
}
