cmake_minimum_required(VERSION 3.16)

project(hash_table VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(HASH_TABLE_IS_TOP_LEVEL ON)
else()
    set(HASH_TABLE_IS_TOP_LEVEL OFF)
endif()

if(HASH_TABLE_IS_TOP_LEVEL)
    include(CTest)
    set(HASH_TABLE_TESTS_ENABLED ${BUILD_TESTING})
else()
    option(HASH_TABLE_BUILD_TESTS "Build the hash table tests" OFF)
    set(HASH_TABLE_TESTS_ENABLED OFF)
    if(HASH_TABLE_BUILD_TESTS)
        get_property(HASH_TABLE_CTEST_ENABLED GLOBAL PROPERTY CTEST_TARGETS_ADDED)
        if(NOT BUILD_TESTING OR NOT HASH_TABLE_CTEST_ENABLED)
            message(FATAL_ERROR
                "HASH_TABLE_BUILD_TESTS requires BUILD_TESTING=ON and include(CTest) in the top-level project"
            )
        endif()
        set(HASH_TABLE_TESTS_ENABLED ON)
    endif()
endif()

option(HASH_TABLE_ENABLE_WARNINGS "Enable strict compiler warnings" ON)
option(HASH_TABLE_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(HASH_TABLE_BUILD_EXAMPLES "Build the hash table examples" ${HASH_TABLE_IS_TOP_LEVEL})

function(hash_table_set_project_options target)
    if(HASH_TABLE_ENABLE_WARNINGS)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        elseif(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endif()
endfunction()

add_library(hash_table_sanitizers INTERFACE)
if(HASH_TABLE_ENABLE_SANITIZERS)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(hash_table_sanitizers INTERFACE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(hash_table_sanitizers INTERFACE -fsanitize=address,undefined)
    else()
        message(WARNING "HASH_TABLE_ENABLE_SANITIZERS is not supported by the selected compilers")
    endif()
endif()

add_library(hash_table_c_lib STATIC
    src/c/hash_table.c
)
add_library(hash_table::c ALIAS hash_table_c_lib)
set_target_properties(hash_table_c_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(hash_table_c_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(hash_table_c_lib PUBLIC c_std_11)
target_link_libraries(hash_table_c_lib PUBLIC hash_table_sanitizers)
hash_table_set_project_options(hash_table_c_lib)

add_library(hash_table_cpp_lib INTERFACE)
add_library(hash_table::cpp ALIAS hash_table_cpp_lib)
target_include_directories(hash_table_cpp_lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(hash_table_cpp_lib INTERFACE cxx_std_11)
target_link_libraries(hash_table_cpp_lib INTERFACE hash_table_sanitizers)

if(HASH_TABLE_BUILD_EXAMPLES)
    add_executable(hash_table_c examples/c/main.c)
    target_link_libraries(hash_table_c PRIVATE hash_table::c)
    hash_table_set_project_options(hash_table_c)

    add_executable(hash_table_cpp examples/cpp/main.cpp)
    target_link_libraries(hash_table_cpp PRIVATE hash_table::cpp)
    hash_table_set_project_options(hash_table_cpp)
endif()

if(HASH_TABLE_TESTS_ENABLED)
    add_executable(hash_table_c_tests tests/c/hash_table_test.c)
    target_link_libraries(hash_table_c_tests PRIVATE hash_table::c)
    hash_table_set_project_options(hash_table_c_tests)
    add_test(NAME hash_table_c_tests COMMAND hash_table_c_tests)

    add_executable(hash_table_cpp_tests tests/cpp/hash_table_test.cpp)
    target_link_libraries(hash_table_cpp_tests PRIVATE hash_table::cpp)
    hash_table_set_project_options(hash_table_cpp_tests)
    add_test(NAME hash_table_cpp_tests COMMAND hash_table_cpp_tests)

    add_executable(hash_table_c_from_cpp_tests tests/cpp/c_api_test.cpp)
    target_link_libraries(hash_table_c_from_cpp_tests PRIVATE hash_table::c)
    hash_table_set_project_options(hash_table_c_from_cpp_tests)
    add_test(NAME hash_table_c_from_cpp_tests COMMAND hash_table_c_from_cpp_tests)
endif()
