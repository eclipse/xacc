Installation
============

This section provide guidelines for installing XACC and its TPLs.

Install third-party libraries
-----------------------------

The following third party libraries (TPLs) are used by XACC:

+------------------------+------------+-----------+
| Packages               | Dependency | Version   |
+========================+============+===========+
| C++14 Compiler         | Required   | See below |
+------------------------+------------+-----------+
| Boost                  | Required   | 1.59.0+   |
+------------------------+------------+-----------+
| MPI                    | Optional   | N/A       |
+------------------------+------------+-----------+

Note that you must have a C++14 compliant compiler. 
For GCC, this means version 6.1+, for Clang, this means 3.4+.

These dependencies are relatively easy to install on popular operating
systems. Any of the following commands will work (showing with and without MPI):

.. code::

   $ (macosx) brew install boost
   $ (macosx) brew install boost-mpi
   $ (fedora) dnf install boost-devel
   $ (fedora) dnf install boost-mpich-devel
   $ (fedora) dnf install boost-openmpi-devel
   $ (ubuntu) apt-get install libboost-all-dev # will install openmpi

Build XACC
-----------

Clone the XACC repository:

.. code::

   $ git clone https://github.com/ornl-qci/xacc

XACC requires CMake 3.2+ to build. Run the following to
configure and build XACC:

.. code:: bash

   $ cd xacc && mkdir build && cd build
   $ cmake ..
   $ make install # can pass -jN for N = number of threads to use

This will build, test, and install XACC to ``/usr/local/xacc``
(Pass ``-DCMAKE_INSTALL_PREFIX=$YOURINSTALLPATH`` to install it somewhere else).

Set your PATH variable to include the XACC bin directory:

.. code::

   $ export PATH=/usr/local/xacc/bin:$PATH

Additionally, you could add this to your home directory's ``.basrhc`` file (or equivalent).

Installing XACC Plugins
-----------------------------------
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

You can also pass CMake arguments to this script. For example, if you
wanted to specify the path to your OpenSSL install for a plugin
that depended on OpenSSL, you could run the following

.. code::

   $ xacc-install-plugins.py -p PLUGIN-NAME -a OPENSSL_ROOT_DIR=/usr/local/opt/openssl

You can pass multiple CMake arguments at once.

XACC and Spack
---------------
You can build XACC and its dependencies with the `Spack
<https://github.com/llnl/spack>`_ package manager.

To configure your available system compilers run

.. code::

   $ spack compilers

.. note::

   If you run 'spack config get compilers' and your desired
   compiler has fc and f77 set to Null or None, then the
   install will not work if you are including MPI support.
   If this is the case, it usually
   works to run 'spack config edit compilers' and
   manually replace Null with /path/to/your/gfortran

We will rely on the environment-modules package to load/unload
installed Spack modules. If you don't have this installed
(you can check by running 'module avail') install with

.. code::

   $ spack install environment-modules

Add the following to your ~/.bashrc (or equivalent)

.. code::

   . $SPACK_ROOT/share/spack/setup-env.sh
   source $(spack location -i environment-modules)/Modules/init/bash

If you do not have a C++14 compliant compiler, you can
install one with Spack, for example

.. code::

   $ spack install gcc@7.2.0 # this will take awhile...
   $ spack load gcc
   $ spack compiler find

XACC has not yet been accepted into the Spack (we will soon issue a PR
to get it shipped as part of Spack). So in order to install it with Spack
we have to download our custom package recipe from the XACC repository:

.. code::

   $ cd $SPACK_ROOT/var/spack/repos/builtin/packages/ && mkdir xacc
   $ cd xacc && wget https://github.com/ORNL-QCI/xacc/raw/master/cmake/spack/xacc/package.py .

Now we can run 

.. code::

   $ (without MPI support) spack install xacc 
   $ (with MPI support) spack install xacc +mpi 
   $ (with specified compiler) spack install xacc %gcc@7.2.0

Update your PATH to point to the XACC ``bin`` directory: 

.. code::

   $ export PATH=$(spack location -i xacc)/bin:$PATH

We recommend you add this command to you ``.bashrc`` file (or equivalent).
