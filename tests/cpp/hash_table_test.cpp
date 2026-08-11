#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include <hash_table/cpp/hash_table.hpp>

struct collision_hash {
    std::size_t operator()(const std::string &) const { return 0U; }
};

struct throwing_hash {
    static int calls_until_throw;

    std::size_t operator()(const int value) const {
        if (calls_until_throw == 0) {
            throw std::runtime_error("hash failed");
        }
        --calls_until_throw;
        return static_cast<std::size_t>(value);
    }
};

int throwing_hash::calls_until_throw = 100;

static void check(const bool condition, const char *expression, const int line) {
    if (!condition) {
        std::cerr << "check failed at line " << line << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK(expression) check((expression), #expression, __LINE__)

int main() {
    hash_table<std::string, int, collision_hash> table(5, 0.5);
    CHECK(table.empty());
    CHECK(table.capacity() == 5U);

    for (int index = 0; index < 100; ++index) {
        CHECK(table.insert_or_assign("key-" + std::to_string(index), index));
    }
    CHECK(table.size() == 100U);
    CHECK(table.capacity() > 5U);

    for (int index = 0; index < 100; ++index) {
        int value = -1;
        CHECK(table.find("key-" + std::to_string(index), value));
        CHECK(value == index);
    }

    CHECK(!table.insert_or_assign("key-42", 420));
    CHECK(table.size() == 100U);
    int updated = 0;
    CHECK(table.find("key-42", updated));
    CHECK(updated == 420);
    CHECK(table.contains("key-42"));
    CHECK(!table.contains("missing"));

    int *mutable_value = table.find("key-42");
    CHECK(mutable_value != NULL);
    *mutable_value = 421;
    const hash_table<std::string, int, collision_hash> &const_table = table;
    const int *const_value = const_table.find("key-42");
    CHECK(const_value != NULL);
    CHECK(*const_value == 421);

    int visited = 0;
    int value_sum = 0;
    table.for_each([&visited, &value_sum](const std::string &, const int value) {
        ++visited;
        value_sum += value;
    });
    CHECK(visited == 100);
    CHECK(value_sum == 5329);

    table.reserve(1000U);
    CHECK(table.capacity() >= 2000U);

    hash_table<std::string, int, collision_hash> copy = table;
    CHECK(copy.remove("key-42"));
    CHECK(!copy.remove("key-42"));
    CHECK(table.find("key-42", updated));

    for (int index = 0; index < 100; ++index) {
        table.remove("key-" + std::to_string(index));
    }
    CHECK(table.empty());
    CHECK(table.capacity() == 5U);

    hash_table<std::string, int> clearable;
    clearable.insert("one", 1);
    clearable.reserve(100U);
    const std::size_t capacity_before_clear = clearable.capacity();
    clearable.clear();
    CHECK(clearable.empty());
    CHECK(clearable.capacity() == capacity_before_clear);

    bool rejected_capacity = false;
    try {
        hash_table<int, int> invalid(0);
    } catch (const std::invalid_argument &) {
        rejected_capacity = true;
    }
    CHECK(rejected_capacity);

    bool rejected_factor = false;
    try {
        hash_table<int, int> invalid(10, 1.5);
    } catch (const std::invalid_argument &) {
        rejected_factor = true;
    }
    CHECK(rejected_factor);

    bool rejected_nan_factor = false;
    try {
        hash_table<int, int> invalid(10, std::numeric_limits<double>::quiet_NaN());
    } catch (const std::invalid_argument &) {
        rejected_nan_factor = true;
    }
    CHECK(rejected_nan_factor);

    hash_table<int, int> low_load_table(5, 0.01);
    low_load_table.insert(1, 1);
    CHECK(low_load_table.load_factor() <= 0.01);

    hash_table<int, int> move_source;
    move_source.insert(1, 10);
    hash_table<int, int> move_destination(std::move(move_source));
    CHECK(move_destination.contains(1));
    CHECK(move_source.empty());
    CHECK(!move_source.contains(1));
    move_source.insert(2, 20);
    CHECK(move_source.contains(2));

    hash_table<int, int> move_assignment_target;
    move_assignment_target = std::move(move_destination);
    CHECK(move_assignment_target.contains(1));
    CHECK(move_destination.empty());
    CHECK(!move_destination.contains(1));

    throwing_hash::calls_until_throw = 100;
    hash_table<int, int, throwing_hash> exception_safe_table;
    exception_safe_table.insert(1, 1);
    exception_safe_table.insert(2, 2);
    exception_safe_table.insert(3, 3);
    throwing_hash::calls_until_throw = 1;
    bool hash_threw = false;
    try {
        exception_safe_table.reserve(100U);
    } catch (const std::runtime_error &) {
        hash_threw = true;
    }
    CHECK(hash_threw);
    throwing_hash::calls_until_throw = 100;
    CHECK(exception_safe_table.size() == 3U);
    CHECK(exception_safe_table.contains(1));
    CHECK(exception_safe_table.contains(2));
    CHECK(exception_safe_table.contains(3));

    throwing_hash::calls_until_throw = 100;
    hash_table<int, int, throwing_hash> remove_exception_safe_table;
    remove_exception_safe_table.reserve(100U);
    remove_exception_safe_table.insert(1, 1);
    remove_exception_safe_table.insert(2, 2);
    throwing_hash::calls_until_throw = 1;
    CHECK(remove_exception_safe_table.remove(1));
    throwing_hash::calls_until_throw = 100;
    CHECK(!remove_exception_safe_table.contains(1));
    CHECK(remove_exception_safe_table.contains(2));
    CHECK(remove_exception_safe_table.size() == 1U);

    hash_table<int, int> sparse_table;
    sparse_table.reserve(100000U);
    sparse_table.insert(1, 1);
    CHECK(sparse_table.remove(1));
    CHECK(sparse_table.capacity() == 31U);
    return 0;
}
