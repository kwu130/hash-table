//
// Created by 1412 on 2023/7/28.
//

#ifndef HASHTABLE_HASH_TABLE_H
#define HASHTABLE_HASH_TABLE_H

#include <iostream>
#include <list>
#include <string>
#include "prime.h"
#include "hash_node.h"

#define HASH_TABLE_DEFAULT_SIZE 31
#define HASH_TABLE_MAX_LOAD_FACTOR 0.75
#define HASH_TABLE_MIN_LOAD_FACTOR 0.10

template<typename K, typename V>
class hash_table {
public:
    hash_table();
    hash_table(const int capacity);
    hash_table(const int capacity, const double factor);
    void insert(const K &key, const V &value);
    bool find(const K &key, V &value);
    void remove(const K &key);
    int size() const;
    int capacity() const;
    ~hash_table();

private:
    int get_index(const K &key) const;
    void resize_ht(const int new_capacity);
    void ht_resize_up();
    void ht_resize_down();

private:
    std::list<hash_node<K, V>> *table;
    int ht_size{};
    int ht_capacity{};
    double max_factor{};
    double min_factor{};
};


template<typename K, typename V>
hash_table<K, V>::hash_table() {
    this->ht_size = 0;
    this->ht_capacity = HASH_TABLE_DEFAULT_SIZE;
    this->max_factor = HASH_TABLE_MAX_LOAD_FACTOR;
    this->min_factor = HASH_TABLE_MIN_LOAD_FACTOR;
    this->table = new std::list<hash_node<K, V>>[this->ht_capacity];
}


template<typename K, typename V>
hash_table<K, V>::hash_table(const int capacity) {
    this->ht_size = 0;
    this->ht_capacity = capacity;
    this->max_factor = HASH_TABLE_MAX_LOAD_FACTOR;
    this->min_factor = HASH_TABLE_MIN_LOAD_FACTOR;
    this->table = new std::list<hash_node<K, V>>[this->ht_capacity];
}


template<typename K, typename V>
hash_table<K, V>::hash_table(const int capacity, const double factor) {
    this->ht_size = 0;
    this->ht_capacity = capacity;
    this->max_factor = factor;
    this->min_factor = HASH_TABLE_MIN_LOAD_FACTOR;
    this->table = new std::list<hash_node<K, V>>[this->ht_capacity];
}


template<typename K, typename V>
int hash_table<K, V>::get_index(const K &key) const{
    std::hash<K> hash;
    return hash(key) % this->ht_capacity;
}

template<typename K, typename V>
void hash_table<K, V>::insert(const K &key, const V &value) {
    if (this->ht_size > this->ht_capacity * this->max_factor) {
        this->ht_resize_up();
    }
    int index = this->get_index(key);
    std::list<hash_node<K, V>> &bucket = this->table[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->get_key() == key) {
            it->set_value(value);
            return;
        }
    }
    bucket.push_back(hash_node<K, V>(key, value));
    ++this->ht_size;
}


template<typename K, typename V>
bool hash_table<K, V>::find(const K &key, V &value) {
    int index = this->get_index(key);
    std::list<hash_node<K, V>> &bucket = this->table[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->get_key() == key) {
            value = it->get_value();
            return true;
        }
    }
    return false;
}


template<typename K, typename V>
void hash_table<K, V>::remove(const K &key) {
    if (this->ht_size < this->ht_capacity * this->min_factor) {
        this->ht_resize_down();
    }
    int index = this->get_index(key);
    std::list<hash_node<K, V>> &bucket = this->table[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->get_key() == key) {
            bucket.erase(it);
            --this->ht_size;
            return;
        }
    }
}


template<typename K, typename V>
void hash_table<K, V>::resize_ht(const int new_capacity) {
    int next_prime_cap = next_prime(new_capacity);
    int old_capacity = this->ht_capacity;
    this->ht_size = 0;
    this->ht_capacity = next_prime_cap;
    auto *new_table = new std::list<hash_node<K, V>>[this->ht_capacity];
    for (int i = 0; i < old_capacity; ++i) {
        for (auto it = this->table[i].begin(); it != this->table[i].end(); ++it) {
            int index = this->get_index(it->get_key());
            new_table[index].push_back(*it);
            ++this->ht_size;
        }
    }
    delete [] this->table;
    this->table = new_table;
}


template<typename K, typename V>
void hash_table<K, V>::ht_resize_up() {
    this->resize_ht(this->ht_capacity * 2);
}


template<typename K, typename V>
void hash_table<K, V>::ht_resize_down() {
    this->resize_ht(this->ht_capacity / 2);
}


template<typename K, typename V>
int hash_table<K, V>::size() const{
    return this->ht_size;
}


template<typename K, typename V>
int hash_table<K, V>::capacity() const {
    return this->ht_capacity;
}


template<typename K, typename V>
hash_table<K, V>::~hash_table() {
    delete[] this->table;
}


#endif //HASHTABLE_HASH_TABLE_H
