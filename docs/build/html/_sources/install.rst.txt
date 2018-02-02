Installation
============

This section provide guidelines for installing XACC and its TPLs.

Install third-party libraries
-----------------------------

The following third party libraries (TPLs) are used by XACC:

+------------------------+------------+-----------+
| Packages               | Dependency | Version   |
+========================+============+===========+
| C++11 Compiler         | Required   | See below |
+------------------------+------------+-----------+
| Boost                  | Required   | 1.59.0+   |
+------------------------+------------+-----------+
| OpenSSL                | Required   | 1.0.2     |
+------------------------+------------+-----------+
| CMake                  | Required   | 3.2+      |
+------------------------+------------+-----------+

Note that you must have a C++11 compliant compiler. 
For GCC, this means version 4.8.1+, for Clang, this means 3.3+.

These dependencies are relatively easy to install on various operating
systems. Any of the following commands will work for Mac, Fedora/RedHat/CentOS, or Ubuntu:

.. code::

   $ (macosx) brew/port install boost openssl
   $ (fedora) dnf install boost-devel
   $ (ubuntu) apt-get install libboost-all-dev

..  note::

   On Fedora, the latest version of OpenSSL available via DNF is 1.1.1. This
   version of OpenSSL is not compatible with CppRestSDK, a library that XACC
   leverages to connect to remote Accelerators. To install OpenSSL from source

   .. code::

      $ wget https://www.openssl.org/source/openssl-1.0.2n.tar.gz
      $ tar -xvzf openssl-1.0.2n.tar.gz
      $ cd openssl-1.0.2h
      $ CFLAGS=-fPIC ./config shared (--prefix=/path/to/install if you want)
      $ make
      $ make install

Building XACC
-------------

Clone the XACC repository:

.. code::

   $ git clone https://github.com/ornl-qci/xacc

XACC requires CMake 3.2+ to build. Run the following to
configure and build XACC:

.. code:: bash

   $ cd xacc && mkdir build && cd build
   $ cmake ..
   $ make install # can pass -jN for N = number of threads to use

This will build, test, and install XACC to ``/usr/local/xacc``. If you don't have
root access, and therefore can't write to ``/usr/local/xacc``, or would like to just
install XACC somewhere else, replace the above ``cmake`` command with the following

.. code::

   $ cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/local/install

After running CMake, you should see that OpenSSL was found. If there is
a problem with finding OpenSSL 1.0.2, you can point XACC to OpenSSL with

.. code:: bash

   $ cmake .. -DOPENSSL_ROOT_DIR=/path/to/openssl

The CMake configure will also look for Python development libraries on your
system, and if found, will generate the `XACC Python Bindings <python.html>`_ bindings.
If XACC does not find your correct Python installation, you can point to it with

.. code:: bash

   $ cmake .. -DPYTHON_EXECUTABLE=/path/to/python

At this point, if you like, you can setup your Python environment
to automatically load the XACC Python bindings with

.. code:: bash

   $ export PYTHONPATH=$PYTHONPATH:/usr/local/xacc/lib/python

or ``$YOURINSTALLPATH/lib/python``.

You may also set your PATH variable to include the XACC bin directory:

.. code::

   $ export PATH=/usr/local/xacc/bin:$PATH

or wherever you installed XACC to (``$YOURINSTALLPATH/bin``).

Additionally, you may want to add these exports
to your home directory's ``.basrhc`` file (or equivalent).

Installing XACC Plugins
------------------------
If you have successfully built XACC (see above)
then you can now run

.. code::

   $ xacc-install-plugins.py --help

This is a convenience python script to help download, build, and install
all currently supported XACC plugins. The execution syntax is as follows:

.. code::

   $ xacc-install-plugins.py -p PLUGIN-NAME

You can also run this script with multiple plugin names.

.. code::

   $ xacc-install-plugins.py -p PLUGIN1 PLUGIN2 PLUGIN3

and pass CMake arguments to this script. For example, if you
wanted to specify the argument ``CMAKEARG`` for a plugin, you could run the following

.. code::

   $ xacc-install-plugins.py -p PLUGIN-NAME -a CMAKEARG=arg

You can also pass multiple CMake arguments at once.

