---
layout: post
title: XACC Rigetti Accelerator Tutorial
permalink: /tutorials/rigetti
category: tutorials
---

## Using XACC to Execute Quantum Programs on the Rigetti QPU

XACC can now execute quantum kernels on the Rigetti QVM. To learn more about 
the architecture of this support, check out 
[(RigettiAccelerator)](https://ornl-qci.github.io/xacc/accelerators/rigetti).

This post provides a tutorial on how to actually use this support. 

# Prerequisites

To run this tutorial, you need to install OpenSSL on your machine. To do so 
on Fedora: 

```bash
dnf install -y openssl openssl-devel
```

On Ubuntu: 

```bash
apt-get install -y libssl-dev
```

Furthermore, to execute on the Rigetti QVM, you must have been provided 
a valid Rigetti API key. Check out [Forest](forest.rigetti.com) to learn how 
to get this key. 

# XACC-Rigetti Quantum Teleportation 

To install XACC, run the following (note here we are using a Fedora 25 Linux install):

```bash
dnf install -y openssl-devel https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm \
    http://eclipseice.ornl.gov/downloads/xacc/rpms/x86_64/xacc-1.0-1.fc25.x86_64.rpm
```
Note that we are also installing the Scaffold quantum programming language. XACC has support 
for a Scaffold Compiler instance, and we will use it in this tutorial as our main 
quantum kernel programming language. 

Create a new directory called test-xacc-rigetti and cd into it. Let's now create a 
test-xacc-rigetti.cpp file and get it started with the following boilerplate code: 

```cpp
#include "XACC.hpp"

int main(int argc, char** argv) {

   // Initialize XACC - find all available 
   // compilers and accelerators, parse command line.
   xacc::Initialize(argc, argv);

   // ... Code to come ...

   // Finalize the framework. 
   xacc::Finalize();
}
```

Building this code is straightforward with CMake. Create a CMakeLists.txt file in the sam e
directory as the test-xacc-rigetti.cpp file, and add the following to it: 

```bash
# Start a CMake project
project(test-xacc-rigetti CXX)

# Set the minimum version to 2.8
cmake_minimum_required(VERSION 2.8)

# We require the Boost system, program options, and fs libraries
find_package(Boost COMPONENTS system program_options filesystem REQUIRED)

# Find XACC
find_package(XACC REQUIRED)

# Include all XACC Include Directories
include_directories(${XACC_INCLUDE_DIRS})

# Link to hte XACC Library Directory,
# this is where Scaffold, Rigetti, SimpleAcc
# plugins reside
link_directories(${XACC_LIBRARY_DIR})

# Create the executabe
add_executable(test-xacc-rigetti test-xacc-rigetti.cpp)

# Like the necessary libaries
target_link_libraries(test-xacc-rigetti ${Boost_LIBRARIES} dl pthread)
```

Now from within the test-xacc-rigetti directory, run the following: 

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

This will build test-xacc-rigetti.cpp and provide you with a test-xacc-rigetti executable. Run that 
executable to ensure that your build worked (you should see the following output):
```bash
$ make
$ ./test-xacc-rigetti
[2017-06-20 16:14:07.076] [xacc-console] [info] [xacc] Initializing XACC Framework
[2017-06-20 16:14:07.091] [xacc-console] [info] [xacc::compiler] XACC has 3 Compilers available.
[2017-06-20 16:14:07.091] [xacc-console] [info] [xacc::accelerator] XACC has 2 Accelerators available.
[2017-06-20 16:14:07.091] [xacc-console] [info]
[xacc] XACC Finalizing
[xacc::compiler] Cleaning up Compiler Registry.
[xacc::accelerator] Cleaning up Accelerator Registry.
```
Now that we have our build and initial boilerplate code setup, let's actually write some quantum code, specifically 
teleporting the state of one qubit to another. Following the XACC.hpp include statement at the top of the file, 
add the following quantum kernel declaration: 

```cpp
const std::string src("__qpu__ teleport (qbit qreg) {\n"
"   cbit creg[3];\n"
"   // Init qubit 0 to 1\n"
"   X(qreg[0]);\n"
"   // Now teleport...\n"
"   H(qreg[1]);\n"
"   CNOT(qreg[1],qreg[2]);\n"
"   CNOT(qreg[0],qreg[1]);\n"
"   H(qreg[0]);\n"
"   creg[0] = MeasZ(qreg[0]);\n"
"   creg[1] = MeasZ(qreg[1]);\n"
"   if (creg[0] == 1) Z(qreg[2]);\n"
"   if (creg[1] == 1) X(qreg[2]);\n"
"   // Check that 3rd qubit is a 1\n"
"   creg[2] = MeasZ(qreg[2]);\n"
"}\n");
```
Now we are ready to build and execute this kernel using the XACC Runtime API. After the call 
to xacc::Initialize, add the following: 

```cpp
// Create a reference to the Rigetti 
// QPU at api.rigetti.com/qvm
auto qpu = xacc::getAccelerator("rigetti");

// Allocate a register of 3 qubits
auto qubitReg = qpu->createBuffer("qreg", 3);

// Create a Program
xacc::Program program(qpu, src);

// Request the quantum kernel representing
// the above source code
auto teleport = program.getKernel("teleport");

// Execute!
teleport(qubitReg);
```

The code above starts by getting a reference to the RigettiAccelerator (see [RigettiAccelerator](https://ornl-qci.github.io/xacc/accelerators/rigetti)). With that reference, we then allocate a register of qubits 
to operate the teleport kernel on. Next, we instantiate an XACC Program instance, which keeps track of the 
desired Accelerator and the source code to be compiled. The Program instance orchestrates the compilation of the 
quantum kernel to produce the XACC intermediate representation, and then handles the creation of an 
executable classical kernel function that offloads the compiled quantum code to the specified Accelerator. 
Finally, the user requests a reference to the executable kernel functor, and executes it on the 
provided register of qubits. 

The total test-xacc-rigetti.cpp file should look like this: 
```cpp
#include "XACC.hpp"

// Quantum Kernel executing teleportation of
// qubit state to another.
const std::string src("__qpu__ teleport (qbit qreg) {\n"
   "   cbit creg[3];\n"
   "   // Init qubit 0 to 1\n"
   "   X(qreg[0]);\n"
   "   // Now teleport...\n"
   "   H(qreg[1]);\n"
   "   CNOT(qreg[1],qreg[2]);\n"
   "   CNOT(qreg[0],qreg[1]);\n"
   "   H(qreg[0]);\n"
   "   creg[0] = MeasZ(qreg[0]);\n"
   "   creg[1] = MeasZ(qreg[1]);\n"
   "   if (creg[0] == 1) Z(qreg[2]);\n"
   "   if (creg[1] == 1) X(qreg[2]);\n"
   "   // Check that 3rd qubit is a 1\n"
   "   creg[2] = MeasZ(qreg[2]);\n"
"}\n");

int main (int argc, char** argv) {

   // Initialize the XACC Framework
   xacc::Initialize(argc, argv);

   // Create a reference to the Rigetti 
   // QPU at api.rigetti.com/qvm
   auto qpu = xacc::getAccelerator("rigetti");

   // Allocate a register of 3 qubits
   auto qubitReg = qpu->createBuffer("qreg", 3);

   // Create a Program
   xacc::Program program(qpu, src);

   // Request the quantum kernel representing
   // the above source code
   auto teleport = program.getKernel("teleport");

   // Execute!
   teleport(qubitReg);

   // Finalize the XACC Framework
   xacc::Finalize();

   return 0;
}
```

Now, to build simple run: 
```bash
$ cd test-xacc-rigetti/build
$ make
```

To execute this code on the Rigetti QVM, you must provide your API key. You can do this 
the same way you do with PyQuil (in your $HOME/.pyquil_config file, or in the $PYQUIL_CONFIG 
environment variable). You can also pass your API key to the XACC executable through the 
--rigetti-api-key command line argument: 

```bash
$ ./test-xacc-rigetti --rigetti-api-key KEY
[2017-06-20 17:43:38.744] [xacc-console] [info] [xacc] Initializing XACC Framework
[2017-06-20 17:43:38.760] [xacc-console] [info] [xacc::compiler] XACC has 3 Compilers available.
[2017-06-20 17:43:38.760] [xacc-console] [info] [xacc::accelerator] XACC has 2 Accelerators available.
[2017-06-20 17:43:38.766] [xacc-console] [info] Executing Scaffold compiler.
[2017-06-20 17:43:38.770] [xacc-console] [info] Rigetti Json Payload = { "type" : "multishot", "addresses" : [0, 1, 2], "quil-instructions" : "X 0\nH 1\nCNOT 1 2\nCNOT 0 1\nH 0\nMEASURE 0 [0]\nMEASURE 1 [1]\nJUMP-UNLESS @conditional_0 [0]\nZ 2\nLABEL @conditional_0\nJUMP-UNLESS @conditional_1 [1]\nX 2\nLABEL @conditional_1\nMEASURE 2 [2]\n", "trials" : 10 }
[2017-06-20 17:43:40.439] [xacc-console] [info] Successful HTTP Post to Rigetti.
[2017-06-20 17:43:40.439] [xacc-console] [info] Rigetti QVM Response:
[[0,1,1],[1,1,1],[1,1,1],[0,1,1],[1,0,1],[1,1,1],[0,1,1],[0,1,1],[0,0,1],[0,0,1]]
[2017-06-20 17:43:40.439] [xacc-console] [info]
[xacc] XACC Finalizing
[xacc::compiler] Cleaning up Compiler Registry.
[xacc::accelerator] Cleaning up Accelerator Registry.
```

You should see the console text printed above. 

You can also tailor the number of executions that occur for the multishot execution type: 


```bash
$ ./test-xacc-rigetti --rigetti-trials 1000
[2017-06-20 17:50:57.285] [xacc-console] [info] [xacc] Initializing XACC Framework
[2017-06-20 17:50:57.301] [xacc-console] [info] [xacc::compiler] XACC has 3 Compilers available.
[2017-06-20 17:50:57.301] [xacc-console] [info] [xacc::accelerator] XACC has 2 Accelerators available.
[2017-06-20 17:50:57.307] [xacc-console] [info] Executing Scaffold compiler.
[2017-06-20 17:50:57.310] [xacc-console] [info] Rigetti Json Payload = { "type" : "multishot", "addresses" : [0, 1, 2], "quil-instructions" : "X 0\nH 1\nCNOT 1 2\nCNOT 0 1\nH 0\nMEASURE 0 [0]\nMEASURE 1 [1]\nJUMP-UNLESS @conditional_0 [0]\nZ 2\nLABEL @conditional_0\nJUMP-UNLESS @conditional_1 [1]\nX 2\nLABEL @conditional_1\nMEASURE 2 [2]\n", "trials" : 100 }
[2017-06-20 17:50:57.909] [xacc-console] [info] Successful HTTP Post to Rigetti.
[2017-06-20 17:50:57.909] [xacc-console] [info] Rigetti QVM Response:
[[1,0,1],[0,0,1],[1,1,1],[0,1,1],[1,0,1],[0,1,1],[0,0,1],[1,1,1],[1,0,1],[1,0,1],[0,0,1],[1,0,1],[1,1,1],[0,1,1],[0,0,1],[1,1,1],[1,0,1],[1,1,1],[0,0,1],[1,1,1],[1,0,1],[0,0,1],[0,0,1],[1,0,1],[0,1,1],[0,0,1],[1,1,1],[0,0,1],[0,1,1],[1,1,1],[1,0,1],[1,0,1],[0,1,1],[0,1,1],[1,1,1],[1,1,1],[1,1,1],[0,1,1],[1,1,1],[1,0,1],[1,0,1],[1,1,1],[1,1,1],[0,0,1],[1,1,1],[0,0,1],[1,0,1],[1,1,1],[1,0,1],[1,1,1],[0,1,1],[0,1,1],[1,0,1],[0,0,1],[1,1,1],[0,1,1],[0,1,1],[1,1,1],[1,0,1],[1,1,1],[0,0,1],[0,0,1],[1,0,1],[0,1,1],[0,0,1],[0,1,1],[1,0,1],[0,1,1],[1,0,1],[0,0,1],[1,0,1],[1,1,1],[1,0,1],[1,1,1],[0,0,1],[0,1,1],[1,0,1],[1,1,1],[1,1,1],[0,1,1],[1,0,1],[1,1,1],[0,1,1],[1,0,1],[1,0,1],[0,0,1],[1,0,1],[0,0,1],[0,0,1],[1,0,1],[1,1,1],[0,1,1],[0,1,1],[0,1,1],[1,0,1],[1,1,1],[1,1,1],[0,1,1],[0,1,1],[0,1,1]]
[2017-06-20 17:50:57.910] [xacc-console] [info]
[xacc] XACC Finalizing
[xacc::compiler] Cleaning up Compiler Registry.
[xacc::accelerator] Cleaning up Accelerator Registry.
```

Note above we let XACC find the API Key in the standard .pyquil_config file. 






