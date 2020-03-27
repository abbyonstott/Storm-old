# Contributing

## Table of Contents

- [Getting Started](#getting-started)
- [Building](#building)
- [Making Changes](#making-changes)
- [Making a Pull Request](#making-a-pull-request)

## Getting Started

Fork the repository and clone with

```console
git clone https://github.com/your-name/Storm
```

To sync your local repo with master, run
```console
git fetch upstream
git merge upstream/master
```

## Building

For instructions on how to build, see [building](/building)

## Making changes

Before making changes to any C++ code, see [the C++ style guide](/contributing/style-cpp)

If you need to reference bytecode values. The complete list of them is in [bytecode](/contributing/bytecode)

Checkout a branch on your local repository.

After making changes, run the tests.

- Linux/MacOS

```sh
cd scripts
./test.sh
```

- Windows

```bat
cd scripts
.\test.bat
```

**Make sure that all of the tests pass.**

If you are unsure of why a test isn't passing, create a pull request, push, and ask for help.

## Making a Pull Request

- Make a pull request to master with your branch.
- Wait for review.
