# Building

## Targets

Targets and their files are listed in [build/Makefile.am](/build/Makefile.am).

## Debugging

### Setup

Set up debugging with

```console
autoreconf --install
make clean
./configure --enable-debug=yes
make -j$(nproc) all
```

### Change

Change debug setting with

```console
make clean
./configure --enable-debug=[yes/no]
make -j$(nproc) all
```

if you building non debug, run stormcompiler or stormrun independently, as in non-debug mode storm looks for these files in PATH.
