#ifndef HASH_TABLE_CPP_DETAIL_PRIME_HPP
#define HASH_TABLE_CPP_DETAIL_PRIME_HPP

#include <cstddef>

namespace hash_table_detail {

inline bool is_prime(const std::size_t value) {
    if (value < 2U) {
        return false;
    }
    if (value == 2U) {
        return true;
    }
    if (value % 2U == 0U) {
        return false;
    }

    for (std::size_t divisor = 3U; divisor <= value / divisor; divisor += 2U) {
        if (value % divisor == 0U) {
            return false;
        }
    }
    return true;
}

inline std::size_t next_prime(std::size_t value) {
    if (value < 2U) {
        return 2U;
    }
    while (!is_prime(value)) {
        ++value;
    }
    return value;
}

} // namespace hash_table_detail

#endif
