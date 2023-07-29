#include <iostream>
#include "hash_table.h"

using namespace std;

int main() {
    hash_table<string, string> ht(13, 0.5);

    cout << ht.size() << " " << ht.capacity() << endl;
    ht.insert("key1",  "value1");
    ht.insert("key2",  "value2");
    ht.insert("key3",  "value3");
    ht.insert("key4",  "value4");

    cout << ht.size() << " " << ht.capacity() << endl;
    string value;

    cout << ht.find("key1", value) << " " << value << endl;
    cout << ht.find("key2", value) << " " << value << endl;
    cout << ht.find("key3", value) << " " << value << endl;
    cout << ht.find("key4", value) << " " << value << endl;

    cout << ht.size() << " " << ht.capacity() << endl;


    ht.remove("key1");
    cout << ht.size() << " " << ht.capacity() << endl;
    ht.remove("key2");
    cout << ht.size() << " " << ht.capacity() << endl;
    ht.remove("key3");
    cout << ht.size() << " " << ht.capacity() << endl;
    ht.remove("key4");
    cout << ht.size() << " " << ht.capacity() << endl;

    cout << ht.size() << " " << ht.capacity() << endl;

    value = "null";

    cout << ht.find("key1", value) << " " << value << endl;
    cout << ht.find("key2", value) << " " << value << endl;
    cout << ht.find("key3", value) << " " << value << endl;
    cout << ht.find("key4", value) << " " << value << endl;

    ht.insert("abc",  "value1");
    ht.insert("abc",  "value2");
    ht.insert("xyz",  "value3");

    value = "null";

    cout << ht.find("abc", value) << " " << value << endl;
    cout << ht.find("xyz", value) << " " << value << endl;

    return 0;
}
