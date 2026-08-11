#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hash_table/c/hash_table.h>

enum { ITEM_COUNT = 200, BUFFER_SIZE = 32 };

static void check(const int condition, const char *expression, const int line) {
    if (!condition) {
        fprintf(stderr, "check failed at line %d: %s\n", line, expression);
        exit(EXIT_FAILURE);
    }
}

#define CHECK(expression) check((expression), #expression, __LINE__)

/* Deliberately collide with common helper names to verify library symbol isolation. */
int is_prime(const int value) {
    (void) value;
    return 0;
}

int next_prime(const int value) {
    (void) value;
    return 2;
}

typedef struct {
    int count;
    int limit;
} visit_context;

static int count_items(const char *key, const char *value, void *raw_context) {
    visit_context *context = raw_context;
    CHECK(key != NULL);
    CHECK(value != NULL);
    ++context->count;
    return context->limit == 0 || context->count < context->limit;
}

int main(void) {
    ht_hash_table *table = ht_new();
    CHECK(table != NULL);
    CHECK(table->count == 0);
    CHECK(table->size == HT_INITIAL_BASE_SIZE);
    CHECK(ht_count(table) == 0);
    CHECK(ht_capacity(table) == HT_INITIAL_BASE_SIZE);
    CHECK(ht_load_factor(table) == 0.0);
    CHECK(ht_search(table, "missing") == NULL);
    CHECK(!ht_contains(table, "missing"));

    for (int index = 0; index < ITEM_COUNT; ++index) {
        char key[BUFFER_SIZE];
        char value[BUFFER_SIZE];
        (void) snprintf(key, sizeof(key), "key-%d", index);
        (void) snprintf(value, sizeof(value), "value-%d", index);
        CHECK(ht_insert_or_assign(table, key, value));
    }

    CHECK(table->count == ITEM_COUNT);
    CHECK(table->size > HT_INITIAL_BASE_SIZE);
    CHECK(ht_count(table) == ITEM_COUNT);
    CHECK(ht_load_factor(table) > 0.0);
    for (int index = 0; index < ITEM_COUNT; ++index) {
        char key[BUFFER_SIZE];
        char expected[BUFFER_SIZE];
        (void) snprintf(key, sizeof(key), "key-%d", index);
        (void) snprintf(expected, sizeof(expected), "value-%d", index);
        const char *actual = ht_search(table, key);
        CHECK(actual != NULL);
        CHECK(strcmp(actual, expected) == 0);
    }

    const int count_before_update = table->count;
    ht_insert(table, "key-42", "updated");
    CHECK(table->count == count_before_update);
    CHECK(strcmp(ht_search(table, "key-42"), "updated") == 0);
    CHECK(ht_contains(table, "key-42"));
    CHECK(ht_reserve(table, 1000));
    CHECK(ht_capacity(table) >= 1334);
    CHECK(strcmp(ht_search(table, "key-42"), "updated") == 0);

    visit_context all_items = {0, 0};
    CHECK(ht_foreach(table, count_items, &all_items) == ITEM_COUNT);
    CHECK(all_items.count == ITEM_COUNT);

    visit_context first_three = {0, 3};
    CHECK(ht_foreach(table, count_items, &first_three) == 3);
    CHECK(first_three.count == 3);

    for (int index = 0; index < ITEM_COUNT; ++index) {
        char key[BUFFER_SIZE];
        (void) snprintf(key, sizeof(key), "key-%d", index);
        CHECK(ht_remove(table, key));
    }

    CHECK(table->count == 0);
    CHECK(table->size == HT_INITIAL_BASE_SIZE);
    CHECK(!ht_remove(table, "missing"));
    ht_delete(table, "missing");
    ht_insert(NULL, "key", "value");
    ht_delete(NULL, "key");
    CHECK(ht_search(NULL, "key") == NULL);

    ht_del_hash_table(table);
    ht_del_hash_table(NULL);

    ht_hash_table *reserved = ht_new_with_capacity(5);
    CHECK(reserved != NULL);
    CHECK(ht_capacity(reserved) == 5);
    CHECK(ht_reserve(reserved, 1000));
    CHECK(ht_capacity(reserved) >= 1334);
    const int reserved_capacity = ht_capacity(reserved);
    CHECK(ht_insert_or_assign(reserved, "key", "value"));
    CHECK(ht_contains(reserved, "key"));
    ht_clear(reserved);
    CHECK(ht_count(reserved) == 0);
    CHECK(ht_capacity(reserved) == reserved_capacity);
    CHECK(!ht_contains(reserved, "key"));
    CHECK(ht_insert_or_assign(reserved, "key", "value"));
    CHECK(ht_remove(reserved, "key"));
    CHECK(ht_capacity(reserved) == 5);
    ht_del_hash_table(reserved);

    CHECK(ht_new_with_capacity(0) == NULL);
    CHECK(!ht_reserve(NULL, 10));
    return 0;
}
