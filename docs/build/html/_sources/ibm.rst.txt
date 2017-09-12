IBM Integration
================

Installation
-------------

.. note::

   If you want support for the IBM Accelerator, you must install
   `CppRestSDK <https://github.com/microsoft/cpprestsdk>`_ and OpenSSL. This
   is required to make remote HTTP Rest calls to the IBM Quantum Experience 
   server APIs. Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) brew install cpprestsdk
      $ (fedora) dnf install cpprest-devel openssl-devel
      $ (ubuntu) apt-get install libcpprest-dev libssl-dev
      $ (spack) spack install cpprestsdk
      $ (spack) spack install openssl


The `IBM Plugin <https://github.com/ornl-qci/xacc-ibm>`_ provides
support to XACC for executing programs
on the IBM Quantum Experience via the IBM Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-ibm

.. note::

   It has been observed on Mac OS X that the above command may fail 
   due to CMake incorrectly finding OpenSSL. If that happens, run the 
   following:
   
   .. code::
      
      $ xacc-install-plugins.py -p xacc-ibm -a OPENSSL_ROOT_DIR=/usr/local/opt/openssl

   Homebrew creates the above OpenSSL root directory. If yours is different, then 
   set the ``OPENSSL_ROOT_DIR`` as such. 

You have now installed the IBM plugin. It is located in ``$XACC_ROOT/lib/plugins/accelerator``,
where ``XACC_ROOT`` is your XACC install prefix.

Hydrogen VQE Example
---------------------
Here we demonstrate how to use XACC to compute the expectation values for the
terms in the Hamiltonian modeling the
electronic structure of a simple Hydrogen molecule. The quantum
kernel code for this example can be found in `Scalable Quantum Simulation of Molecular Energies <https://arxiv.org/abs/1512.06860>`_.

.. note::

   This example requires Scaffold. See `Install Scaffold <scaffold.html>`_ for
   installation details.


.. code-block:: cpp

   #include "XACC.hpp"

   const std::string src(""
   "__qpu__ initializeState(qbit qreg, float theta) {\n"
   "   Rx(qreg[0], 3.1415926);\n"
   "   Ry(qreg[1], 1.57079);\n"
   "   Rx(qreg[0], 7.8539752);\n"
   "   CNOT(qreg[1], qreg[0]);\n"
   "   Rz(qreg[0], theta);\n"
   "   CNOT(qreg[1], qreg[0]);\n"
   "   Ry(qreg[1], 7.8539752);\n"
   "   Rx(qreg[0], 1.57079);\n"
   "}\n"
   ""
   "__qpu__ g1Term (qbit qreg, float theta) {\n"
   "   initializeState(qreg, theta);\n"
   "   cbit creg[1];\n"
   "   creg[0] = MeasZ(qreg[0]);\n"
   "}\n"
   ""
   "__qpu__ g2Term (qbit qreg, float theta) {\n"
   "   initializeState(qreg, theta);\n"
   "   cbit creg[1];\n"
   "   creg[0] = MeasZ(qreg[1]);\n"
   "}\n"
   "__qpu__ g3Term (qbit qreg, float theta) {\n"
   "   initializeState(qreg, theta);\n"
   "   cbit creg[2];\n"
   "   creg[1] = MeasZ(qreg[1]);\n"
   "   creg[0] = MeasZ(qreg[0]);\n"
   "}\n"
   "__qpu__ g4Term(qbit qreg, float theta) {\n"
   "   initializeState(qreg, theta);\n"
   "   cbit creg[2];\n"
   "   Rx(qreg[1], 1.57079);\n"
   "   Rx(qreg[0], 1.57079);\n"
   "   creg[1] = MeasZ(qreg[1]);\n"
   "   creg[0] = MeasZ(qreg[0]);\n"
   "}\n"
   ""
   "__qpu__ g5Term(qbit qreg, float theta) {\n"
   "   initializeState(qreg, theta);\n"
   "   cbit creg[2];\n"
   "   H(qreg[1]);\n"
   "   creg[1] = MeasZ(qreg[1]);\n"
   "   H(qreg[0]);\n"
   "   creg[0] = MeasZ(qreg[0]);\n"
   "}\n"
   "");

   int main (int argc, char** argv) {

      // Initialize the XACC Framework
      xacc::Initialize(argc, argv);

      // Create a reference to the Rigetti
      // QPU at api.rigetti.com/qvm
      auto qpu = xacc::getAccelerator("ibm");

      // Allocate a register of 3 qubits
      auto qubitReg = qpu->createBuffer("qreg", 2);

      // Create a Program
      xacc::Program program(qpu, src);

      // Request the quantum kernel representing
      // the above source code
      auto kernels = program.getKernels<float>();
      std::ofstream file("out.csv");
      file << "Angle, Z0, Z1, Z0Z1, Y0Y1, X0X1\n";

      auto pi = 3.14159265359;
      for (float theta = -pi; theta <= pi; theta += .2) {

         file << theta;

         // Skip the first kernel, it is the state prep
         // kernel that all others will call anyway
         for (int i = 1; i < kernels.size(); i++) {
            file << ", ";
            std::cout << "Executing Kernel " << i << "\n";
            kernels[i](qubitReg, theta);
            std::cout << "Done Executing Kernel " << i << "\n";
            auto e = qubitReg->getExpectationValueZ();
            qubitReg->resetBuffer();
            file << e;
         }
        file << "\n";
        file.flush();
      }

      file.close();

      // Finalize the XACC Framework
      xacc::Finalize();

      return 0;
   }


