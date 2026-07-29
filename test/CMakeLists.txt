cmake_minimum_required(VERSION 3.14)
project(p101_util_tests C CXX)
enable_testing()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(P101_TEST_COVERAGE "Instrument the tests for coverage" OFF)
set(P101_TEST_COVERAGE_FLAGS "")
if(P101_TEST_COVERAGE)
    set(P101_TEST_COVERAGE_FLAGS --coverage)
endif()

set(P101_PUBLIC_INCLUDE_DIRS "" CACHE STRING "Extra p101 include dirs")
set(P101_PUBLIC_LINK_DIRS "" CACHE STRING "Extra p101 link dirs")
separate_arguments(P101_PUBLIC_INCLUDE_DIRS_LIST NATIVE_COMMAND "${P101_PUBLIC_INCLUDE_DIRS}")
separate_arguments(P101_PUBLIC_LINK_DIRS_LIST NATIVE_COMMAND "${P101_PUBLIC_LINK_DIRS}")

if(P101_PUBLIC_LINK_DIRS_LIST)
    find_library(P101_UTIL_LIBRARY NAMES p101_util PATHS ${P101_PUBLIC_LINK_DIRS_LIST} NO_DEFAULT_PATH)
    find_library(P101_ENV_LIBRARY NAMES p101_env PATHS ${P101_PUBLIC_LINK_DIRS_LIST} NO_DEFAULT_PATH)
    find_library(P101_ERROR_LIBRARY NAMES p101_error PATHS ${P101_PUBLIC_LINK_DIRS_LIST} NO_DEFAULT_PATH)
else()
    find_library(P101_UTIL_LIBRARY NAMES p101_util)
    find_library(P101_ENV_LIBRARY NAMES p101_env)
    find_library(P101_ERROR_LIBRARY NAMES p101_error)
endif()
if(NOT P101_UTIL_LIBRARY OR NOT P101_ENV_LIBRARY OR NOT P101_ERROR_LIBRARY)
    message(FATAL_ERROR "Could not find p101_util, p101_env, and p101_error")
endif()

set(P101_TEST_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../include" ${P101_PUBLIC_INCLUDE_DIRS_LIST})
set(P101_TEST_LIBRARIES "${P101_UTIL_LIBRARY}" "${P101_ENV_LIBRARY}" "${P101_ERROR_LIBRARY}")

set(P101_C_TESTS
        test_lib_util
        test_native_first
        test_p101_first
)

foreach(target IN LISTS P101_C_TESTS)
    add_executable(${target} ${target}.c)
    target_include_directories(${target} PRIVATE ${P101_TEST_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${P101_TEST_LIBRARIES})
    add_test(NAME ${target} COMMAND ${target})
endforeach()

add_executable(test_headers_cxx test_headers.cpp)
target_include_directories(test_headers_cxx PRIVATE ${P101_TEST_INCLUDE_DIRS})
target_link_libraries(test_headers_cxx PRIVATE ${P101_TEST_LIBRARIES})
add_test(NAME test_headers_cxx COMMAND test_headers_cxx)

add_executable(test_runtime_fallback test_runtime_fallback.c ../src/endian.c)
target_include_directories(test_runtime_fallback PRIVATE ${P101_TEST_INCLUDE_DIRS})
target_link_libraries(test_runtime_fallback PRIVATE "${P101_ENV_LIBRARY}" "${P101_ERROR_LIBRARY}")
add_test(NAME test_runtime_fallback COMMAND test_runtime_fallback)

target_compile_options(test_lib_util PRIVATE ${P101_TEST_COVERAGE_FLAGS})
target_link_options(test_lib_util PRIVATE ${P101_TEST_COVERAGE_FLAGS})

if(APPLE)
    foreach(target IN ITEMS ${P101_C_TESTS} test_headers_cxx)
        target_compile_definitions(${target} PRIVATE _DARWIN_C_SOURCE)
    endforeach()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    foreach(target IN ITEMS ${P101_C_TESTS} test_headers_cxx)
        target_compile_definitions(${target} PRIVATE _GNU_SOURCE)
    endforeach()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    foreach(target IN ITEMS ${P101_C_TESTS} test_headers_cxx)
        target_compile_definitions(${target} PRIVATE _BSD_SOURCE)
    endforeach()
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    foreach(target IN LISTS P101_C_TESTS)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Werror -pedantic-errors)
    endforeach()
    target_compile_options(test_runtime_fallback PRIVATE
            -Wall
            -Wextra
            -Werror
            -pedantic-errors
            -U__BYTE_ORDER__
            -U__ORDER_LITTLE_ENDIAN__
            -U__ORDER_BIG_ENDIAN__
    )
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(test_headers_cxx PRIVATE -Wall -Wextra -Werror -pedantic-errors)
endif()
