# hash-table
A simple hash table implemention by C and C++
## implement_by_c
use c language implement a hash table
## implement_by_cpp
use c++ language implement a hash table

# usage
CMake file include two hash tables build scripts:

- c 
```
# for c language
cmake_minimum_required(VERSION 3.25)
project(hash_table C)

set(CMAKE_C_STANDARD 11)

aux_source_directory(implement_by_c SRC_DIR)

add_executable(hash_table ${SRC_DIR})
```
- c++
```
# for cpp language
cmake_minimum_required(VERSION 3.25)
project(hash_table)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(implement_by_cpp SRC_DIR)

add_executable(hash_table ${SRC_DIR})
```
you only need use one of scripts when build this project



