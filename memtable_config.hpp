#include <string>
using namespace std;

struct MemtableConfig
{
    char key_value_delimeter = ':';
    char key_offset_indicator = '&';
    unsigned int key_offset_frequency = 5;
    unsigned int capacity;
    string file_path;
};
