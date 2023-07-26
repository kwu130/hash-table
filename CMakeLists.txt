cmake_minimum_required(VERSION 3.25)
project(hash_table C)

set(CMAKE_C_STANDARD 11)

aux_source_directory(. SRC_DIR)

add_executable(hash_table ${SRC_DIR})
