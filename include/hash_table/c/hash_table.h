#ifndef HASH_TABLE_C_HASH_TABLE_H
#define HASH_TABLE_C_HASH_TABLE_H

#define HT_INITIAL_BASE_SIZE 31
#define HT_MAX_LOAD_FACTOR 0.75
#define HT_MIN_LOAD_FACTOR 0.10

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ht_item {
    char *key;
    char *value;
    struct ht_item *next;
} ht_item;

typedef struct {
    int base_size;
    int minimum_base_size;
    int size;
    int count;
    ht_item **items;
} ht_hash_table;

typedef int (*ht_visit_fn)(const char *key, const char *value, void *context);

ht_hash_table *ht_new(void);
ht_hash_table *ht_new_with_capacity(int capacity);
void ht_del_hash_table(ht_hash_table *ht);

int ht_insert_or_assign(ht_hash_table *ht, const char *key, const char *value);
void ht_insert(ht_hash_table *ht, const char *key, const char *value);
const char *ht_search(const ht_hash_table *ht, const char *key);
int ht_contains(const ht_hash_table *ht, const char *key);
int ht_remove(ht_hash_table *ht, const char *key);
void ht_delete(ht_hash_table *ht, const char *key);

int ht_reserve(ht_hash_table *ht, int expected_count);
void ht_clear(ht_hash_table *ht);
int ht_count(const ht_hash_table *ht);
int ht_capacity(const ht_hash_table *ht);
double ht_load_factor(const ht_hash_table *ht);
int ht_foreach(const ht_hash_table *ht, ht_visit_fn visitor, void *context);

#ifdef __cplusplus
}
#endif

#endif
