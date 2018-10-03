Rigetti
=======

Installation
-------------

The `Rigetti Plugin <https://github.com/ornl-qci/xacc-rigetti>`_ provides
support to XACC for compiling kernels writting in Quil, and executing programs
on the Rigetti QVM via a Rigetti Accelerator.

To install this plugin, run the following

.. code::

   $ git clone https://github.com/ornl-qci/xacc-rigetti
   $ cd xacc-rigetti && mkdir build && cd build
   $ cmake .. -DXACC_DIR=$HOME/.xacc

If you installed the XACC Python bindings, then you can run

.. code::

   $ cmake .. -DXACC_DIR=$(python -m xacc -L)

ensuring that xacc is in your ``PYTHONPATH``.

You have now installed the Rigetti plugin. It is located in ``$XACC_ROOT/plugins``
and ``$XACC_ROOT/plugins``, where ``XACC_ROOT`` is your XACC install prefix.

Setting Credentials
-------------------

In order to target the Rigetti QVM you must provide XACC with your API key. By default
XACC will use your PyQuil Config file at ``$HOME/.pyquil_config``.
See `Connecting to the Rigetti Forest <http://pyquil.readthedocs.io/en/latest/getting_started.html#connecting-to-the-rigetti-forest>`_ for information on creating this file.

If you installed the XACC Python bindings, then you can run

.. code::

   $ python -m xacc -c rigetti -k YOURAPIKEY -u YOURUSERID

Rigetti Command Line Arguments
------------------------------
The Rigetti plugin exposes the following command line arguments

+------------------------+----------------------------------------+
| Argument               |            Description                 |
+========================+========================================+
| --rigetti-backend      | The backend to target (defaults to QVM)|
+------------------------+----------------------------------------+
| --rigetti-trials       | The number of shots to execute per job |
+------------------------+----------------------------------------+

Rigetti QVM Tutorial
---------------------
Let's test out the Rigetti Accelerator by creating a code to
perform quantum teleportation.

Create a new directory called test-xacc-rigetti and cd into it. Let's now create a
test-xacc-rigetti.cpp file and get it started with the following boilerplate code:

.. code-block:: cpp

   #include "XACC.hpp"

   int main(int argc, char** argv) {

      // Initialize XACC - find all available
      // compilers and accelerators, parse command line.
      xacc::Initialize(argc, argv);

      // ... Code to come ...

      // Finalize the framework.
      xacc::Finalize();
   }

Building this code is straightforward with CMake. Create a CMakeLists.txt file in the same
directory as the test-xacc-rigetti.cpp file, and add the following to it:

.. code-block:: cmake

   # Start a CMake project
   project(test-xacc-rigetti CXX)

   # Set the minimum version to 3.2
   cmake_minimum_required(VERSION 3.2)

   # Find XACC
   find_package(XACC REQUIRED)

   # Find Boost
   find_package(Boost COMPONENTS system program_options filesystem chrono thread REQUIRED)

   # Include all XACC Include Directories
   include_directories(${XACC_INCLUDE_DIRS})

   # Link to the XACC Library Directory,
   link_directories(${XACC_LIBRARY_DIR})

   # Create the executabe
   add_executable(test-xacc-rigetti test-xacc-rigetti.cpp)

   # Like the necessary libaries
   target_link_libraries(test-xacc-rigetti ${XACC_LIBRARIES} dl pthread)

Now from within the test-xacc-rigetti directory, run the following:

.. code-block:: bash

   $ mkdir build
   $ cd build
   $ cmake ..
   $ make

This will build test-xacc-rigetti.cpp and provide you with a test-xacc-rigetti executable. Run that
executable to ensure that your build worked (you should see the following output):

.. code-block:: bash

   $ make
   $ ./test-xacc-rigetti
   [2017-06-20 16:14:07.076] [xacc-console] [info] [xacc] Initializing XACC Framework
   [2017-06-20 16:14:07.091] [xacc-console] [info] [xacc::compiler] XACC has 1 Compilers available.
   [2017-06-20 16:14:07.091] [xacc-console] [info] [xacc::accelerator] XACC has 1 Accelerators available.
   [2017-06-20 16:14:07.091] [xacc-console] [info]
   [xacc] XACC Finalizing
   [xacc::compiler] Cleaning up Compiler Registry.
   [xacc::accelerator] Cleaning up Accelerator Registry.

Now that we have our build and initial boilerplate code setup, let's actually write some quantum code, specifically
teleporting the state of one qubit to another. Following the XACC.hpp include statement at the top of the file,
add the following quantum kernel declaration:

.. code-block:: cpp

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

Now we are ready to build and execute this kernel using the XACC Runtime API. After the call
to xacc::Initialize, add the following:

.. code-block:: cpp

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

The code above starts by getting a reference to the RigettiAccelerator.
With that reference, we then allocate a register of qubits
to operate the teleport kernel on. Next, we instantiate an XACC Program instance, which keeps track of the
desired Accelerator and the source code to be compiled. The Program instance orchestrates the compilation of the
quantum kernel to produce the XACC intermediate representation, and then handles the creation of an
executable classical kernel function that offloads the compiled quantum code to the specified Accelerator.
Finally, the user requests a reference to the executable kernel functor, and executes it on the
provided register of qubits.

The total test-xacc-rigetti.cpp file should look like this:

.. code-block:: cpp

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

Now, to build simple run:

.. code-block:: bash

   $ cd test-xacc-rigetti/build
   $ make


To execute this code on the Rigetti QVM, you must provide your API key. You can do this
the same way you do with PyQuil (in your $HOME/.pyquil_config file, or in the $PYQUIL_CONFIG
environment variable). You can also pass your API key to the XACC executable through the
--rigetti-api-key command line argument:

.. code-block:: bash

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

You should see the console text printed above.

You can also tailor the number of executions that occur for the multishot execution type:


.. code-block:: bash

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

Note above we let XACC find the API Key in the standard .pyquil_config file.

Executing PyQuil Code on any XACC Accelerator
----------------------------------------------
If you've installed the XACC Python Bindings (see here `<python.html#Installation>`_), then you can
actually take existing PyQuil code and execute it on any of the XACC Accelerators (Rigetti, IBM, TNQVM, any future ones coming online). You probably don't need to execute on the Rigetti Accelerator, since the Forest Python API
already provides that support. But this is a great way to run comparisons on our tensor network simulator or the
5 or 16 qubit IBM QPUs.

Check out the following example that uses PyQuil to program the VQE problem for
the Hydrogen molecule (code taken from `Scalable Quantum Simulation of Molecular Energies <https://arxiv.org/abs/1512.06860>`_ ).

.. note::

   To run this, you must install pyQuil:

   .. code::

      $ pip install pyquil


.. code-block:: python

   from pyquil.quil import Program
   import pyquil.api as forest
   from pyquil.gates import X, Z, H, CNOT, RX, RY, RZ
   import numpy as np
   import xacc

   # Pyquil State Preparation circuit gen
   def statePrep(qs, angle):
      return Program(RX(np.pi, qs[0]),
                  RY(np.pi / 2, qs[1]),
                  RX(2.5*np.pi, qs[0]),
                  CNOT(qs[1],qs[0]),
                  RZ(angle, qs[0]),
                  CNOT(qs[1],qs[0]),
                  RY(2.5*np.pi, qs[1]),
                  RX(np.pi / 2, qs[0]))

   # Z0 term of Hamiltonian
   def Z0Term(qs, angle):
      p = statePrep(qs,angle)
      p.measure(qs[0],0)
      return p

   # Z1 term of Hamiltonian
   def Z1Term(qs, angle):
      p = statePrep(qs, angle)
      p.measure(qs[1],0)
      return p

   # Z0Z1 term of Hamiltonian
   def Z0Z1Term(qs, angle):
      p = statePrep(qs, angle)
      p.measure(qs[1],1)
      p.measure(qs[0],0)
      return p

   # collect all generators in a list
   terms = [Z0Term, Z1Term, Z0Z1Term]

   # Initialize XACC
   xacc.Initialize()

   # turn on the QuilCompiler
   xacc.setOption('compiler','quil')

   # Get reference to our tensor network simulator
   qpu = xacc.getAccelerator('tnqvm')

   # create some qubits - an AcceleratorBuffer
   qs = qpu.createBuffer('qs',2)

   # Store results to this CSV file
   file = open('pyquil_out.csv', 'w')
   file.write('theta, Z0, Z1, Z0Z1\n')

   # Pyquil uses int indices for qubits
   pyquilQubits = [0,1]

   # Loop -pi to pi
   for theta in np.linspace(-np.pi,np.pi,100):
      file.write(str(theta))

      # Loop over our above generators
      for t in terms:
         file.write(', ')

         # Kernel-ize the pyquil program
         src = '__qpu__ kernel(AcceleratorBuffer qs, double theta) {\n'
         src += t([0,1], theta).out()
         src += '}'

         # Create an XACC Program and compile
         xaccProgram = xacc.Program(qpu, src)
         xaccProgram.build()

         # Get reference to the executable kernel
         k = xaccProgram.getKernel('kernel')

         # Execute, no params since theta has
         # already been input to the term generator
         k.execute(qs, [])

         # Get the expectation value
         e = qs.getExpectationValueZ()

         # Reset the qubits for the next iteration
         qs.resetBuffer()

         file.write(str(e))
      file.write('\n')

   file.close()

   # Finalize the framework.
   xacc.Finalize()


This will produce a file called ``pyquil_out.csv``. Plotting this shows the
correct expectation values from the simulation.

.. image:: ../assets/pyquil_out.png
