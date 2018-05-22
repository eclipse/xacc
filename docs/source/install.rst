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

   $ (macosx) brew/port install openssl cmake
   $ (fedora) dnf install cmake 
   $ (ubuntu) apt-get install cmake libssl-dev

..  note::

   Installing OpenSSL from source is fairly simple

   .. code::

      $ wget https://www.openssl.org/source/openssl-1.0.2n.tar.gz
      $ tar -xvzf openssl-1.0.2n.tar.gz
      $ cd openssl-1.0.2n
      $ CFLAGS=-fPIC ./config shared (--prefix=/path/to/install if you want)
      $ make
      $ make install

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
   $ make install # can pass -jN for N = number of threads to use

This will build, test, and install XACC to ``$HOME/.xacc``. If you would 
like to install XACC somewhere else, replace the above ``cmake`` command with the following

.. code::

   $ cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/local/install

You can also build XACC with Python support: 

.. code:: bash

   $ export PY_INC_DIR=$(python -c "import sysconfig; print(sysconfig.get_paths()['platinclude'])")
   $ cmake .. -DPYTHON_INCLUDE_DIR=$PY_INC_DIR

This will install XACC headers and libraries, and an ``xacc-framework`` executable to ``$PREFIX/bin``.
You may want to set your PATH variable to include this bin directory:

.. code::

   $ export PATH=$HOME/.xacc/bin:$PATH

or wherever you installed XACC to (``$YOURINSTALLPATH/bin``).

Additionally, you may want to add these exports
to your home directory's ``.basrhc`` file (or equivalent).

Installing XACC Plugins
------------------------
If you have successfully built XACC (see above)
then you can now run

.. code::

   $ xacc-framework --help

This is a convenience python script to help download, build, and install
all currently supported XACC plugins. The execution syntax is as follows:

.. code::

   $ xacc-framework -p PLUGIN-NAME

You can also run this script with multiple plugin names.

.. code::

   $ xacc-framework -p PLUGIN1 PLUGIN2 PLUGIN3

and pass CMake arguments to this script. For example, if you
wanted to specify the argument ``CMAKEARG`` for a plugin, you could run the following

.. code::

   $ xacc-framework -p PLUGIN-NAME -a CMAKEARG=arg

You can also pass multiple CMake arguments at once.

