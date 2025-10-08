# Project metadata
set(PROJECT_NAME "p101_unix")
set(PROJECT_VERSION "0.0.1")
set(PROJECT_DESCRIPTION "Unix libraries")
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
set(LIBRARY_TARGETS p101_unix)

# Define source files per library
set(p101_unix_SOURCES
        src/err.c
        src/fstab.c
        src/getopt.c
        src/ifaddrs.c
        src/resolv.c
        src/stdlib.c
        src/ttyent.c
        src/arpa/nameser.c
        src/sys/mount.c
        src/sys/timex.c
)

# Define header files per library
set(p101_unix_HEADERS
        include/p101_unix/p101_err.h
        include/p101_unix/p101_fstab.h
        include/p101_unix/p101_getopt.h
        include/p101_unix/p101_ifaddrs.h
        include/p101_unix/p101_resolv.h
        include/p101_unix/p101_stdlib.h
        include/p101_unix/p101_ttyent.h
        include/p101_unix/arpa/p101_nameser.h
        include/p101_unix/sys/p101_mount.h
        include/p101_unix/sys/p101_timex.h
)

# Define linked libraries per library
set(p101_unix_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
)
