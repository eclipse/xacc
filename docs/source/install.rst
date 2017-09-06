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

.. code::

   $ cd xacc && mkdir build && cd build
   $ cmake ..
   $ make install # can pass -jN for N = number of threads to use

This will install XACC to /usr/local/xacc
(Pass -DCMAKE_INSTALL_PREFIX=$YOURINSTALLPATH to install it somewhere else).

Set your PATH variable to include the XACC bin directory:

.. code::

   $ export PATH=/usr/local/xacc/bin:$PATH

Additionally, you could add this to your home directory's .basrhc file (or equivalent).

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

Let's see how to use this script to install the
Rigetti, IBM, TNQVM, Scaffold, D-Wave, and Python plugins.

.. note::

   If you want support for the IBM, D-Wave, and Rigetti Accelerators, you must install
   `CppRestSDK <https://github.com/microsoft/cpprestsdk>`_ and OpenSSL. This
   is required for these Accelerators to make remote HTTP Rest calls to their
   respective server APIs. Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) brew install cpprestsdk
      $ (fedora) dnf install cpprest-devel openssl-devel
      $ (ubuntu) apt-get install libcpprest-dev libssl-dev


Rigetti Support
^^^^^^^^^^^^^^^^
The `Rigetti Plugin <https://github.com/ornl-qci/xacc-rigetti>`_ provides
support to XACC for compiling kernels writting in Quil, and executing programs
on the Rigetti QVM via a Rigetti Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-rigetti

You have now installed the Rigetti plugin. It is located in $XACC_ROOT/lib/plugins/accelerator
and $XACC_ROOT/lib/plugins/compilers, where XACC_ROOT is your XACC install prefix.

IBM Support
^^^^^^^^^^^^^
The `IBM Plugin <https://github.com/ornl-qci/xacc-ibm>`_ provides
support to XACC for executing programs
on the IBM Quantum Experience via the IBM Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-ibm

You have now installed the IBM plugin. It is located in $XACC_ROOT/lib/plugins/accelerator,
where XACC_ROOT is your XACC install prefix.


TNQVM
^^^^^^^
The `TNQVM Plugin <https://github.com/ornl-qci/tnqvm>`_ provides
support to XACC for executing programs
on the ORNL tensor network quantum virtual machine.

.. note::

   This Accelerator requires BLAS/LAPACK libraries to be installed.
   Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) should already be there in Accelerate Framework, if not
      $ (macosx) brew install openblas lapack
      $ (fedora) dnf install blas-devel lapack-devel
      $ (ubuntu) apt-get install libblas-dev liblapack-dev

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p tnqvm

You have now installed the TNQVM plugin. It is located in $XACC_ROOT/lib/plugins/accelerator,
where XACC_ROOT is your XACC install prefix.

Scaffold Support
^^^^^^^^^^^^^^^^^
.. note::

   Due to issues getting `ScaffCC <https://github.com/ornl-qci/ScaffCC>`_ to link correctly with RTTI on Mac OS X, we do not have a binary package installer for Mac OS X. We are open to PRs on this if you can help.

To use the `Scaffold Plugin <https://github.com/ornl-qci/tnqvm>`_ you must have our fork of
Scaffold installed as a binary package. We have builds for Fedora 25/26 and Ubuntu 16.04/17.04. To
install

.. code::

   $ (fedora) dnf install https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm (REPLACE 25 with 26 if on FC26)
   $ (ubuntu) wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb
   $ (ubuntu) apt-get install -y $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | sed "s/.*ends:\ //" | sed 's/,//g')
   $ (ubuntu) dpkg -i scaffold_2.0_amd64.deb

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-scaffold

You have now installed the Scaffold plugin. It is located in $XACC_ROOT/lib/plugins/compilers,
where XACC_ROOT is your XACC install prefix.

D-Wave Support
^^^^^^^^^^^^^^^
The `D-Wave Plugin <https://github.com/ornl-qci/xacc-dwave>`_ provides
support to XACC for executing programs
on the D-Wave QPU via the D-Wave Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-dwave

You have now installed the D-Wave plugin. It is located in $XACC_ROOT/lib/plugins/accelerator,
where XACC_ROOT is your XACC install prefix.

Furthermore, XACC has extensibility built in for minor graph embedding
algorithms. We currently have one supported embedding algorithm, a wrapper around
the D-Wave SAPI Cai, Macready, Roi algorithm. In order to install this as a plugin,
run the following

.. note::

   The following embedding algorithm needs to leverage the proprietary
   D-Wave SAPI header file and associated shared library: dwave_sapi.h and libdwave_sapi.so.
   In order for the installation below to work, place dwave_sapi.h in /usr/local/include/
   and libdwave_sapi.so in /usr/local/lib/

.. code::

   $ xacc-install-plugins.py -p xacc-dwave-sapi-embedding

You have now installed the D-Wave plugin. It is located in $XACC_ROOT/lib/plugins/accelerator
and $XACC_ROOT/lib/plugins/compilers, where XACC_ROOT is your XACC install prefix.

Python Bindings
^^^^^^^^^^^^^^^^
The `Python Plugin <https://github.com/ornl-qci/xacc-python>`_ provides
Python language bindings to XACC through the `pybind11 <https://github.com/pybind/pybind11>`_ project.

.. note::

   This plugin requires Python 2.7+ development headers/library.
   Here's how to install these on various popular platforms:

   .. code::

      $ (macosx) brew install python
      $ (fedora) dnf install python-devel
      $ (ubuntu) apt-get install python-dev

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-python

You have now installed the Python plugin. It is located in $XACC_ROOT/lib/python,
where XACC_ROOT is your XACC install prefix.

In order to use this installation, you must update your PYTHONPATH environment variable

.. code::

   $ export PYTHONPATH=$XACC_ROOT/lib/python:$PYTHONPATH

We recommend placing this command in your home directory's .bashrc file (or equivalent).

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

Now install the dependencies with your specified C++14 compiler (mine
will be gcc 7.2.0)

.. code::

   $ (with MPI support) spack install boost+mpi+graph ^mpich %gcc@7.2.0
   $ (without MPI support) spack install boost+graph %gcc@7.2.0

XACC has not yet been accepted into the Spack (we will soon issue a PR
to get it shipped as part of Spack). So in order to install it with Spack
we have to download our custom package recipe from the XACC repository:

.. code::

   $ cd $SPACK_ROOT/var/spack/repos/builtin/packages/ && mkdir xacc
   $ cd xacc && wget https://github.com/ORNL-QCI/xacc/raw/master/cmake/spack/xacc/package.py .

Now we can run

.. code::

   $ spack install xacc %gcc@7.2.0

Once all these are installed, load them as environment modules
so they are available for the XACC build:

.. code::

   $ spack load boost
