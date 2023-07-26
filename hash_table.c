//
// Created by 1412 on 2023/7/26.
//

#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "prime.h"

static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    i->next = NULL;
    return i;
}

static ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}

void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item *item = ht->items[i];
        while (item != NULL) {
            ht_item *next = item->next;
            ht_del_item(item);
            item = next;
        }
    }
    free(ht->items);
    free(ht);
}

static int ht_hash(const char* s, const int m) {
    int hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash = 31 * hash + s[i];
        hash = hash % m;
    }
    return (int)hash;
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* head = ht->items[i];
        while (head != NULL) {
            ht_insert(new_ht, head->key, head->value);
            head = head->next;
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

static ht_item* ht_find(ht_hash_table* ht, const char* key) {
    int index = ht_hash(key, ht->size);
    ht_item* head = ht->items[index];
    while (head != NULL) {
        if (strcmp(head->key, key) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

char* ht_search(ht_hash_table* ht, const char* key) {
    ht_item* item = ht_find(ht, key);
    if (item == NULL) {
        return NULL;
    }
    return item->value;
}

void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }

    ht_item *find = ht_find(ht, key);
    if (find != NULL) {
        find->value = strdup(value);
        // no need to increment ht->count
        return;
    } else {
        ht_item* item = ht_new_item(key, value);
        int index = ht_hash(item->key, ht->size);
        ht_item* head = ht->items[index];
        item->next = head;
        ht->items[index] = item;
    }
    ht->count++;
}

void ht_delete(ht_hash_table* ht, const char* key) {
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        ht_resize_down(ht);
    }

    int index = ht_hash(key, ht->size);
    ht_item* head = ht->items[index];
    ht_item* cur = head;
    ht_item* prev = NULL;
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            if (head == cur) {
                ht->items[index] = cur->next;
            } else {
                prev->next = cur->next;
            }
            ht_del_item(cur);
            ht->count--;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

