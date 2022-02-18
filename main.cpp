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

    auto result = memtable.find("first");
    // cout << result.value() << endl;

    map<string, int> map;
    map.insert({ "first", 1});
    vector<pair<string, int>> result_vector;
    // for(auto iterator = map.begin(); iterator != map.end(); ++iterator){
    //     result_vector.push_back(make_pair(iterator->first, iterator->second));
    // }
    // cout << result_vector.at(0).first << endl;

    auto all_elements = memtable.get_all_elements();
    for(const auto& [key, value] : all_elements){
        cout << key << " " << value << endl;
    }
}
