//
// Created by 1412 on 2023/7/29.
//

#ifndef HASHTABLE_HASH_NODE_H
#define HASHTABLE_HASH_NODE_H


template<typename K, typename V>
class hash_node {
public:
    hash_node() = default;
    hash_node(const K &key, const V &value) : key(key), value(value) {}
    const K& get_key() const;
    const V& get_value() const;
    void set_value(const V &value);
    ~hash_node() = default;

private:
    K key;
    V value;
};


template<typename K, typename V>
const K& hash_node<K, V>::get_key() const {
    return key;
}


template<typename K, typename V>
const V& hash_node<K, V>::get_value() const {
    return value;
}


template<typename K, typename V>
void hash_node<K, V>::set_value(const V &value) {
    this->value = value;
}

#endif //HASHTABLE_HASH_NODE_H
