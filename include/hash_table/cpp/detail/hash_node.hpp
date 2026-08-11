#ifndef HASH_TABLE_CPP_DETAIL_HASH_NODE_HPP
#define HASH_TABLE_CPP_DETAIL_HASH_NODE_HPP

namespace hash_table_detail {

template<typename K, typename V>
class hash_node {
public:
    hash_node(const K &key, const V &value) : key_(key), value_(value) {}

    const K &get_key() const { return key_; }
    V &get_value() { return value_; }
    const V &get_value() const { return value_; }
    void set_value(const V &new_value) { value_ = new_value; }

private:
    K key_;
    V value_;
};

} // namespace hash_table_detail

#endif
