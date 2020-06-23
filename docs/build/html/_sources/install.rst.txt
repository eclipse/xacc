Installation
============

Note that you must have a C++14 compliant compiler and a recent version of CMake (version 3.12+). We
recommend installing with the Python API (although you may choose not to). This discussion will
describe the build process with the Python API enabled. For this you will need a Python 3 executable and
development install. To interact with remote QPUs, you will need CURL with OpenSSL development
headers and libraries.

Quick-Start with Docker
-----------------------
To get up and running quickly and avoid installing the prerequisites you can
pull the ``xacc/xacc`` Docker image
(see `here <https://xacc.readthedocs.io/en/latest/developers.html#quick-start-with-docker>`_ for instructions).
This image provides an Ubuntu 18.04 container that serves up an Eclipse Theia IDE. XACC is already
built and ready to go. Moreover, there are several variants: ``xacc/xacc-tnqvm-exatn``, and ``xacc/xacc-quac`` to
name a few. 

Prerequisites
-------------
Ubuntu 16.04
++++++++++++
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

Ubuntu 18.04
++++++++++++
Here we will demonstrate installing from a bare Ubuntu install using GCC 7 (default on 18.04). We
install BLAS and LAPACK as well, which is required to build some optional simulators.
We install libunwind-dev which is also optional, but provides verbose stack-trace printing
upon execution error.

.. code:: bash

   $ sudo apt-get update
   $ sudo apt-get -y install gcc g++ git libcurl4-openssl-dev python3 libunwind-dev \
            libpython3-dev python3-pip libblas-dev liblapack-dev

Centos 7
++++++++
Here we will demonstrate installing from a bare Centos 7 install using GCC 8. We
install BLAS and LAPACK as well, which is required to build some optional simulators.

.. code:: bash

   $ sudo yum install libcurl-devel python3-devel git centos-release-scl make \
          devtoolset-8-gcc devtoolset-8-gcc-c++ blas-devel lapack-devel
   $ scl enable devtoolset-8 -- bash [ you might put this in your .bashrc ]

Fedora 30
+++++++++
Here we will demonstrate installing from a bare Fedora 30 install using GCC 9. We
install BLAS and LAPACK as well, which is required to build some optional simulators.

.. code:: bash

   $ sudo dnf install python3-devel libcurl-devel git g++ gcc make blas-devel lapack-devel
   $ sudo python3 -m pip install cmake

Mac OS X
+++++++++
On Mac OS X, we recommend our users install GCC 8 via Homebrew instead of relying
on XCode command line tools installation and the default Apple Clang compilers

Too often we see our users on Mac with issues regarding missing standard includes like `wchar.h` and others.
This is ultimately due to an improper install of XCode (see `here <https://stackoverflow.com/a/52530212>`_).
If you do not see these issues with Apple Clang, feel free to use it, XACC will build fine. But if you
do see these issues, the easiest thing to do is to use Homebrew GCC 8.

.. code:: bash

   $ brew install gcc@8
   $ export CC=gcc-8
   $ export CXX=g++-8

Note these last exports are very important. You could also run CMake (see below) with these
variables set

.. code:: bash

   $ CC=gcc-8 CXX=g++-8 cmake ..

You will need to make sure to do this for all plugins / projects that build off of XACC.
You will see errors if you accidentally build other projects leveraging XACC (like tnqvm)
with compilers different than what was used to build XACC.

You will also need the following 3rd party dependencies

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
   $ export PYTHONPATH=$PYTHONPATH:$HOME/.xacc
   $ python3 ../python/examples/ddcl_example.py

Most users build and install the TNQVM Accelerator

.. code:: bash

   $ git clone https://github.com/ornl-qci/tnqvm
   $ cd tnqvm && mkdir build && cd build
   $ cmake .. -DXACC_DIR=$HOME/.xacc
   $ make -j$(nproc --all) install

