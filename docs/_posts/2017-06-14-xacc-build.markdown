---
layout: post
title: XACC Build Instructions
permalink: /build/xacc_build
category: build
---

# XACC Build from Source

This section provide guidelines for installing XACC and its TPLs.

## Third Party Dependencies

The following third party libraries (TPLs) are used by XACC:

| Packages               | Dependency | Version |
|------------------------|------------|---------|
| C++14 (GCC 6.1+)       | Required   | N/A     |
| CMake                  | Required   | 2.8+    |
| Boost                  | Required   | 1.59.0+ |
| CppMicroServices       | Required   | Master  |
| spdlog                 | Required   | N/A     |
| MPI                    | Required   | N/A     |

## Fedora/CentOS/Redhat Build Instructions

```bash
#Install 3rd party tools/libraries
$ dnf install -y gcc-c++ cmake mpich-devel boost-mpich-devel make git spdlog environment-modules

# Install Scaffold support (Required as of 2017-06-14)
$ dnf install https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm

# Clone CppMicroServices
$ git clone https://github.com/cppmicroservices/cppmicroservices
$ cd cppmicroservices && mkdir build && cd build
$ cmake .. && make install
$ cd ../..

# Clone XACC
$ git clone --recursive https://github.com/ORNL-QCI/xacc

# Load MPI Environment Variables
$ module load mpi/mpich-x86_64

# Build XACC
$ cd xacc && mkdir build && cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/xacc -DCMAKE_BUILD_TYPE=Release
$ make -j4 install
$ ctest
```

## Ubuntu 16.04 Build Instructions

```bash
# Install 3rd party tools/libraries and GCC 6+
$ apt-get -y upgrade && apt-get -y update && apt-get install -y software-properties-common
$ add-apt-repository ppa:ubuntu-toolchain-r/test
$ apt-get -y update && apt-get -y install libspdlog-dev libboost-all-dev git make libtool cmake gcc-6 g++-6 mpich wget
$ update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6

# Install Scaffold support (Required as of (2017-06-14)
$ wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb 
$ apt-get -y update && apt-get -y install $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | sed "s/.*ends: //" | sed 's/,//g') && dpkg -i scaffold_2.0_amd64.deb

# Clone CppMicroServices
$ git clone https://github.com/cppmicroservices/cppmicroservices
$ cd cppmicroservices && mkdir build && cd build
$ cmake .. && make install
$ cd ../..

# Build XACC
$ git clone --recursive https://github.com/ORNL-QCI/xacc
$ cd xacc && mkdir build && cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/xacc -DCMAKE_BUILD_TYPE=Release
$ make -j4 install
$ ctest
```
