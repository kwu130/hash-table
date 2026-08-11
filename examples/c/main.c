#include <stdio.h>
#include <stdlib.h>

#include <hash_table/c/hash_table.h>

int main(void) {
    ht_hash_table *table = ht_new();
    if (table == NULL) {
        fputs("failed to create hash table\n", stderr);
        return EXIT_FAILURE;
    }

    ht_insert(table, "language", "C");
    ht_insert(table, "project", "hash-table");
    ht_insert(table, "language", "C11");
    (void) ht_reserve(table, 100);

    printf("size=%d capacity=%d load=%.3f\n",
           ht_count(table), ht_capacity(table), ht_load_factor(table));
    printf("language=%s\n", ht_search(table, "language"));

    ht_delete(table, "project");
    printf("contains project: %s\n", ht_search(table, "project") == NULL ? "no" : "yes");

    ht_del_hash_table(table);
    return EXIT_SUCCESS;
}
