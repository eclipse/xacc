XACC Tutorials
==============

XACC Python JIT
---------------

Simple Teleportation Kernel
+++++++++++++++++++++++++++

.. code::

   import xacc

   # Initialize the framework
   xacc.Initialize()

   # Get the desired QPU and
   # allocate some qubits
   qpu = xacc.getAccelerator('ibm') # or rigetti, tnqvm, etc.
   qubits = qpu.createBuffer('q',3)

   # Define the XACC Kernel
   @xacc.qpu(accelerator=qpu)
   def teleport(buffer):
      X(0)
      H(1)
      CNOT(1,2)
      CNOT(0,1)
      CNOT(1,2)
      CNOT(2,0)
      Measure(2, 0)

   teleport(qubits)

   # Display the results
   print(results)

   # Finalize the framework
   xacc.Finalize()

``xacc()`` Instruction and IRGenerators
+++++++++++++++++++++++++++++++++++++++

D-Wave Python JIT
+++++++++++++++++

AcceleratorBuffer Usage
-----------------------
Children and Extra Information
++++++++++++++++++++++++++++++

Create and Contribute a Plugin
------------------------------

Diatomic Hydrogen Ground State Energy
-------------------------------------
C++
+++

Python
++++++
