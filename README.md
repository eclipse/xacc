[![Build Status](http://ci.eclipse.org/xacc/buildStatus/icon?job=xacc-ci)](http://ci.eclipse.org/xacc/job/xacc-ci/)

# Eclipse XACC 
## Hardware Agnostic Quantum Programming 

XACC is a programming framework for extreme-scale, post-exascale accelerator architectures that integrates alongside existing conventional applications. It is a pluggable framework for programming languages and hardware developed for next-gen computing hardware architectures like quantum and neuromorphic computing. It lets computational scientists efficiently offload work to attached accelerators through user-friendly Kernel definitions. XACC makes post-exascale hybrid programming approachable for domain computational scientists.

XACC currently supports hybrid classical-quantum programming and enables the execution of quantum kernels on IBM, Rigetti, and D-Wave QPUs.

Documentation
-------------

* [Website and Documentation ](https://xacc.readthedocs.io)

Quick Installation
------------------
XACC binaries are available via PyPi as Python Wheels: 
```bash
$ python -m pip install --user xacc
```

Build from Source
-----------------
Ensure that you have installed CMake 3.2+, a C++11 compliant compiler, and OpenSSL development libraries (see [Pre-Requisites](http://xacc.readthedocs.io/en/latest/install.html#pre-requisites)).

Clone the repository recursively
```bash
$ git clone --recursive https://github.com/eclipse/xacc
```
Then configure with `cmake` (optionally passing your Python development header path to build the XACC Python API) and build with `make`
```bash
$ mkdir build && cd build
$ export PY_INC_DIR=$(python -c "import sysconfig; print(sysconfig.get_paths()['platinclude'])")
$ cmake .. -DPYTHON_INCLUDE_DIR=$PY_INC_DIR
$ make install
```
Your install will be in `$HOME/.xacc`. To change the install location, pass `-DCMAKE_INSTALL_PREFIX=/path/to/custom/install`.

Questions, Bug Reporting, and Issue Tracking
--------------------------------------------

Questions, bug reporting and issue tracking are provided by GitHub. Please
report all bugs by creating a new issue with the bug tag. You can ask
questions by creating a new issue with the question tag.

License
-------

XACC is dual licensed - [Eclipse Public License](LICENSE.EPL) and [Eclipse Distribution License](LICENSE.EDL).
