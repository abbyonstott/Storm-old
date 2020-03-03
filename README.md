# Storm
Storm is a general-purpose programming language.

## Build Instructions

Requires CMake 3.0 or higher

**On Unix:**
```sh
$ cd build
$ autoreconf --install
$ ./configure
$ make -j$(nproc)
```

## How to use

Compile a Program
```sh
$ storm -c <file_name> <binary_name>
```
Binary names are usually followed by .stormc, but the file extension is up to the person compiling

In this project they should **always** have the .stormc file extension

Run a Program
```sh
$ storm <binary_name>
```