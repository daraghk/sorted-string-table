#include <iostream>
#include <string>
#include "sorted_string_table.hpp"

using namespace std;

int main()
{
    // Memtable<string, int> memtable(10, "./_test_");
    // memtable.insert("first", 1);
    // memtable.insert("second", 2);
    // memtable.insert("third", 3);

    // auto all_elements = memtable.get_all_elements();
    // for (const auto &[key, value] : all_elements)
    // {
    //     cout << key << " " << value << endl;
    // }

    SortedStringTable<string, int> ss_table(10, "./_test_");

    ss_table.insert("first", 1);
    ss_table.insert("second", 2);

    auto find_result = ss_table.find("third");

    cout << is_integral_v<string> << endl; 
}
