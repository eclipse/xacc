IBM
===

Installation
-------------

The `IBM Plugin <https://github.com/ornl-qci/xacc-ibm>`_ provides
support to XACC for executing programs
on the IBM Quantum Experience via the IBM Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p ibm

You have now installed the IBM plugin. It is located in ``$XACC_ROOT/lib/plugins/accelerators``,
where ``XACC_ROOT`` is your XACC install prefix.

Setting Credentials
-------------------

In order to target the IBM Quantum Experience, you must provide XACC with your API key.
To do so, open the file ``$HOME/.ibm_config``, and add the following contents

.. code:: bash

   key: YOUR_API_KEY
   url: https://quantumexperience.ng.bluemix.net

Alternatively, any application built on the XACC framework with IBM plugins available
will expose ``-ibm-api-url`` and ``--ibm-api-key`` command line arguments:

.. code:: bash

   $ ./my-xacc-app --ibm-api-key YOUR_API_KEY --ibm-api-url https://quantumexperience.mybluemix.net


IBM Command Line Arguments
---------------------------
The IBM plugin exposes the following command line arguments

+------------------------+----------------------------------------+
| Argument               |            Description                 |
+========================+========================================+
| --ibm-backend          | The backend to target (e.g. ibmqx5)    |
+------------------------+----------------------------------------+
| --ibm-shots            | The number of shots to execute per job |
+------------------------+----------------------------------------+
| --ibm-list-backends    | List the available backends            |
+------------------------+----------------------------------------+
| --ibm-shots            | The number of shots to execute per job |
+------------------------+----------------------------------------+

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

   const std::string src = R"src(
   __qpu__ initializeState(qbit qreg, float theta) {
      Rx(qreg[0], 3.1415926);
      Ry(qreg[1], 1.57079);
      Rx(qreg[0], 7.8539752);
      CNOT(qreg[1], qreg[0]);
      Rz(qreg[0], theta);
      CNOT(qreg[1], qreg[0]);
      Ry(qreg[1], 7.8539752);
      Rx(qreg[0], 1.57079);
   }
   
   __qpu__ g1Term (qbit qreg, float theta) {
      initializeState(qreg, theta);
      cbit creg[1];
      creg[0] = MeasZ(qreg[0]);
   }
   
   __qpu__ g2Term (qbit qreg, float theta) {
      initializeState(qreg, theta);
      cbit creg[1];
      creg[0] = MeasZ(qreg[1]);
   }
   __qpu__ g3Term (qbit qreg, float theta) {
      initializeState(qreg, theta);
      cbit creg[2];
      creg[1] = MeasZ(qreg[1]);
      creg[0] = MeasZ(qreg[0]);
   }
   __qpu__ g4Term(qbit qreg, float theta) {
      initializeState(qreg, theta);
      cbit creg[2];
      Rx(qreg[1], 1.57079);
      Rx(qreg[0], 1.57079);
      creg[1] = MeasZ(qreg[1]);
      creg[0] = MeasZ(qreg[0]);
   }
   
   __qpu__ g5Term(qbit qreg, float theta) {
      initializeState(qreg, theta);
      cbit creg[2];
      H(qreg[1]);
      creg[1] = MeasZ(qreg[1]);
      H(qreg[0]);
      creg[0] = MeasZ(qreg[0]);
   }
   )src";

   int main (int argc, char** argv) {

      // Initialize the XACC Framework
      xacc::Initialize(argc, argv);

      // Create a reference to the IBM
      // QPU at the IBM Quantum Experience
      auto qpu = xacc::getAccelerator("ibm");

      // Allocate a register of 2 qubits
      auto qubitReg = qpu->createBuffer(qreg, 2);

      // Create a Program
      xacc::Program program(qpu, src);

      // Request the quantum kernel representing
      // the above source code
      auto kernels = program.getKernels<float>();
      std::ofstream file("out.csv");
      file << "Angle, Z0, Z1, Z0Z1, Y0Y1, X0X1";"

      auto pi = 3.14159265359;
      for (float theta = -pi; theta <= pi; theta += .2) {

         file << theta;

         // Skip the first kernel, it is the state prep
         // kernel that all others will call anyway
         for (int i = 1; i < kernels.size(); i++) {
            file << , ;
            std::cout << "Executing Kernel " << i << ;
            kernels[i](qubitReg, theta);
            std::cout << "Done Executing Kernel" << i << ;
            auto e = qubitReg->getExpectationValueZ();
            qubitReg->resetBuffer();
            file << e;
         }
        file << ;
        file.flush();
      }

      file.close();

      // Finalize the XACC Framework
      xacc::Finalize();

      return 0;
   }


