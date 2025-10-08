# Project metadata
set(PROJECT_NAME "p101_util")
set(PROJECT_VERSION "0.0.1")
set(PROJECT_DESCRIPTION "Utility libraries")
set(PROJECT_LANGUAGE "C")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Common compiler flags
set(STANDARD_FLAGS
        -D_POSIX_C_SOURCE=200809L
        -D_XOPEN_SOURCE=700
        -D_GNU_SOURCE
        -D_DARWIN_C_SOURCE
        -D__BSD_VISIBLE
        -Werror
)

# Define library targets
set(LIBRARY_TARGETS p101_util)

# Define source files per library
set(p101_util_SOURCES
        src/endian.c
)

# Define header files per library
set(p101_util_HEADERS
        include/p101_util/endian.h
)

# Define linked libraries per library
set(p101_util_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
)
