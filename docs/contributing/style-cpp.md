# C++ Style

<!-- Use four spaces instead of tabs in this file -->

## Table of Contents

- [Note](#note)
- [Standard](#standard)
- [Naming](#naming)
- [Header vs Source](#headers-vs-source)
- [Braces](#braces)
- [If statements](#if-statements)
- [Switches](#switches)
- [Functions](#functions)
- [For Loops](#for-loops)
- [While Loops](#while-loops)
- [Classes and Struct](#classes-and-structs)
- [Portability](#portability)

## Note

- C++ source files must use the `.cc` extension, and all header files must use the `.h` extension.
- All operators must be spaced out

## Standard

This project uses the **C++17** standard, but this will change with each revision.

## Naming

- Variable names should be descriptive of their function. (e.g. `loc` is an iterator in the interpreter)
- Words should be separated by capital letters starting on the second word or underscores.
- Avoid using one letter names. (Except for objects that are used temporarily, e.g. `variable v`)

## Headers vs. Source

- Every header file should have a related source file, but every source file does not need its own header file.

- `#defines` should be put in a header file.

- When making classes or structs, **always** put them in a header file. Function and method definitions should always go in source files, but forward declarations should go in header files.

- Class constructors and destructors should be put in source files, with the exception of `default`.

- All includes should be put in header files.

A typical source file and its related header would look something like this.

```cpp
// mysource.h
#pragma once

#include <externalheader.h>

class myClass {
public:
    int member;

    void someMethod();

    myClass() = default;
    myClass(int _member);

    ~myClass() = default;
};

void myFunction(int arg);
```

```cpp
// mysource.cc
#include "mysource.h"

myClass::myClass(int _member) {
    member = _member;
}

void myClass::someMethod() {
    // do something
}

void myFunction(int arg) {
    myClass();
}
```

## Braces

- Braces should be placed on the same line as their parent, unless the parent expression spans multiple lines.
- There should be a space between the parent expression and the opening brace.
- There should be a newline between the closing brace and the next statement.
- Don't use braces for one line scopes (obviously with the exception of functions).

Example:

```cpp
void myFunc() {
    // braces here
    if (expr) {
        doSomething();
        variable++;
    }

    // no braces here
    if (otherExpr)
        variable++;
}
```

## If statements

- Put a space between `if` and the expression.
- Do not put a space between the parentheses and the expression. For instance, `(x == 2)` not `( x == 2 )`. **this applies to all binary expressions**
- Use logical operators (`&&`, `||`, etc.) sparingly.
- When using logical operators, adding parentheses around the expressions is optional.
- Put a space between operands and operators.

Example:

```cpp
// OK
if (someValue == foo && otherValue == bar)
    // do something

// also OK
if ((someValue == foo) && (otherValue == bar))
    // do something
```

- If you are evaluating a `bool`, use only the name.

```cpp
// OK
if (booleanVar)
    // do something

// Don't do this
if (booleanVar == true)
    // do something
```

## Switches

- When evaluating a variable for a specific set of values, use a switch.
- The `case` statements should have an extra level of indentation.
- Put the body of the `case` on the next line.

Example:

```cpp
char c;

// OK
switch (c) {
    default:
        break;
}

// Don't do this
switch (c)
{
    default: break;
}
```

- If a variable is defined inside of a case statement, add a scope starting on the next line for each case statement.
- Make sure the cases cover all possible values (use `default`).
- Some compilers, such as gcc, allow for ellipsis to be used in case statements. **Do not use this**. If you have a large range, use an if statement instead.

Example:

```cpp

char c;

// OK
switch (c) {
    case 'h':
    {
        // value defiend
        int value;
        break;
    }
    case 'e':
    {
        // even if no value defined here
        break;
    }
    default:
    {
        break;
    }
}

// Don't do this
switch (c) {
    case 'h':
    {
        int v;
        break;
    }
    case 'e':
        break;
    default:
        break;
}
```

## Functions

- A value should **always** be returned in a non-void function.
- Most functions should be void.
- Put a space between the comma in the argument list.
- Put a space between the closing parenthesis and the opening brace.
- Use pointers or references when necessary.

Example:

```cpp
// OK
std::string myFunc(std::string myArg1, int myArg2) {
    return myArg1 + myArg2;
}

// Don't do this
std::string myFunc(std::string myArg1,int myArg2) {
    return myArg1 + myArg2;
}
```

## For loops

- When using a type with an iterator, use a range-based for loop if you are reading every value in order.
- If you need to manipulate the iterator, use an iterator object instead of an int subscript to access values.

Example:

```cpp
std::string myStr = "Hello, World!";

// OK
for (char c : myStr)
    foo(c);

// OK
for (std::string::iterator c; c != myStr.end(); c++)
    foo(*c);

// Don't do this
for (int i = 0; i < myStr.size(); i++)
    foo(myStr[i]);
```

## While loops

- While loops should always have an exit condition.
- Put space around the expression.
- Use `break` and `continue` where necessary.

## Classes and Structs

- Use classes to represent an object and structs to represent a set of related data
- Classes and structs should be defined in headers
- Use `inline` for structs that declare a variable with them

Example:

```cpp
// Use a class for this
class variable {
    ...
public:
    ...
};

// use a struct for this
inline struct program_t {
    ...
} program;
```

## Portability

The goal of Storm is to be compatible with every platform. Every bit of Unix code should have a Windows api alternative. This is accomplished with the `_WIN32` preprocessor value.

Example:

```cpp
#ifndef _WIN32
    // Unix code
#else
    // Windows api alternative
#endif
```
