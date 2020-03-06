| Branch | Status |
|:-------|:-------|
|master | [![pipeline status](https://code.ornl.gov/qci/xacc/badges/master/pipeline.svg)](https://code.ornl.gov/qci/xacc/commits/master) |
|xacc-devel | [![pipeline status](https://code.ornl.gov/qci/xacc/badges/xacc-devel/pipeline.svg)](https://code.ornl.gov/qci/xacc/commits/xacc-devel) |

![alt text](https://raw.githubusercontent.com/eclipse/xacc/master/docs/assets/xacc-readme.jpg)

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
$ python3 -m xacc --benchmark python/benchmark/chemistry/benchmarks/tnqvm_nah_ucc1.ini 
```
All code is here and you can quickly start developing. We recommend 
turning on file auto-save by clicking ``File > Auto Save ``.
Note the Gitpod free account provides 100 hours of use for the month, so if 
you foresee needing more time, we recommend our nightly docker images.

The XACC nightly docker images also serve an Eclipse Theia IDE (the same IDE Gitpod uses) on port 3000. To get started, run 
```bash
$ docker run --security-opt seccomp=unconfined --init -it -p 3000:3000 xacc/xacc
```
Navigate to ``https://localhost:3000`` in your browser to open the IDE and get started with XACC. These are deployed 
nightly, with ``xacc-tnqvm-exatn`` (tensor network simulator) and ``xacc-quac`` (pulse-level simulation) variants.


Build from Source
-----------------
Full installation details can be followed [here](https://xacc.readthedocs.io/en/latest/install.html).

Ensure that you have installed CMake 3.12+, a C++14 compliant compiler (GCC 6+, Clang 3.4+), and
CURL development headers and libraries with OpenSSL support
(see [prerequisites](http://xacc.readthedocs.io/en/latest/install.html#pre-requisites)).

It is also recommended (though optional) that you install BLAS and LAPACK development libraries (for various simulators),
Python 3 development headers (for the Python API), and Libunwind (for stack trace printing).

To enable XACC Python support, ensure that `python3` is set to your desired version of Python 3. CMake will
find the corresponding development headers. Ensure that when you try to run XACC-enabled Python scripts
you are using the same `python3` executable that was set during your build.

On Mac OS X, we recommend our users install GCC 8 via Homebrew instead of relying
on XCode command line tools installation and the default Apple Clang compilers.
See [here](https://xacc.readthedocs.io/en/latest/install.html#mac-os-x) for more details on this.

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
