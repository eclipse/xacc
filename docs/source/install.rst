Installation
============

This section provide guidelines for installing XACC and its TPLs.

Pre-Requisites
--------------

The following software is required before building XACC: 

+------------------------+------------+-----------+
| Packages               | Dependency | Version   |
+========================+============+===========+
| C++11 Compiler         | Required   | See below |
+------------------------+------------+-----------+
| OpenSSL                | Required   |   1.0     |
+------------------------+------------+-----------+
| CMake                  | Required   |   3.2+    |
+------------------------+------------+-----------+

Note that you must have a C++11 compliant compiler. 
For GCC, this means version 4.8.1+, for Clang, this means 3.3+.

These dependencies are relatively easy to install on various operating
systems. Any of the following commands will work for Mac, Fedora/RedHat/CentOS, or Ubuntu:

.. code::

   $ (macosx) brew/port install cmake openssl
   $ (fedora) dnf install cmake openssl-devel
   $ (ubuntu) apt-get install cmake libssl-dev

Building XACC
-------------

Clone the XACC repository:

.. code::

   $ git clone --recursive https://github.com/ornl-qci/xacc

XACC requires CMake 3.2+ to build. Run the following to
configure and build XACC:

.. code:: bash

   $ cd xacc && mkdir build && cd build
   $ cmake ..
   $ make install 

This will build and install XACC to ``$HOME/.xacc`` by default. If you would 
like to install XACC somewhere else, replace the above ``cmake`` command with the following

.. code::

   $ cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/local/install

You can also build XACC with Python support: 

.. code:: bash

   $ export PY_INC_DIR=$(python -c "import sysconfig; print(sysconfig.get_paths()['platinclude'])")
   $ cmake .. -DPYTHON_INCLUDE_DIR=$PY_INC_DIR
   $ make install

To build with tests

.. code:: bash

   $ cmake .. -DXACC_BUILD_TESTS=TRUE
   $ make install
   $ ctest

Keeping for Reference
---------------------
..  note::

   Installing OpenSSL from source is fairly simple:

   .. code::

      $ wget https://www.openssl.org/source/openssl-1.0.2n.tar.gz
      $ tar -xvzf openssl-1.0.2n.tar.gz
      $ cd openssl-1.0.2n
      $ CFLAGS=-fPIC ./config shared (--prefix=/path/to/install if you want)
      $ make
      $ make install

