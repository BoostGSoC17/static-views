# Boost.StaticViews [![Travis Status](https://travis-ci.org/BoostGSoC17/static-views.svg?branch=development)](https://travis-ci.org/BoostGSoC17/static-views) [![Appveyor status](https://ci.appveyor.com/api/projects/status/gydxsxfm0o5t311e/branch/development?svg=true)](https://ci.appveyor.com/project/twesterhout/static-views/branch/development) [![License](https://img.shields.io/badge/license-Boost-blue.svg?style=flat)](https://www.boost.org/LICENSE_1_0.txt) [![Documentation](https://img.shields.io/badge/documentation-online-lightgrey.svg?style=flat)](https://boostgsoc17.github.io/static-views)



> Handy tools for manipulation of compile-time data.

StaticViews is a modern `C++14/17` library that lets you **easily manipulate
`constexpr` homogeneous sequences** using [Ranges
TS](https://github.com/ericniebler/range-v3) syntax. The main building block is
a *View*, a non-owning container with random access to elements. These views can
be used to access of modify your data, and can be used instead of iterators.
Views are also easily composable which allows you to **build `constexpr`
computational pipelines with a readable syntax**. On top of views, a
`static_map`, a **compile-time hash table**, is built. Full documentation is
available [here](https://boostgsoc17.github.io/static-views).
