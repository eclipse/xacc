[![Build Status](https://jenkins.eclipse.org/xacc/buildStatus/icon?job=xacc-ci)](https://jenkins.eclipse.org/xacc/job/xacc-ci/)

# Eclipse XACC 
## Language and Hardware Independent Quantum Programming Framework
XACC is an extensible compilation framework for hybrid quantum-classical computing architectures. It provides extensible language frontend and hardware backend compilation components glued together via a novel quantum intermediate representation. XACC currently supports quantum-classical programming and enables the execution of quantum kernels on IBM, Rigetti, and D-Wave QPUs, as well as a number of quantum computer simulators.

Documentation
-------------

* [Website and Documentation ](https://xacc.readthedocs.io)

Quick Installation and Example Usage
-------------------------------------
XACC binaries are available via PyPi as Python Wheels: 
```bash
$ python -m pip install --user xacc
```
Install plugins to interact with various quantum programming languages and hardware:
```bash
$ python -m pip install --user xacc-rigetti
```

With XACC installed, one can compile and run quantum kernels in python and 
run on available QPUs and simulators:
```python
import xacc

# Initialize the framework
xacc.Initialize()

# Select the QPU you want to use and 
# allocate some qubits
qpu = xacc.getAccelerator('rigetti') # or ibm, tnqvm, etc..
qubits = qpu.createBuffer('q',2)

# Annotate functions that are to be 
# run on the QPU
@xacc.qpu(accelerator=qpu)
def entangle(buffer):
   H(0)
   CNOT(0, 1)
   Measure(0, 0)
   Measure(1, 1)
   
# Execute, gather results
entangle(qubits)
print(result.getMeasurementCounts())

# Cleanup
xacc.Finalize()
```
Build from Source
-----------------
Ensure that you have installed CMake 3.2+, a C++11 compliant compiler (GCC 5+, Clang 3.8+), and OpenSSL development libraries (see [prerequisites](http://xacc.readthedocs.io/en/latest/install.html#pre-requisites)).

Clone the repository recursively, configure with `cmake` and build with `make`
```bash
$ git clone --recursive https://github.com/eclipse/xacc
$ cd xacc && mkdir build && cd build
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
