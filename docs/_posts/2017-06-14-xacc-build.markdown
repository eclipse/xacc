---
layout: post
title: XACC Build Instructions
permalink: /build/xacc_build
category: build
---

# XACC Build from Source

This section provide guidelines for installing XACC and its TPLs. We will show 
how to build XACC on a Fedora/CentOS/Redhat OS, and Ubuntu OS, and from 
scratch on any other Unix OS. 

## Third Party Dependencies

The following third party libraries (TPLs) are used by XACC:

| Packages              | Dependency | Version   |
|-----------------------|------------|-----------|
| C++14                 | Required   | See below |
| CMake                 | Required   | 2.8+      |
| Boost                 | Required   | 1.59.0+   |
| CppMicroService       | Required   | Master    |
| CppRestSDK            | Optional   | 2.9       |
| OpenSSL               | Optional   | 1.0+      |
| MPI                   | Optional   | N/A       |
| Python-Devel          | Optional   | 2.7+      |

Note that you must have a C++14 compliant compiler. For GCC, this means 
version 6.1+, for Clang, this means 3.4+. 

Also note that CppRestSDK is optional. XACC uses this library for 
remote interacting with remotely hosted quantum resources. If you choose 
not to include this library in your XACC build, you will not have 
access to the Rigetti, D-Wave, or IBM Accelerators. If you choose 
to include CppRestSDK you must also install OpenSSL. 

To build the Python XACC API, you must also have installed the python 
development library and headers. 

## Spack Installation



## Fedora/CentOS/Redhat Build Instructions

Here we will build XACC with CppRestSDK, OpenSSL, MPI, and Python. You 
can remove any of these and the build will still work. The following 
has been tested in a Fedora 26 Docker container. 

```bash
#Install 3rd party tools/libraries
$ dnf install -y gcc-c++ cmake mpich-devel boost-mpich-devel \
                 make git environment-modules cpprest-devel openssl-devel

# Install Scaffold support (Required as of 2017-06-14)
$ dnf install https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc26.x86_64.rpm

# Clone CppMicroServices
$ git clone https://github.com/cppmicroservices/cppmicroservices
$ cd cppmicroservices && mkdir build && cd build
$ cmake .. -DCMAKE_CXX_FLAGS='-Wimplicit-fallthrough=0' && make install
$ cd ../..

# Clone XACC
$ git clone https://github.com/ORNL-QCI/xacc

# Load MPI Environment Variables
$ module load mpi/mpich-x86_64

# Build XACC
$ cd xacc && mkdir build && cd build
$ cmake .. 
$ make -j4 install
$ ctest
```

## Ubuntu 16.04 Build Instructions

Here we will build XACC with CppRestSDK, OpenSSL, MPI, and Python. You 
can remove any of these and the build will still work. The following 
has been tested in an Ubuntu 16.04 Docker container. 

```bash
# Install 3rd party tools/libraries and GCC 6+
$ apt-get install -y software-properties-common
$ add-apt-repository ppa:ubuntu-toolchain-r/test
$ apt-get -y update && apt-get -y install libboost-all-dev git make libtool cmake gcc-6 g++-6 mpich python-dev libssl-dev libcpprest-dev
$ update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6

# Install Scaffold support (Required as of (2017-06-14))
$ wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb 
$ apt-get -y update && apt-get -y install $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | sed "s/.*ends: //" | sed 's/,//g') && dpkg -i scaffold_2.0_amd64.deb

# Clone CppMicroServices
$ git clone https://github.com/cppmicroservices/cppmicroservices
$ cd cppmicroservices && mkdir build && cd build
$ cmake .. && make install
$ cd ../..

# Build XACC
$ git clone https://github.com/ORNL-QCI/xacc
$ cd xacc && mkdir build && cd build
$ cmake .. 
$ make -j4 install
$ ctest
```
