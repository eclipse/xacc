# XACC-Scaffold

XACC-Scaffold is a XACC and QCOR plugin meant to compile and run scaffold 
quantum code on a quantum processor while working in a classical environment. 

## Dependencies
Compiler (C++11): GNU 5+, Clang 8+ 
CMake 3.9+ 
XACC: see https://xacc.readthedocs.io/en/latest/install.html#building-xacc 
QCOR: see https://github.com/ORNL-QCI/qcor/ 
SCAFFOLD: see https://github.com/epiqc/ScaffCC

##Build instructions
For CMake 3.9+, do not use the apt-get installer, instead use `pip`, 
and ensure that `/usr/local/bin` is in your PATH:
```bash
$ python -m pip install --upgrade cmake
$ export PATH=$PATH:/usr/local/bin
```

On Ubuntu 16+, install latest clang and llvm libraries and headers (you may need sudo)
```bash
$ wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
$ echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" > /etc/apt/sources.list.d/llvm.list
$ apt-get update
$ apt-get install -y libclang-9-dev llvm-9-dev
$ ln -s /usr/bin/llvm-config-9 /usr/bin/llvm-config
```



Note that, for now, developers must clone XACC-Scaffold manually:
``` bash
$ git clone https://code.ornl.gov/qci/xacc-scaffold
$ cd xacc-scaffold
$ mkdir build && cd build
$ cmake .. -DXACC_DIR=~/.xacc (or wherever you installed XACC) -DSCAFFOLD_PATH = ~/home/project/scaffcc (Or wherever you installed scaffold)
$ make install
```
Update your PATH to ensure that the ```qcor``` compiler is available.
```bash
$ export PATH=$PATH:$HOME/.xacc/bin
```
## Example Usage

Here we demonstrate how to program, compile and run basic quantum scaffold code to make a bell state. 
Create the following file called bell_state.cpp

```cpp
#include qcor
  
      
int main(int argc, char** argv) {

  qcor::Initialize(argc, argv);

  auto buffer = std::make_shared<AcceleratorBuffer>(2);
  
  #pragma qcor scaffold(buffer, 2)
  {
    cbit out[2];
    H(buffer[0]);
    CNOT(buffer[0], buffer[1]);
    out[0] = MeasZ(buffer[0]);
    out[1] = MeasZ(buffer[1]);
  }


  buffer->print();
}
```
Top compile this with QCOR, run the following
```bash
$ qcor bell_state.cpp
```
Here we demonstrate how to program, compile and run quantum scaffold code with functions and modules. Create the following file
called scaff_function_example.cpp
```cpp
#include "qcor.hpp"


void biina (std::shared_ptr<xacc::AcceleratorBuffer> b) {
  #pragma qcor scaffold(b, 2)
  {

      module hi (qbit* b1) {
          H(b1[0]);
      }
      module hello(qbit* b1) {
          int i = 0;
          for (i = 0; i < 2; i++) {
              hi(b1);
          }
      }
    cbit out[2];
    hello(b);
    CNOT(b[0], b[1]);
    out[0] = MeasZ(b[0]);
    out[1] = MeasZ(b[1]);

  }
  b -> print();
}

int main(int argc, char** argv) {

  qcor::Initialize(argc, argv);

  auto buffer = std::make_shared<AcceleratorBuffer>(2);
  biina(buffer);


  buffer->print();
}
```
and run
```bash
$ qcor scaff_function_example.cpp
```

Here we demonstrate how to program, compile, and run a VQE problem with scaffold. Create the following file called vqe_example.cpp
```cpp
#include "qcor.hpp"

int main(int argc, char** argv) {

  qcor::Initialize(argc, argv);

  // Create a buffer of qubits
  auto buffer = xacc::qalloc(2);

  // allocate the required vqe parameters
  // initialized to 0.0
  std::vector<double> params(1);

  // Create an Optimizer
  auto optimizer = qcor::getOptimizer(
      "nlopt", {{"nlopt-optimizer", "cobyla"},
                {"nlopt-maxeval", 20}});

  // Create the Deuteron N=2 Hamiltonian
  // This will dictate measurements on our ansatz
  auto op = qcor::getObservable("pauli", "5.907 - 2.1433 X0X1 "
                                         "- 2.1433 Y0Y1"
                                         "+ .21829 Z0 - 6.125 Z1");

  // Create the ansatz with Scaffold code
  // Annotate to indicate we want to use this ansatz
  // for VQE, providing the qreg and its size, and
  // VQE-pertinent information
  #pragma qcor scaffold(buffer, 2) vqe(params, op, optimizer)
  {
    X(buffer[0]);
    Ry(buffer[1], params[0]);
    CNOT(buffer[1], buffer[0]);
  }

  // Print the results
  std::cout << "Optimal Energy = " << mpark::get<double>(buffer->getInformation("opt-val")) << "\n";

}
```
To compile this with QCOR, run the following
```bash
$ qcor vqe_example.cpp
```
