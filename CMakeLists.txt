# for c language
#cmake_minimum_required(VERSION 3.25)
#project(hash_table C)
#
#set(CMAKE_C_STANDARD 11)
#
#aux_source_directory(implement_by_c SRC_DIR)
#
#add_executable(hash_table ${SRC_DIR})



# for cpp language
cmake_minimum_required(VERSION 3.25)
project(hash_table)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(implement_by_cpp SRC_DIR)

add_executable(hash_table ${SRC_DIR})
