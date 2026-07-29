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
        -Werror
)

set(DARWIN_STANDARD_FLAGS)
set(LINUX_STANDARD_FLAGS)
set(BSD_STANDARD_FLAGS)

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
        p101_env
)
