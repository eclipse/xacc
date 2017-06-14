---
layout: post
title: XACC Build Instructions
permalink: /build/xacc_build
category: build
---

# XACC Build 

This section provide guidelines for installing XACC and its TPLs.

## Third Party Dependencies

The following third party libraries (TPLs) are used by XACC:

| Packages               | Dependency | Version |
|------------------------|------------|---------|
| C++14 (GCC 6.1+)       | Required   | N/A     |
| CMake                  | Required   | 2.8+    |
| Boost                  | Required   | 1.59.0+ |
| spdlog                 | Required   | N/A     |
| MPI                    | Required   | N/A     |

# Fedora 25 Build Instructions

```bash
dnf install -y gcc-c++ cmake mpich-devel boost-mpich-devel make git spdlog environment-modules
git clone --recursive https://github.com/ORNL-QCI/xacc
module load mpi/mpich-x86_64

cd xacc && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/xacc -DCMAKE_BUILD_TYPE=Release
make -j4 install
ctest
```
