[![Build Status](http://ci.eclipse.org/xacc/buildStatus/icon?job=xacc-ci)](http://ci.eclipse.org/xacc/job/xacc-ci/)

# Eclipse XACC 
## Language and Hardware Independent Quantum Programming Framework

XACC is a programming framework for extreme-scale, post-exascale accelerator architectures that integrates alongside existing conventional applications. It is a pluggable framework (built off the [CppMicroServices](https://github.com/cppmicroservices/cppmicroservices) native C++ OSGi implementation) for quantum programming languages and hardware developed for next-gen heterogeneous computing architectures. It lets computational scientists efficiently offload work to attached accelerators through user-friendly kernel definitions.

XACC currently supports quantum-classical programming and enables the execution of quantum kernels on IBM, Rigetti, and D-Wave QPUs.

Documentation
-------------

* [Website and Documentation ](https://xacc.readthedocs.io)

Quick Installation
------------------
XACC binaries are available via PyPi as Python Wheels: 
```bash
$ python -m pip install --user xacc
```
Install plugins to interact with various quantum programming languages and hardware:
```bash
$ python -m pip install --user xacc-rigetti xacc-vqe
```

Build from Source
-----------------
Ensure that you have installed CMake 3.2+, a C++11 compliant compiler, and OpenSSL development libraries (see [Pre-Requisites](http://xacc.readthedocs.io/en/latest/install.html#pre-requisites)).

Clone the repository recursively
```bash
$ git clone --recursive https://github.com/eclipse/xacc
```
Then configure with `cmake` and build with `make`
```bash
$ mkdir build && cd build
$ cmake .. 
$ make install
```
Your install will be in `$HOME/.xacc`. To change the install location, pass `-DCMAKE_INSTALL_PREFIX=/path/to/custom/install` to `cmake`. To build the optional Python API, pass `-DPYTHON_INCLUDE_DIR=/usr/include/python3.5` (or wherever your Python.h is located) to `cmake`. 

Questions, Bug Reporting, and Issue Tracking
--------------------------------------------

Questions, bug reporting and issue tracking are provided by GitHub. Please
report all bugs by creating a new issue with the bug tag. You can ask
questions by creating a new issue with the question tag.

License
-------

XACC is dual licensed - [Eclipse Public License](LICENSE.EPL) and [Eclipse Distribution License](LICENSE.EDL).
