#include <cstring>

#include <hash_table/c/hash_table.h>

int main() {
    ht_hash_table *table = ht_new();
    if (table == nullptr) {
        return 1;
    }

    const bool inserted = ht_insert_or_assign(table, "language", "C11") != 0;
    const char *value = ht_search(table, "language");
    const bool found = value != nullptr && std::strcmp(value, "C11") == 0;

    ht_del_hash_table(table);
    return inserted && found ? 0 : 1;
}
