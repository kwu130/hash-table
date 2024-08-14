cmake_minimum_required(VERSION 3.25)

# for c language
project(hash_table_c C)

set(CMAKE_C_STANDARD 11)

aux_source_directory(implement_by_c SRC_DIR_C)

add_executable(hash_table_c ${SRC_DIR_C})


# for cpp language
project(hash_table_cpp)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(implement_by_cpp SRC_DIR_CPP)

add_executable(hash_table_cpp ${SRC_DIR_CPP})
