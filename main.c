#include <stdio.h>
#include "hash_table.h"

int main() {
    ht_hash_table *table = ht_new();
    printf("%d %d\n", table->size, table->count);

    ht_insert(table, "key1", "value1");
    ht_insert(table, "key2", "value2");
    ht_insert(table, "key3", "value3");
    ht_insert(table, "key4", "value4");
    ht_insert(table, "key5", "value5");
    ht_insert(table, "key6", "value6");

    printf("%d %d\n", table->size, table->count);

    printf("%s\n", ht_search(table, "key1"));
    printf("%s\n", ht_search(table, "key2"));
    printf("%s\n", ht_search(table, "key3"));
    printf("%s\n", ht_search(table, "key4"));
    printf("%s\n", ht_search(table, "key5"));
    printf("%s\n", ht_search(table, "key6"));

    ht_delete(table, "key1");
    ht_delete(table, "key2");
    ht_delete(table, "key3");
    ht_delete(table, "key4");
    ht_delete(table, "key5");
    ht_delete(table, "key6");

    printf("%d %d\n", table->size, table->count);

    printf("%s\n", ht_search(table, "key1"));
    printf("%s\n", ht_search(table, "key2"));
    printf("%s\n", ht_search(table, "key3"));
    printf("%s\n", ht_search(table, "key4"));
    printf("%s\n", ht_search(table, "key5"));
    printf("%s\n", ht_search(table, "key6"));

    ht_insert(table, "abc", "value1");
    ht_insert(table, "abc", "value2");
    ht_insert(table, "xyz", "value3");

    printf("%d %d\n", table->size, table->count);

    printf("%s\n", ht_search(table, "abc"));
    printf("%s\n", ht_search(table, "xyz"));

    ht_delete(table, "abc");
    ht_delete(table, "xyz");

    printf("%d %d\n", table->size, table->count);

    printf("%s\n", ht_search(table, "abc"));
    printf("%s\n", ht_search(table, "xyz"));

    ht_del_hash_table(table);

    return 0;
}
