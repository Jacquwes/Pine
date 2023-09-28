# Pine

[![CMake on multiple platforms](https://github.com/Jacquwes/Pine/actions/workflows/cmake.yml/badge.svg?branch=asio)](https://github.com/Jacquwes/Pine/actions/workflows/cmake.yml)

Pine is a modern and lightweight C++20 library for building chat applications.

It uses its own protocol, which is based on TCP using asio. Therefore, it is 
not suitable for use on the web.

It is multi-threaded and uses asynchronous I/O, and is designed to be
easily extensible.

## Building

_Build will not work if using clang_

It is recommended to use [vcpkg](https://github.com/microsoft/vcpkg) to install
dependencies. Pine uses CMake as its build system. To build Pine, run the
following commands:

Dependencies:

```bash
vcpkg install asio:x64-windows gtest:x64-windows
vcpkg install asio:x64-linux gtest:x64-linux
```

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path to vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build .
```
