#include <hash_table/c/hash_table.h>

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static int ht_is_prime(const int value) {
    if (value < 2) {
        return 0;
    }
    if (value == 2) {
        return 1;
    }
    if (value % 2 == 0) {
        return 0;
    }

    for (int divisor = 3; divisor <= value / divisor; divisor += 2) {
        if (value % divisor == 0) {
            return 0;
        }
    }
    return 1;
}

static int ht_next_prime(int value) {
    if (value < 2) {
        return 2;
    }
    while (!ht_is_prime(value)) {
        ++value;
    }
    return value;
}

static char *ht_copy_string(const char *source) {
    const size_t length = strlen(source) + 1U;
    char *copy = malloc(length);
    if (copy != NULL) {
        memcpy(copy, source, length);
    }
    return copy;
}

static void ht_del_item(ht_item *item) {
    if (item == NULL) {
        return;
    }
    free(item->key);
    free(item->value);
    free(item);
}

static ht_item *ht_new_item(const char *key, const char *value) {
    ht_item *item = malloc(sizeof(*item));
    if (item == NULL) {
        return NULL;
    }

    item->key = ht_copy_string(key);
    item->value = ht_copy_string(value);
    item->next = NULL;
    if (item->key == NULL || item->value == NULL) {
        ht_del_item(item);
        return NULL;
    }
    return item;
}

static ht_hash_table *ht_new_sized(const int base_size) {
    ht_hash_table *ht = malloc(sizeof(*ht));
    if (ht == NULL) {
        return NULL;
    }

    ht->base_size = base_size;
    ht->minimum_base_size = base_size;
    ht->size = ht_next_prime(base_size);
    ht->count = 0;
    ht->items = calloc((size_t) ht->size, sizeof(*ht->items));
    if (ht->items == NULL) {
        free(ht);
        return NULL;
    }
    return ht;
}

ht_hash_table *ht_new(void) {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

ht_hash_table *ht_new_with_capacity(const int capacity) {
    if (capacity <= 0) {
        return NULL;
    }
    return ht_new_sized(capacity);
}

void ht_clear(ht_hash_table *ht) {
    if (ht == NULL) {
        return;
    }

    for (int index = 0; index < ht->size; ++index) {
        ht_item *item = ht->items[index];
        while (item != NULL) {
            ht_item *next = item->next;
            ht_del_item(item);
            item = next;
        }
        ht->items[index] = NULL;
    }
    ht->count = 0;
}

void ht_del_hash_table(ht_hash_table *ht) {
    if (ht == NULL) {
        return;
    }

    ht_clear(ht);
    free(ht->items);
    free(ht);
}

static int ht_hash(const char *key, const int bucket_count) {
    size_t hash = 0U;
    const unsigned char *current = (const unsigned char *) key;
    while (*current != '\0') {
        hash = (31U * hash + *current) % (size_t) bucket_count;
        ++current;
    }
    return (int) hash;
}

static ht_item *ht_find(const ht_hash_table *ht, const char *key) {
    const int index = ht_hash(key, ht->size);
    ht_item *item = ht->items[index];
    while (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

const char *ht_search(const ht_hash_table *ht, const char *key) {
    if (ht == NULL || key == NULL) {
        return NULL;
    }

    const ht_item *item = ht_find(ht, key);
    return item == NULL ? NULL : item->value;
}

static int ht_resize(ht_hash_table *ht, const int requested_base_size) {
    const int new_base_size = requested_base_size < ht->minimum_base_size
        ? ht->minimum_base_size
        : requested_base_size;
    const int new_size = ht_next_prime(new_base_size);
    if (new_size == ht->size) {
        ht->base_size = new_base_size;
        return 1;
    }
    ht_item **new_items = calloc((size_t) new_size, sizeof(*new_items));
    if (new_items == NULL) {
        return 0;
    }

    for (int index = 0; index < ht->size; ++index) {
        ht_item *item = ht->items[index];
        while (item != NULL) {
            ht_item *next = item->next;
            const int new_index = ht_hash(item->key, new_size);
            item->next = new_items[new_index];
            new_items[new_index] = item;
            item = next;
        }
    }

    free(ht->items);
    ht->items = new_items;
    ht->base_size = new_base_size;
    ht->size = new_size;
    return 1;
}

static int ht_should_grow(const ht_hash_table *ht) {
    return (double) (ht->count + 1) / (double) ht->size > HT_MAX_LOAD_FACTOR;
}

static int ht_should_shrink(const ht_hash_table *ht) {
    return ht->base_size > ht->minimum_base_size
        && (double) ht->count / (double) ht->size < HT_MIN_LOAD_FACTOR;
}

int ht_insert_or_assign(ht_hash_table *ht, const char *key, const char *value) {
    if (ht == NULL || key == NULL || value == NULL) {
        return 0;
    }

    ht_item *existing = ht_find(ht, key);
    if (existing != NULL) {
        char *new_value = ht_copy_string(value);
        if (new_value != NULL) {
            free(existing->value);
            existing->value = new_value;
            return 1;
        }
        return 0;
    }

    if (ht_should_grow(ht)) {
        (void) ht_resize(ht, ht->base_size * 2);
    }

    ht_item *item = ht_new_item(key, value);
    if (item == NULL) {
        return 0;
    }
    const int index = ht_hash(key, ht->size);
    item->next = ht->items[index];
    ht->items[index] = item;
    ++ht->count;
    return 1;
}

void ht_insert(ht_hash_table *ht, const char *key, const char *value) {
    (void) ht_insert_or_assign(ht, key, value);
}

int ht_contains(const ht_hash_table *ht, const char *key) {
    return ht_search(ht, key) != NULL;
}

int ht_remove(ht_hash_table *ht, const char *key) {
    if (ht == NULL || key == NULL) {
        return 0;
    }

    const int index = ht_hash(key, ht->size);
    ht_item **link = &ht->items[index];
    while (*link != NULL) {
        ht_item *item = *link;
        if (strcmp(item->key, key) == 0) {
            *link = item->next;
            ht_del_item(item);
            --ht->count;
            while (ht_should_shrink(ht)) {
                if (!ht_resize(ht, ht->base_size / 2)) {
                    break;
                }
            }
            return 1;
        }
        link = &item->next;
    }
    return 0;
}

void ht_delete(ht_hash_table *ht, const char *key) {
    (void) ht_remove(ht, key);
}

int ht_reserve(ht_hash_table *ht, const int expected_count) {
    if (ht == NULL || expected_count < 0) {
        return 0;
    }

    const size_t count = (size_t) expected_count;
    const size_t required_buckets = count + (count + 2U) / 3U;
    if (required_buckets <= (size_t) ht->size) {
        return 1;
    }
    if (required_buckets > (size_t) INT_MAX) {
        return 0;
    }
    return ht_resize(ht, (int) required_buckets);
}

int ht_count(const ht_hash_table *ht) {
    return ht == NULL ? 0 : ht->count;
}

int ht_capacity(const ht_hash_table *ht) {
    return ht == NULL ? 0 : ht->size;
}

double ht_load_factor(const ht_hash_table *ht) {
    if (ht == NULL || ht->size == 0) {
        return 0.0;
    }
    return (double) ht->count / (double) ht->size;
}

int ht_foreach(const ht_hash_table *ht, ht_visit_fn visitor, void *context) {
    if (ht == NULL || visitor == NULL) {
        return 0;
    }

    int visited = 0;
    for (int index = 0; index < ht->size; ++index) {
        const ht_item *item = ht->items[index];
        while (item != NULL) {
            ++visited;
            if (!visitor(item->key, item->value, context)) {
                return visited;
            }
            item = item->next;
        }
    }
    return visited;
}
