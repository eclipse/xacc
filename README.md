![XACC](docs/assets/xacc_full_logo.svg)


| Branch | Status |
|:-------|:-------|
|master | [![Linux CI](https://github.com/eclipse/xacc/actions/workflows/ci-linux.yml/badge.svg?branch=master)](https://github.com/eclipse/xacc/actions/workflows/ci-linux.yml) [![Mac CI](https://github.com/eclipse/xacc/actions/workflows/ci-mac.yml/badge.svg?branch=master)](https://github.com/eclipse/xacc/actions/workflows/ci-mac.yml) |
|devel | [![Linux CI](https://github.com/eclipse/xacc/actions/workflows/ci-linux.yml/badge.svg?branch=xacc-devel)](https://github.com/eclipse/xacc/actions/workflows/ci-linux.yml) [![Mac CI](https://github.com/eclipse/xacc/actions/workflows/ci-mac.yml/badge.svg?branch=xacc-devel)](https://github.com/eclipse/xacc/actions/workflows/ci-mac.yml)|


## Language and Hardware Independent Quantum Programming Framework
XACC is an extensible compilation framework for hybrid quantum-classical computing architectures.
It provides extensible language frontend and hardware backend compilation components glued together
via a novel quantum intermediate representation. XACC currently supports quantum-classical programming
and enables the execution of quantum kernels on IBM, Rigetti, and D-Wave QPUs, as well as a number
of quantum computer simulators.

Documentation
-------------

* [Website and Documentation](https://xacc.readthedocs.io)
* [API Documentation](https://ornl-qci.github.io/xacc-api-docs/)

Quick Start
-----------
Click [![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/eclipse/xacc) 
to open up a pre-configured Eclipse Theia IDE. You should immediately be able to 
run any of the C++ or Python examples from the included terminal:
```bash
[example C++ executables are in build/quantum/examples/*]
$ build/quantum/examples/qasm/deuteron_from_qasm

[example Python scripts are in python/examples/*]
$ python3 python/examples/deuteronH2.py

[run some XACC benchmarks]
$ python3 -m xacc --benchmark python/benchmark/chemistry/benchmarks/nah_ucc1.ini 
```
All code is here and you can quickly start developing. We recommend 
turning on file auto-save by clicking ``File > Auto Save ``.
Note the Gitpod free account provides 100 hours of use for the month, so if 
you foresee needing more time, we recommend our nightly docker images.

The XACC nightly docker images also serve an [Eclipse Theia IDE](https://theia-ide.org/) on port 3000. To get started, run 
```bash
$ docker run --security-opt seccomp=unconfined --init -it -p 3000:3000 ghcr.io/eclipse/xacc/xacc:latest
```
Navigate to ``https://localhost:3000`` in your browser to open the IDE and get started with XACC. 


Build from Source
-----------------
Full installation details can be followed [here](https://xacc.readthedocs.io/en/latest/install.html).

Ensure that you have installed CMake 3.12+, a C++17 compliant compiler (GCC 8+, Clang 5+), and
CURL development headers and libraries with OpenSSL support
(see [prerequisites](http://xacc.readthedocs.io/en/latest/install.html#pre-requisites)).

It is also recommended (though optional) that you install BLAS and LAPACK development libraries (for various simulators),
Python 3 development headers (for the Python API), and Libunwind (for stack trace printing).

To enable XACC Python support, ensure that `python3` is set to your desired version of Python 3. CMake will
find the corresponding development headers. Ensure that when you try to run XACC-enabled Python scripts
you are using the same `python3` executable that was set during your build.

Clone the repository recursively, configure with `cmake` and build with `make`
```bash
$ git clone https://github.com/eclipse/xacc
$ cd xacc && mkdir build && cd build
[default cmake call]
$ cmake ..
[with tests and examples]
$ cmake .. -DXACC_BUILD_EXAMPLES=TRUE -DXACC_BUILD_TESTS=TRUE
[now build xacc]
$ make install
[for a speedier build on linux]
$ make -j$(nproc --all) install
[and on mac os x]
$ make -j$(sysctl -n hw.physicalcpu) install
[if built with tests, run them]
$ ctest --output-on-failure
```
See full documentation for all CMake optional arguments.

Your installation will be in `$HOME/.xacc`. If you built with the Python API,
be sure to update your `PYTHONPATH` environment variable to point to the installation:
```bash
$ export PYTHONPATH=$PYTHONPATH:$HOME/.xacc
```

You will probably want the XACC default simulator, TNQVM. To install, run the following:
```bash
$ git clone https://github.com/ornl-qci/tnqvm
$ cd tnqvm && mkdir build && cd build
$ cmake .. -DXACC_DIR=~/.xacc
$ make install
```

Questions, Bug Reporting, and Issue Tracking
--------------------------------------------

Questions, bug reporting and issue tracking are provided by GitHub. Please
report all bugs by creating a new issue with the bug tag. You can ask
questions by creating a new issue with the question tag.

License
-------

XACC is dual licensed - [Eclipse Public License](LICENSE.EPL) and [Eclipse Distribution License](LICENSE.EDL).

Cite XACC
----------
If you use XACC in your research, please use the following citation
```
@article{xacc_2020,
	doi = {10.1088/2058-9565/ab6bf6},
	url = {https://doi.org/10.1088%2F2058-9565%2Fab6bf6},
	year = 2020,
	month = {feb},
	publisher = {{IOP} Publishing},
	volume = {5},
	number = {2},
	pages = {024002},
	author = {Alexander J McCaskey and Dmitry I Lyakh and Eugene F Dumitrescu and Sarah S Powers and Travis S Humble},
	title = {{XACC}: a system-level software infrastructure for heterogeneous quantum{\textendash}classical computing},
	journal = {Quantum Science and Technology}
}
```
