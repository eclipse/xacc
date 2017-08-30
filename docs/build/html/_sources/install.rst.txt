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
| CppMicroServices       | Required   | 3.1       |
+------------------------+------------+-----------+
| CppRestSDK             | Optional   | 2.9+      |
+------------------------+------------+-----------+
| OpenSSL                | Optional   | 1.0+      |
+------------------------+------------+-----------+
| MPI                    | Optional   | N/A       |
+------------------------+------------+-----------+
| Python-Devel           | Optional   | 2.7       |
+------------------------+------------+-----------+

Note that you must have a C++14 compliant compiler. 
For GCC, this means version 6.1+, for Clang, this means 3.4+.

Also note that CppRestSDK is optional. XACC uses this 
library for remote interacting with remotely hosted 
quantum resources. If you choose not to include this 
library in your XACC build, you will not have access 
to the Rigetti, D-Wave, or IBM Accelerators. If you 
choose to include CppRestSDK you must also install OpenSSL.

To build the Python XACC API, you must also have 
installed the python development library and headers.

You can build these dependencies with the `Spack
<https://github.com/llnl/spack>`_ package manager. Configure 
your available system compilers by running 

.. code::

   $ spack compilers
.. note::

   If you run 'spack config get compilers' and your desired 
   compiler has fc and f77 set to Null or None, then the 
   install will not work. If this is the case, it usually 
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

   $ spack install boost+mpi+graph ^mpich %gcc@7.2.0
   $ spack install python %gcc@7.2.0
   $ spack install cpprestsdk %gcc@7.2.0
   
CppMicroServices has not yet been accepted into the Spack (we will soon issue a PR 
to get it shipped as part of Spack). So in order to install it with Spack 
we have to download our custom package recipe from the XACC repository:

.. code::

   $ cd $SPACK_ROOT/var/spack/repos/builtin/packages/ && mkdir cppmicroservices
   $ cd cppmicroservices && wget https://github.com/ORNL-QCI/xacc/raw/master/cmake/spack/cppmicroservices/package.py .

Now we can run 

.. code::

   $ spack install cppmicroservices %gcc@7.2.0

Once all these are installed, load them as environment modules 
so they are available for the XACC build:

.. code::

   $ spack load cppmicroservices
   $ spack load boost
   $ spack load python
   $ spack load cpprestsdk

Build XACC
-----------

Clone the XACC repository:

.. code::

   $ git clone https://github.com/ornl-qci/xacc

XACC requires CMake 2.8+ to build. Run the following to 
configure and build XACC:

.. code::

   $ cd xacc && mkdir build && cd build
   $ cmake ..
   $ make install # can pass -jN for N = number of threads to use
   $ ctest

This will install XACC to /usr/local/xacc. Since we built with 
CppRestSDK/OpenSSL, we have the Rigetti and D-Wave Accelerators 
installed. Since we build with Python, we also have 
/usr/local/xacc/lib/python/pyxacc.so which contains the 
XACC Python API. 
