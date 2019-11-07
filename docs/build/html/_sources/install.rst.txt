Installation
============

Note that you must have a C++14 compliant compiler and a recent version of CMake (version 3.12+). We
recommend installing with the Python API (although you may choose not to). This discussion will
describe the build process with the Python API enabled. For this you will need a Python 3 executable and
development install. To interact with remote QPUs, you will need CURL with OpenSSL development
headers and libraries.

Ubuntu 16.04 Prerequisites
--------------------------
Here we will demonstrate installing from a bare Ubuntu install using GCC 8. We
install BLAS and LAPACK as well, which is required to build some optional simulators.
We install libunwind-dev which is also optional, but provides verbose stack-trace printing
upon execution error.

.. code:: bash

   $ sudo apt-get update && sudo apt-get install -y software-properties-common
   $ sudo add-apt-repository ppa:ubuntu-toolchain-r/test && sudo apt-get update
   $ sudo apt-get -y install gcc-8 g++-8 git libcurl4-openssl-dev python3 libunwind-dev \
            libpython3-dev python3-pip libblas-dev liblapack-dev
   $ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 50
   $ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 50

Ubuntu 18.04 Prerequisites
--------------------------
Here we will demonstrate installing from a bare Ubuntu install using GCC 7 (default on 18.04). We
install BLAS and LAPACK as well, which is required to build some optional simulators.
We install libunwind-dev which is also optional, but provides verbose stack-trace printing
upon execution error.

.. code:: bash

   $ sudo apt-get update
   $ sudo apt-get -y install gcc g++ git libcurl4-openssl-dev python3 libunwind-dev \
            libpython3-dev python3-pip libblas-dev liblapack-dev

Centos 7 Prerequisites
--------------------------
Here we will demonstrate installing from a bare Centos 7 install using GCC 8. We
install BLAS and LAPACK as well, which is required to build some optional simulators.

.. code:: bash

   $ sudo yum install libcurl-devel python3-devel git centos-release-scl make \
          devtoolset-8-gcc devtoolset-8-gcc-c++ blas-devel lapack-devel
   $ scl enable devtoolset-8 -- bash [ you might put this in your .bashrc ]

Fedora 30 Prerequisites
--------------------------
Here we will demonstrate installing from a bare Fedora 30 install using GCC 9. We
install BLAS and LAPACK as well, which is required to build some optional simulators.

.. code:: bash

   $ sudo dnf install python3-devel libcurl-devel git g++ gcc make blas-devel lapack-devel
   $ sudo python3 -m pip install cmake

Mac OS X Prerequisites
--------------------------
Ensure that you have XCode command utilities installed. A common issue seen is missing
standard includes like `wchar.h` and others. See `here <https://stackoverflow.com/a/52530212>`_
for proper XCode install and configuring to address these types of issues. Here we assume you
have Homebrew installed.

.. code:: bash

   $ brew install python3 openssl curl

Build XACC
----------
The best way to install a recent version of CMake is through Python Pip.

.. code:: bash

   $ sudo python3 -m pip install cmake

Now clone and build XACC

.. code:: bash

   $ git clone https://github.com/eclipse/xacc
   $ cd xacc && mkdir build && cd build
   [ note tests and examples are optional ]
   $ cmake .. -DXACC_BUILD_TESTS=TRUE -DXACC_BUILD_EXAMPLES=TRUE
   $ make -j$(nproc --all) install
   [ run tests with ]
   $ ctest --output-on-failure
   [ some examples executables are in build/quantum/examples ]
   $ quantum/examples/base_api/bell_quil_ibm_local

You can run Python examples as well

.. code:: bash

   [ you may also want to add this to your .bashrc ]
   $ export PYTHONPATH:$PYTHONPATH:$HOME/.xacc
   $ python3 ../python/examples/ddcl_example.py

Most users build and install the TNQVM Accelerator

.. code:: bash

   $ git clone https://github.com/ornl-qci/tnqvm
   $ cd tnqvm && mkdir build && cd build
   $ cmake .. -DXACC_DIR=$HOME/.xacc
   $ make -j$(nproc --all) install

