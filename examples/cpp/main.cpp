#include <iostream>
#include <string>

#include <hash_table/cpp/hash_table.hpp>

int main() {
    hash_table<std::string, std::string> table;
    table.insert("language", "C++");
    table.insert("project", "hash-table");
    table.insert_or_assign("language", "C++11");
    table.reserve(100U);

    std::string value;
    if (table.find("language", value)) {
        std::cout << "language=" << value << '\n';
    }

    table.remove("project");
    std::cout << "size=" << table.size() << " capacity=" << table.capacity() << '\n';
    return 0;
}
