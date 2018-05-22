XACC Python Bindings
=====================

Example Usage
--------------

Suppose you had the Rigetti plugin installed `see here <rigetti.rst>`_, and 
you wanted to program a simple teleportation code and launch it on the Rigetti QVM
To program this with the XACC Python bindings you could write the following python script:

.. code::

   import pyxacc as xacc

   # Create the source code
   src = """__qpu__ test(AcceleratorBuffer b) {
   X 0
   H 1
   CNOT 1 2
   CNOT 0 1
   CNOT 1 2
   CNOT 2 0
   MEASURE 2 [0]
   }
   """
   # Initialize the framework
   xacc.Initialize()

   # Get reference to the Rigetti QVM
   qpu = xacc.getAccelerator('rigetti')

   # Create some qubits to operate on
   qreg = qpu.createBuffer('qreg',3)

   # Create the program and build it
   p = xacc.Program(qpu, src)
   p.build()

   # Get the teleport kernel
   k = p.getKernel('teleport')

   # Execute on qreg with no InstructionParameters
   k.execute(qreg, [])

   # Display the results
   print(qreg.getMeasurementStrings())
   print(qreg.getExpectationValueZ())

   # Finalize the framework
   xacc.Finalize()


