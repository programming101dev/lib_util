# lib_util Repository Guide

`lib_util` provides small, portable utilities that do not belong to the ISO C,
POSIX, XSI, or common-Unix wrapper libraries. It contains the endian conversion
family in `<p101_util/endian.h>` and the subprocess capture mechanism in
`<p101_util/tool_run.h>`.

## Contract and limits

- **Admitted inputs:** fixed-width 16-, 32-, and 64-bit unsigned integers;
  caller-supplied argument strings; executable argument vectors; and optional
  `p101_env`/`p101_error` objects.
- **Outputs:** byte-swapped values, host/little/big-endian conversions, host
  byte-order detection, owned argument vectors, child output streams, native
  wait statuses, and balanced p101 call events when an environment is supplied.
- **Error behavior:** the total integer operations do not use a `p101_error`;
  every input value has a result. Allocation, pipe, fork, descriptor, stream,
  exec, and wait failures in the subprocess helpers are raised through the
  supplied error object.
- **Portability:** the implementation uses the compiler byte-order constants
  when available and a byte probe otherwise. It does not define or replace the
  platform `bswap*`, `htobe*`, `htole*`, `be*toh`, or `le*toh` names.
- **Blind spots:** direct native endian macros do not emit p101 events.

The subprocess API is deliberately mechanism-only. `p101_tool_argv_*` builds an
owned, NULL-terminated argument vector, while `p101_tool_read_pipe_*` executes
that vector directly and exposes the child's output as a readable stream.
`p101_tool_run_capture` redirects a child process, optionally invokes a
caller-owned child setup callback, executes the requested command, and returns
the native wait status. None of these functions invoke a shell, choose tools,
clear environment variables, interpret exit codes, or judge findings; those
policies remain in the calling tool. Direct argv execution is the safety
boundary that keeps paths containing spaces or shell metacharacters as data.

## **Table of Contents**

1. [Cloning the Repository](#cloning-the-repository)
2. [Prerequisites](#prerequisites)
3. [Configuring the Build](#configuring-the-build)
4. [Building](#building)
5. [Testing](#testing)
6. [Installing](#installing)
7. [Adding or Removing Files](#adding-or-removing-files)

## **Cloning the Repository**

Clone the repository using the following command:

```bash
git clone https://github.com/programming101dev/lib_util.git
```

Navigate to the cloned directory:

```bash
cd lib_util
```

Ensure the scripts are executable:

```bash
chmod +x *.sh
```

## **Prerequisites**

To ensure you have all of the required tools installed, run:

```bash
./check-env.sh
```

If you are missing tools follow these [instructions](https://docs.google.com/document/d/1ZPqlPD1mie5iwJ2XAcNGz7WeA86dTLerFXs9sAuwCco/edit?usp=drive_link). If something still looks wrong, `./doctor.sh` reports what actually works on this machine for this project.

## **Configuring the Build**

Tell CMake which compiler you want to use:

```bash
./change-compiler.sh -c <compiler>
```

To see the list of possible compilers:

```bash
cat supported_c_compilers.txt
```

Run it again any time to switch compilers; each compiler configures into its own build directory (e.g. `build-clang`, `build-gcc-15`).

## **Building**

To build the library run:

```bash
./build.sh
```

This compiles through the strict analysis pipeline: the clang-format check, clang-tidy, cppcheck, the Clang static analyzer, and hundreds of warnings under `-Werror`. `./build.sh -f` applies the formatter and tidy fixes in place.

## **Testing**

`./check.sh` is the one command to run before you submit: the format check, the strict build, the tests, and a short fuzz smoke run, with a single PASS/FAIL at the end.
The deterministic suite checks C and C++ public-header compatibility, both
include orders with native endian headers, every conversion width, round trips,
single argument evaluation, balanced tracing, and every possible 16-bit
byte-swap input. Fuzzing is intentionally omitted: the fixed-width arithmetic
has a finite, deterministic contract that unit tests cover more directly.

## **Using the API**

```c
#include <p101_util/endian.h>

uint32_t wire_value;
uint32_t host_value = UINT32_C(0x12345678);

wire_value = p101_htobe32(env, host_value);
host_value = p101_be32toh(env, wire_value);
```

The public functions are `p101_bswap16/32/64`, `p101_htole16/32/64`,
`p101_htobe16/32/64`, `p101_le16toh/32/64`, `p101_be16toh/32/64`, and
`p101_is_little_endian`.

## **Installing**

To install the library run:

```bash
./install.sh
```

You may need to run it via sudo, or give the user account access to the install directories. `./uninstall.sh` removes it again.

## **Adding or Removing Files**

The `CMakeLists.txt` is fixed and shared across every repository — do not edit it. When you add or remove a source or header, edit the lists in `config.cmake` (`p101_util_SOURCES`, `p101_util_HEADERS`, and `p101_util_LINK_LIBRARIES`), then re-configure and build:

```bash
./change-compiler.sh -c <compiler>
./build.sh
```
