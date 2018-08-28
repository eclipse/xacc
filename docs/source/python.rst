XACC Python Bindings
=====================

Example Usage
--------------

Suppose you had the Rigetti plugin installed `see here <rigetti.rst>`_, and 
you wanted to program a simple teleportation code and launch it on the Rigetti QVM.
To program this with the XACC Python bindings you could write the following python script:

.. code::

   import xacc
   from xacc import qpu

   xacc.Initialize()

   # Create the source code
   @qpu(accelerator='ibm')
   def teleport(): 
      X(0)
      H(1)
      CNOT(1,2)
      CNOT(0,1)
      CNOT(1,2)
      CNOT(2,0)
      Measure(2, 0)
      return

   results = teleport()

   # Display the results
   print(results.getMeasurementStrings())

   # Finalize the framework
   xacc.Finalize()
