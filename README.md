# Hash Table

A separate-chaining hash table implemented in both C11 and C++11. The two
implementations share the same behavior: inserting an existing key updates its
value, buckets grow above the configured maximum load, and shrink without going
below their initial capacity.

## Highlights

- Collision handling with separate chaining
- Prime-number bucket counts for a more even distribution
- Automatic growth and shrinkage
- Capacity reservation, clearing, membership checks, and full-table traversal
- Allocation-safe C string ownership
- RAII and safe copy/move semantics in C++
- CTest coverage for insert, update, lookup, delete, collision, resize, and copy

## Project layout

```text
.
├── include/hash_table/
│   ├── c/hash_table.h          # public C API
│   └── cpp/
│       ├── hash_table.hpp      # public C++ API
│       └── detail/             # template implementation details
├── src/c/                      # C implementation and private headers
├── examples/{c,cpp}/           # runnable examples
└── tests/{c,cpp}/              # language-specific tests
```

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The build creates two examples:

```sh
./build/hash_table_c
./build/hash_table_cpp
```

Strict compiler warnings are enabled by default. For a sanitizer build:

```sh
cmake -S . -B build-sanitized \
  -DCMAKE_BUILD_TYPE=Debug \
  -DHASH_TABLE_ENABLE_SANITIZERS=ON
cmake --build build-sanitized --parallel
ctest --test-dir build-sanitized --output-on-failure
```

Examples are controlled by `HASH_TABLE_BUILD_EXAMPLES` and default to enabled
only for standalone builds. Standalone tests use the standard `BUILD_TESTING`
option as their single switch. When included through `add_subdirectory`, tests
default to disabled and can be enabled with `HASH_TABLE_BUILD_TESTS`; the parent
must also call `include(CTest)` with `BUILD_TESTING=ON` so CTest can discover
them from the top-level build directory.

## C API

```c
#include <hash_table/c/hash_table.h>

ht_hash_table *table = ht_new();
ht_reserve(table, 100);
if (!ht_insert_or_assign(table, "language", "C11")) {
    /* invalid input or allocation failure */
}

const char *value = ht_search(table, "language");
if (ht_contains(table, "language")) {
    ht_remove(table, "language");
}
ht_del_hash_table(table);
```

The table owns copies of inserted keys and values. A pointer returned by
`ht_search` remains valid until that key is updated or deleted, or the table is
destroyed.

Consumers can link the CMake target `hash_table::c` (`hash_table_c_lib` remains
available as its concrete target).

| Operation | API |
| --- | --- |
| Create with an initial capacity | `ht_new_with_capacity` |
| Insert or update with status | `ht_insert_or_assign` |
| Lookup / membership | `ht_search`, `ht_contains` |
| Remove with status | `ht_remove` |
| Preallocate for entries | `ht_reserve` |
| Remove all entries | `ht_clear` |
| Inspect size and load | `ht_count`, `ht_capacity`, `ht_load_factor` |
| Visit every entry | `ht_foreach` |

`ht_foreach` stops early when its callback returns zero. The callback must not
mutate the table while traversal is in progress.

## C++ API

```cpp
#include <hash_table/cpp/hash_table.hpp>

hash_table<std::string, int> table;
table.reserve(100);
table.insert_or_assign("answer", 42);

if (int *value = table.find("answer")) {
    *value += 1;
}

table.for_each([](const std::string &key, const int value) {
    // inspect key and value
});
```

The C++ implementation supports custom hash and equality functors through its
third and fourth template parameters. Consumers can link the header-only CMake
target `hash_table::cpp` (`hash_table_cpp_lib` remains available as its concrete
target).

The original `insert` and output-parameter `find` APIs remain available.
`insert_or_assign` returns `true` for a new key and `false` for an update;
`remove` reports whether a key was present. `reserve` preallocates buckets for an
expected number of entries, while `clear` retains that capacity for reuse.
