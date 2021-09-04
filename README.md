# Wrench

**Wrench** is a set of cross-platform single-header C++ libraries. All of them were developed during work on my personal projects.

## Table of contents

1. ### [List of libraries](#libraries-list)
2. ### [How to Use](#how-to-use)

***

### List of Libraries<a name="libraries-list"></a>

* **[MemTracker](source/memTracker.hpp)** - The library is a diagnostic utility that overloads new/delete operators to control allocations and memory leaks.
* **[Result](source/result.hpp)** - The library provides a mix of Alexandrescu's std::expected and Result<T, E> type from Rust programming language.
* **[StringUtils](source/stringUtils.hpp)** - A bunch of helper functions that simplify work with std::string.
* **[Variant](source/variant.hpp)** - A lightweight yet simple implementation of type-safe unions. That works under C++0x standard.

***

### How to Use<a name="how-to-use"></a>

All the libraries have documentation at the beginning of their sources. And you can find relative detailed information up from there. 
To use some library in your own project just copy its source file in project's directory and use the following lines of code:
```cpp
/// Use the header when you need the declarations only
#include "library_name.hpp"

/// The macro above should be placed once at some cpp file
#define LIBRARY_NAME_IMPLEMENTATION
#include "library_name.hpp"
``` 
where **LIBRARY_NAME** is one of the listed above. 