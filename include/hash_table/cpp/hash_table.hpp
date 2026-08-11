#ifndef HASH_TABLE_CPP_HASH_TABLE_H
#define HASH_TABLE_CPP_HASH_TABLE_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/hash_node.hpp"
#include "detail/prime.hpp"

static const int HASH_TABLE_DEFAULT_SIZE = 31;
static const double HASH_TABLE_MAX_LOAD_FACTOR = 0.75;
static const double HASH_TABLE_MIN_LOAD_FACTOR = 0.10;

template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
class hash_table {
public:
    hash_table();
    explicit hash_table(int capacity);
    hash_table(int capacity, double max_load_factor);
    hash_table(const hash_table &) = default;
    hash_table &operator=(const hash_table &) = default;
    hash_table(hash_table &&other);
    hash_table &operator=(hash_table &&other);

    bool insert_or_assign(const K &key, const V &value);
    void insert(const K &key, const V &value);
    V *find(const K &key);
    const V *find(const K &key) const;
    bool find(const K &key, V &value) const;
    bool contains(const K &key) const;
    bool remove(const K &key);

    void reserve(std::size_t expected_size);
    void clear();
    void swap(hash_table &other);
    template<typename Function>
    void for_each(Function function) const;

    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;
    double load_factor() const;

private:
    typedef std::list<hash_table_detail::hash_node<K, V>> bucket_type;

    std::size_t get_index(const K &key, std::size_t bucket_count) const;
    void resize(std::size_t requested_capacity);
    void grow_if_needed(std::size_t projected_size);
    void shrink_if_needed() noexcept;

    std::vector<bucket_type> buckets_;
    std::size_t size_;
    std::size_t minimum_capacity_;
    double max_load_factor_;
    Hash hasher_;
    KeyEqual keys_equal_;
};

template<typename K, typename V, typename Hash, typename KeyEqual>
hash_table<K, V, Hash, KeyEqual>::hash_table()
    : hash_table(HASH_TABLE_DEFAULT_SIZE, HASH_TABLE_MAX_LOAD_FACTOR) {}

template<typename K, typename V, typename Hash, typename KeyEqual>
hash_table<K, V, Hash, KeyEqual>::hash_table(const int capacity)
    : hash_table(capacity, HASH_TABLE_MAX_LOAD_FACTOR) {}

template<typename K, typename V, typename Hash, typename KeyEqual>
hash_table<K, V, Hash, KeyEqual>::hash_table(const int capacity, const double max_load_factor)
    : buckets_(),
      size_(0U),
      minimum_capacity_(0U),
      max_load_factor_(max_load_factor),
      hasher_(),
      keys_equal_() {
    if (capacity <= 0) {
        throw std::invalid_argument("hash table capacity must be positive");
    }
    if (!(max_load_factor > 0.0 && max_load_factor <= 1.0)) {
        throw std::invalid_argument("max load factor must be in the range (0, 1]");
    }

    minimum_capacity_ = hash_table_detail::next_prime(static_cast<std::size_t>(capacity));
    buckets_.resize(minimum_capacity_);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
hash_table<K, V, Hash, KeyEqual>::hash_table(hash_table &&other)
    : hash_table() {
    swap(other);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
hash_table<K, V, Hash, KeyEqual> &hash_table<K, V, Hash, KeyEqual>::operator=(hash_table &&other) {
    if (this != &other) {
        hash_table replacement(std::move(other));
        swap(replacement);
    }
    return *this;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
std::size_t hash_table<K, V, Hash, KeyEqual>::get_index(
    const K &key,
    const std::size_t bucket_count) const {
    return hasher_(key) % bucket_count;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
bool hash_table<K, V, Hash, KeyEqual>::insert_or_assign(const K &key, const V &value) {
    bucket_type &current_bucket = buckets_[get_index(key, buckets_.size())];
    for (typename bucket_type::iterator it = current_bucket.begin(); it != current_bucket.end(); ++it) {
        if (keys_equal_(it->get_key(), key)) {
            it->set_value(value);
            return false;
        }
    }

    grow_if_needed(size_ + 1U);
    bucket_type &destination = buckets_[get_index(key, buckets_.size())];
    destination.emplace_back(key, value);
    ++size_;
    return true;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void hash_table<K, V, Hash, KeyEqual>::insert(const K &key, const V &value) {
    (void) insert_or_assign(key, value);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
V *hash_table<K, V, Hash, KeyEqual>::find(const K &key) {
    bucket_type &bucket = buckets_[get_index(key, buckets_.size())];
    for (typename bucket_type::iterator it = bucket.begin(); it != bucket.end(); ++it) {
        if (keys_equal_(it->get_key(), key)) {
            return &it->get_value();
        }
    }
    return nullptr;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
const V *hash_table<K, V, Hash, KeyEqual>::find(const K &key) const {
    const bucket_type &bucket = buckets_[get_index(key, buckets_.size())];
    for (typename bucket_type::const_iterator it = bucket.begin(); it != bucket.end(); ++it) {
        if (keys_equal_(it->get_key(), key)) {
            return &it->get_value();
        }
    }
    return nullptr;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
bool hash_table<K, V, Hash, KeyEqual>::find(const K &key, V &value) const {
    const V *found = find(key);
    if (found != nullptr) {
        value = *found;
        return true;
    }
    return false;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
bool hash_table<K, V, Hash, KeyEqual>::contains(const K &key) const {
    return find(key) != nullptr;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
bool hash_table<K, V, Hash, KeyEqual>::remove(const K &key) {
    bucket_type &bucket = buckets_[get_index(key, buckets_.size())];
    for (typename bucket_type::iterator it = bucket.begin(); it != bucket.end(); ++it) {
        if (keys_equal_(it->get_key(), key)) {
            bucket.erase(it);
            --size_;
            shrink_if_needed();
            return true;
        }
    }
    return false;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void hash_table<K, V, Hash, KeyEqual>::resize(const std::size_t requested_capacity) {
    const std::size_t new_capacity = hash_table_detail::next_prime(
        std::max(requested_capacity, minimum_capacity_));
    if (new_capacity == buckets_.size()) {
        return;
    }

    std::vector<bucket_type> new_buckets(new_capacity);
    std::vector<std::size_t> destination_indexes;
    destination_indexes.reserve(size_);
    for (typename std::vector<bucket_type>::const_iterator bucket = buckets_.begin();
         bucket != buckets_.end();
         ++bucket) {
        for (typename bucket_type::const_iterator node = bucket->begin();
             node != bucket->end();
             ++node) {
            destination_indexes.push_back(get_index(node->get_key(), new_capacity));
        }
    }

    std::size_t position = 0U;
    for (typename std::vector<bucket_type>::iterator bucket = buckets_.begin();
         bucket != buckets_.end();
         ++bucket) {
        while (!bucket->empty()) {
            typename bucket_type::iterator node = bucket->begin();
            const std::size_t index = destination_indexes[position];
            ++position;
            new_buckets[index].splice(new_buckets[index].end(), *bucket, node);
        }
    }
    buckets_.swap(new_buckets);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void hash_table<K, V, Hash, KeyEqual>::grow_if_needed(const std::size_t projected_size) {
    const double projected_load = static_cast<double>(projected_size)
        / static_cast<double>(buckets_.size());
    if (projected_load > max_load_factor_) {
        reserve(projected_size);
    }
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void hash_table<K, V, Hash, KeyEqual>::shrink_if_needed() noexcept {
    try {
        while (buckets_.size() > minimum_capacity_
               && load_factor() < HASH_TABLE_MIN_LOAD_FACTOR) {
            resize(buckets_.size() / 2U);
        }
    } catch (...) {
        // Shrinking is an optional optimization; removal has already succeeded.
    }
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void hash_table<K, V, Hash, KeyEqual>::reserve(const std::size_t expected_size) {
    if (expected_size <= size_) {
        return;
    }

    const double required = static_cast<double>(expected_size) / max_load_factor_;
    if (required > static_cast<double>(buckets_.max_size())) {
        throw std::length_error("requested hash table capacity is too large");
    }
    const std::size_t required_capacity = static_cast<std::size_t>(required)
        + (required > static_cast<double>(static_cast<std::size_t>(required)) ? 1U : 0U);
    if (required_capacity > buckets_.size()) {
        resize(required_capacity);
    }
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void hash_table<K, V, Hash, KeyEqual>::clear() {
    for (typename std::vector<bucket_type>::iterator bucket = buckets_.begin();
         bucket != buckets_.end();
         ++bucket) {
        bucket->clear();
    }
    size_ = 0U;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void hash_table<K, V, Hash, KeyEqual>::swap(hash_table &other) {
    using std::swap;
    buckets_.swap(other.buckets_);
    swap(size_, other.size_);
    swap(minimum_capacity_, other.minimum_capacity_);
    swap(max_load_factor_, other.max_load_factor_);
    swap(hasher_, other.hasher_);
    swap(keys_equal_, other.keys_equal_);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
template<typename Function>
void hash_table<K, V, Hash, KeyEqual>::for_each(Function function) const {
    for (typename std::vector<bucket_type>::const_iterator bucket = buckets_.begin();
         bucket != buckets_.end();
         ++bucket) {
        for (typename bucket_type::const_iterator node = bucket->begin();
             node != bucket->end();
             ++node) {
            function(node->get_key(), node->get_value());
        }
    }
}

template<typename K, typename V, typename Hash, typename KeyEqual>
std::size_t hash_table<K, V, Hash, KeyEqual>::size() const {
    return size_;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
std::size_t hash_table<K, V, Hash, KeyEqual>::capacity() const {
    return buckets_.size();
}

template<typename K, typename V, typename Hash, typename KeyEqual>
bool hash_table<K, V, Hash, KeyEqual>::empty() const {
    return size_ == 0U;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
double hash_table<K, V, Hash, KeyEqual>::load_factor() const {
    return static_cast<double>(size_) / static_cast<double>(buckets_.size());
}

#endif
