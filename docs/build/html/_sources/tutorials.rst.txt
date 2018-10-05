XACC Tutorials
==============
Here we aggregate a number of examples that demonstrate how to use the XACC
framework for the programming, compilation, and execution of quantum programs on
available quantum computers.

XACC Python JIT
---------------
The XACC Python bindings provide a simple and intuitive mechanism for
programming XACC kernels through a single-source, co-processor programming model.
This mechanism enables users to write quantum code once and easily switch amongst
available quantum computing hardware. This provides a Just-in-Time compilation workflow,
i.e. these XACC Pythonic quantum kernels are compiled at runtime.

Simple Teleportation Kernel
+++++++++++++++++++++++++++
An illustrative example of the utility of the XACC Python JIT mechanism is the simple
quantum state teleportation code. Here we write the kernel once and can switch the
execution context amongst IBM, Rigetti, and a variety of simulators by simply updating
the ``xacc.qpu`` accelerator key. See below

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
   print(qubits)

   # Finalize the framework
   xacc.Finalize()

Here we initialize the framework (must be done before using the XACC API), and then
get reference to the Accelerator we want to execute on. We then allocate 3 qubits
(represented as an ``AcceleratorBuffer`` instance)
with the ``Accelerator::createBuffer`` command (akin to ``malloc`` for C). We define
the quantum teleportation code using the ``PyXACCCompiler`` language and annotate it
with the ``xacc.qpu`` decorator and indicate which Accelerator we'd like to execute
on. Execution of this code on the IBM QPU is then affected by simply calling this Python
function. The results of the execution are stored on the users allocated ``AcceleratorBuffer``.
Finally we cleanup the framework with the ``xacc.Finalize()`` call.

``xacc()`` Instruction and IRGenerators
+++++++++++++++++++++++++++++++++++++++
XACC exposes an extensible interface for the generation of ``IR`` instances based
on simple input parameters (this differentiates it from ``Compilers`` which also
generate ``IR`` but do so given kernel source code). This interface is called
the ``IRGenerator`` and it exposes a ``generate`` method that takes as input
a set of ``InstructionParameters`` (these are essentially a ``variant`` type on
``int``, ``float``, ``double``, ``string``, and ``complex<double>``).

The ``IRGenerator`` comes in handy when one wants to program complex quantum programs
that may contain many instructions. They provide a mechanism for easily generating
these programs based on simple input parameters. An example would be the generation of
a quantum fourier transform. It would be easier to declare in a kernel that you would
like to operate a QFT on a subset of qubits than to write out the individual gates that
make up the QFT. Instead one could simply say 'operate a QFT on these qubits', and the
XACC IRGenerator mechanism would produce this sub-circuit (The XACC framework does
provide ``QFT`` and ``InverseQFT`` ``IRGenerators``).

The XACC Python PyXACCCompiler language provides an instruction that lets users
express this ``IRGenerator`` generation step on a single line of quantum code. To do so,
in addition to standard gates and ``qmi`` instructions, this language defines an
``xacc()`` instruction that takes as its first input the name of the IRGenerator
to be used in building up the quantum kernel, followed by any key-value pair arguments.
Here's an example of generating a hardware-efficient ansatz used in a variational
quantum eigensolver routine

.. code::

   @xacc.qpu(accelerator=ibm)
   def hwe(buffer, *args):
      xacc(hwe, layers=2, n_qubits=2, connectivity='[[0,1]]')

This would generate the following circuit

.. code::

   Ry(t0) q0
   Rz(t1) q0
   Ry(t2) q1
   Rz(t3) q1
   CNOT q0,q1
   Ry(t4) q0
   Rz(t5) q0
   Ry(t6) q1
   Rz(t7) q1
   CNOT q0,q1

.. note::

   The ``xacc.qpu`` decorate provides a mechanism for querying
   information about the circuit at runtime. One could get the
   number of runtime parameters, and the IR Function itself with the following
   .. code::

      print(hwe.nParameters())
      print(hwe.getFunction().toString('q'))

   So executing a function with unknown number of parameters can be
   accomplished like this
   .. code::

      init = np.random.uniform(low=-np.pi,high=np.pi, size=(hwe.nParameters(),))
      hwe(buffer, *init)

This ``xacc()`` instruction can be used with any available ``IRGenerator`` present
in the XACC framework.

D-Wave Python JIT
+++++++++++++++++
We can similarly use this Python JIT mechanism for writing and executing D-Wave
code. The ``PyXACCCompiler`` language exposes a ``qmi`` instruction that takes
the indices of the qubits for which we are defining a bias or coupler (qubit indices equal,
we have a bias, qubit indices not equal we have a coupler). And of course these
biases and couplers can be runtime parameters. See below for an example.

.. code::

   import xacc

   xacc.Initialize()

   # Get access to D-Wave QPU and
   # allocate some qubits
   dwave = xacc.getAccelerator('dwave')
   qubits = dwave.createBuffer('q')

   # Define the function we'd like to
   # off-load to the QPU, here
   # we're using a the QMI low-level language
   @xacc.qpu(accelerator=dwave)
   def f(buffer, h, j):
      qmi(0,0,h)
      qmi(1,1,h)
      qmi(0,1,j)

   # Execute on D-Wave
   f(qubits, 1., 2.)

   # Print the buffer, this displays
   # solutions and energies
   print(qubits)

   xacc.Finalize()

Or, if we have an ``IRGenerator`` for a D-Wave problem, we could use the ``xacc()``
instruction. Imagine we have an ``IRGenerator`` implemented that takes an integer ``N``
and creates a D-Wave IR instance that factors ``N`` into its constituent primes.
Our code would look like this

.. code::

   import xacc

   xacc.Initialize()

   # Get access to D-Wave QPU and
   # allocate some qubits
   dwave = xacc.getAccelerator('dwave')
   buffer = dwave.createBuffer('q')

   # Define the function we'd like to
   # off-load to the QPU, here
   # we're using a parameterized Factoring
   # IR Generator
   @xacc.qpu(accelerator='dwave')
   def factor15(buffer):
      xacc(dwave-factoring, n=15)

   # Factor 15 on the D-Wave
   factor15(buffer)

   # We have solutions as 0s and 1s
   # decode that into our factors
   xacc.analyzeBuffer(buffer)

   # Print the factors
   factors = buffer.getInformation('analysis-results')
   xacc.info('Factors = ' + str(factors))

   xacc.Finalize()

Note here that we leveraged an XACC API function called ``xacc.analyzeBuffer``. All
``IRGenerators`` expose a second method called ``analyzeResults`` that takes as input
the ``AcceleratorBuffer`` and parses the resultant bit strings back up to the problem
scope, which in this case is the prime factors we are seeking. The ``xacc.analyzeBuffer``
provides a convenience method for executing the correct ``IRGenerator::analyzeResults``
method.

AcceleratorBuffer Usage
-----------------------
The ``AcceleratorBuffer`` makes up the glue between host-side code and ``Accelerator``
results. It abstracts the concept of a register of qubits that can be measured, and stores
information about the number of qubits in the register and observed measurement bit strings
and counts. This information is then readily available to the creator of the ``AcceleratorBuffer``
and can be used to get higher-level problem result information.

The ``AcceleratorBuffer`` keeps track of what we call ``ExtraInfo``, which is a
``variant`` type that can be of type ``int``, ``double``, ``std::string``, ``std::vector<int>``,
``std::vector<double>``, ``std::vector<std::string>``, or ``std::map<int, std::vector<int>>``.
The ``AcceleratorBuffer`` keeps reference to a map of ``std::string`` to ``ExtraInfo`` which
enables it to keep track of problem-specific information such as ``vqe-energy``, ``vqe-angles``,
``vqe-nQPU-calls`` (for the problem-specific case of VQE). But really, this mechanism can
store any kind of string key to ExtraInfo value (a great example is storing a minor graph embedding
for a D-Wave compilation/execution as a map of ints to vector<int>).

Furthermore, the ``AcceleratorBuffer`` can have children, and each child is an ``AcceleratorBuffer``
with associated ``ExtraInfo`` information. This is great for algorithms like VQE where
one may allocate a single register of qubits to operate on at the top-level, but many
executions are actually executed for the various Pauli Hamiltonian terms.

Let's look at a simple example of using the AcceleratorBuffer and accessing its data. Imagine
we had the following code running on the Local IBM C++ Simulator

.. code::

   import xacc
   import numpy as np

   xacc.Initialize()

   qpu = xacc.getAccelerator('local-ibm')
   buffer = qpu.createBuffer('q',2)

   @xacc.qpu(accelerator=qpu)
   def foo(buffer, theta):
      X(0)
      Ry(theta, 1)
      CNOT(1, 0)
      Measure(0,0)

   # Execute the code on the QPU
   foo(buffer, -1.1415936)
   print(foo.nParameters())

   # Check into the results
   print('Buffer as Json: ', buffer)
   print('Expectation Value wrt Z: ', buffer.getExpectationValueZ())
   print('Measurement Counts: ', buffer.getMeasurementCounts())

   xacc.Finalize()

This will output the following

.. code::

   Buffer as Json:  {
    "AcceleratorBuffer": {
        "name": "q",
        "size": 2,
        "Information": {},
        "Measurements": {
            "00": 288,
            "01": 736
        }
     }
   }
   Expectation Value wrt Z:  -0.4375
   Measurement Counts:  {'00': 288, '01': 736}

Note that this ``AcceleratorBuffer`` instance has been populated with measurement
bit strings and their counts. This is a very simple example, so there is no
extra information involved in this calculation,
but we can compute the expectation value with respect to the Z operator.

For a more complex example that does include extra information, let us consider the
above D-Wave Factoring code that leverages the ``dwave-factoring`` ``IRGenerator``.
Running this code and printing the buffer produces the following

.. code::

   {
       "AcceleratorBuffer": {
           "name": "q",
           "size": 2048,
           "Information": {
               "active-vars": [
                   1944,
                   1946,
                   1947,
                   1948,
                   1949,
                   1951
               ],
               "analysis-results": [
                   3,
                   5
               ],
               "embedding": {
                   "0": [
                       1948,
                       1947
                   ],
                   "1": [
                       1949
                   ],
                   "2": [
                       1946,
                       1951
                   ],
                   "3": [
                       1944
                   ]
               },
               "energies": [
                   -999.75
               ],
               "execution-time": 0.023982,
               "ir-generator": "dwave-factoring",
               "num-occurrences": [
                   100
               ]
           },
           "Measurements": {
               "010001": 100
           }
       }
   }

Notice the execution workflow has populated the ``AcceleratorBuffer`` with multiple
extra information entries: the active-vars, embedding, energies, execution time,
ir-generator used, and measurement counts. Furthermore, invoking the ``xacc.analyzeBuffer``
function mapped the bit strings back to problem-specific results, namely 3 and 5,
and added it to the buffer under the analysis-results key.

One can query this information with the ``getInformation`` method

.. code::

   print('Embedding: ', buffer.getInformation('embedding'))
   print('Active-Vars: ', buffer.getInformation('active-vars'))
   print('Energies: ', buffer.getInformation('energies'))
   print('IRGenerator: ', buffer.getInformation('ir-generator'))

to produce

.. code::

   Embedding:  {0: [1948, 1947], 1: [1949], 2: [1946, 1951], 3: [1944]}
   Active-Vars:  [1944, 1946, 1947, 1948, 1949, 1951]
   Energies:  [-999.75]
   IRGenerator:  dwave-factoring

To see a further complex example that uses ``AcceleratorBuffer`` children, see
`XACC-VQE <apps.html#python-jit-vqe-decorator>`_.

Also note that the ``AcceleratorBuffer`` can be persisted to file and loaded
from file.

.. code::

   bufferStr = str(buffer)
   ... write to file
   ... load from file, get loadedStr
   newBuff = xacc.loadBuffer(loadedStr)
