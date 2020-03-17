# Building

## Targets
Targets and their files are listed in [/build/makefile.am]().

### Which Target should I build?
Files Edited               | storm                  | stormcompiler    | stormrun         |
---------------------------|:----------------------:|:----------------:|:----------------:|
[storm.cc](/Storm/main.cc) |:heavy_check_mark:      |:x:               |:x:               |
[util.cc](/Storm/util.cc)  |:x:                     |:heavy_check_mark:|:heavy_check_mark:|
interpreter/\*             |:x:                     |:x:               |:heavy_check_mark:|
parser/\*                  |:x:                     |:heavy_check_mark:|:x:               |

## Debugging

### Setup
Set up debugging with:
```sh
$ autoreconf --install
$ make clean
$ ./configure --enable-debug=yes
$ make -j$(nproc) all
```

### Change 
Change debug setting with
```sh
$ make clean
$ ./configure --enable-debug=[yes/no]
$ make -j$(nproc) all
```

if you building non debug, run stormcompiler or stormrun independently, as in non-debug mode storm looks for these files in PATH.
