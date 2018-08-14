# Boost.Static_views [![Travis Status](https://travis-ci.org/BoostGSoC17/static-views.svg?branch=development)](https://travis-ci.org/BoostGSoC17/static-views) [![Appveyor status](https://ci.appveyor.com/api/projects/status/gydxsxfm0o5t311e/branch/development?svg=true)](https://ci.appveyor.com/project/twesterhout/static-views/branch/development) [![License](https://img.shields.io/badge/license-Boost-blue.svg?style=flat)](https://www.boost.org/LICENSE_1_0.txt) [![Documentation](https://img.shields.io/badge/documentation-online-lightgrey.svg?style=flat)](https://boostgsoc17.github.io/static-views)



> Handy tools for manipulation of compile-time data.

StaticViews is a modern `C++14/17` library that lets you **easily manipulate
`constexpr` homogeneous sequences** using [Ranges
TS](https://github.com/ericniebler/range-v3) syntax. The main building block is
a *View*, a non-owning container with random access to elements. These views can
be used to access of modify your data, and can be used instead of iterators.
Views are also easily composable which allows you to **build `constexpr`
computational pipelines with a readable syntax**. On top of views, a
`static_map`, a **compile-time hash table**, is built. 


## Getting started

The library has three modes of operation:

* In tree build as part of Boost.
* Standalone.
* Standalone single header.

To try out the standalone single-header just download the
[`single-header/static_views.hpp`](https://github.com/BoostGSoC17/static-views/blob/development/single-header/static_views.hpp)
file:

```bash
wget "https://raw.githubusercontent.com/BoostGSoC17/static-views/development/single-header/static_views.hpp"
```

and add it to your include path. Have a look at
[`example/static_map.cpp`](https://github.com/BoostGSoC17/static-views/blob/development/example/static_map.cpp)
to get started. Full documentation is available
[here](https://boostgsoc17.github.io/static-views).


## Supported compilers

Strictly speaking, any compiler implementing `C++14` should do. Currently,
however, only Clang (4+) can compile Static_views. It seems that GCC has
trouble with EBO (Empty Base Optimisation) which the library uses extensively.
MSVC is a lost cause, I'm afraid.

## TODOs

I am planning to propose this library for addition to Boost. After the second
GSoC I think the implementation is more or less complete. There are probably
lots of bugs that still need to be fixed, and I hope that after a bit of
arguing GCC might agree to compile the code. The documentation still requires
quite a bit of work though (the tutorial is vastly outdated).

