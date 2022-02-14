#include <iostream>
#include <string>
#include "memtable.hpp"

using namespace std;

int main()
{
    Memtable<string, int> memtable(2);
    memtable.insert("first", 1);
    memtable.insert("second", 2);
    memtable.insert("third", 3);

    auto result = memtable.find("first");
    cout << result.value() << endl;

    auto bad_result = memtable.find("third");
    cout << bad_result.value() << endl;
}
