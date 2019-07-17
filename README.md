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

* [Website and Documentation ](https://xacc.readthedocs.io)

Build from Source
-----------------
Ensure that you have installed CMake 3.2+, a C++11 compliant compiler (GCC 5+, Clang 3.8+), and 
CURL development headers and libraries with OpenSSL support 
(see [prerequisites](http://xacc.readthedocs.io/en/latest/install.html#pre-requisites)). 

Optional dependencies include BLAS and LAPACK development libraries (for various simulators), 
Python 3 development headers and library (for the Python API), and Libunwind (for stack trace printing).

Clone the repository recursively, configure with `cmake` and build with `make`
```bash
$ git clone --recursive https://github.com/eclipse/xacc
$ cd xacc && mkdir build && cd build
[default] 
$ cmake .. 
[with python api] 
$ cmake .. -DPYTHON_INCLUDE_DIR=/path/to/python/headers
[with tests]
$ cmake .. -DXACC_BUILD_TESTS=TRUE 
[or any combination of the above]
$ cmake .. -DPYTHON_INCLUDE_DIR=/path/to/python/headers -DXACC_BUILD_TESTS=TRUE 
[now build xacc]
$ make install
[if you built with tests]
$ ctest
```
Your installation will be in `$HOME/.xacc`. If you built with the Python API, be sure to update your `PYTHONPATH` 
environment variable to point to the installation:
```bash
$ export PYTHONPATH=$PYTHONPATH:$HOME/.xacc
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
@article{xacc2018,
   title = "A language and hardware independent approach to quantum-classical computing",
   journal = "SoftwareX",
   volume = "7",
   pages = "245 - 254",
   year = "2018",
   issn = "2352-7110",
   doi = "https://doi.org/10.1016/j.softx.2018.07.007",
   url = "http://www.sciencedirect.com/science/article/pii/S2352711018300700",
   author = "A.J. McCaskey and E.F. Dumitrescu and D. Liakh and M. Chen and W. Feng and T.S. Humble",
   keywords = "Quantum computing, Quantum software"
}
```
