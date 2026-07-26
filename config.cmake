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
        -Werror
)

set(DARWIN_STANDARD_FLAGS
        -D_DARWIN_C_SOURCE
)

set(LINUX_STANDARD_FLAGS
        -D_DEFAULT_SOURCE
        -D_GNU_SOURCE
)

set(BSD_STANDARD_FLAGS
        -D__BSD_VISIBLE
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
