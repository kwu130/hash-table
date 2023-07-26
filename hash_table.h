//
// Created by 1412 on 2023/7/26.
//


#ifndef HASH_TABLE_HASH_TABLE_H
#define HASH_TABLE_HASH_TABLE_H

#define HT_INITIAL_BASE_SIZE 31
// refer to https://github.com/jamesroutley/write-a-hash-table/tree/master

typedef struct ht_item {
    char* key;
    char* value;
    // conflict resolution through separate chaining
    struct ht_item *next;
} ht_item;

typedef struct {
    int base_size;
    int size;
    int count;
    ht_item **items;
} ht_hash_table;

ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* ht);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* h, const char* key);

#endif //HASH_TABLE_HASH_TABLE_H


