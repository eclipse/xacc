Applications Built on XACC
===========================
XACC is designed to serve as a core framework for the development of hybrid
quantum-classical applications. Its extensible interfaces can be implemented to
provide application-specific problems, compilers, and backend accelerators. Here
we detail specific applications leveraging XACC.

XACC-VQE
--------
The variational quantum eigensolver algorithm provides (VQE) a hybrid quantum-classical
approach for computing the lowest eigenvalue of a Hamiltonian by positing some
parameterized circuit ansatz, and classically optimizing those parameters to find that
optimal, lowest eigenvalue.

Often times these Hamiltonians are expressed as second quantized fermionic Hamiltonians
that require some additional compile steps to map to a spin Hamiltonian amenable for
use with quantum computation.

The `XACC-VQE <https://github.com/ornl-qci/xacc-vqe>`_ application builds off of XACC
extensible interfaces to provide a compilation and execution workflow for high-level
problems leveraging the VQE algorithm. It enables
the expression of XACC quantum kernels that model high-level second-quantized
fermionic Hamiltonians and are compiled to the XACC gate model intermediate representation.
It provides C++ and Python APIs for executing this algorithm, as well as a command
line utility that takes input information on the Hamiltonian and the circuit ansatz
to execute the VQE algorithm.

Since it builds off of the XACC framework, XACC-VQE provides a way for users to program,
compile, and execute VQE in a manner that is independent of the underlying gate
model QPU. Therefore running VQE on a set of different quantum computers can be
accomplished via switching a string ('ibm' -> 'rigetti').

Architecture Overview
++++++++++++
The core abstraction for the XACC-VQE application is the concept of a ``VQETask``,
which is an interface representing some VQE-like action the user would like to accomplish.
Common VQETask implementations are the ``VQEMinimizeTask`` and the ``ComputeEnergyVQETask``
which run the full VQE algorithm or just compute the energy at a given parameter
configuration, respectively. The ``VQETask`` leverages the XACC CppMicroServices
infrastructure and therefore is an extensible plugin.

The ``VQETask`` delegates to the ``ComputeEnergyVQETask`` leveraging it throughout
the variational loop. Furthermore, it delegates to an interface called ``VQEBackend``
which abstracts the classical optimizer to be used as part of the VQE parameter search.
This is also an extensible interface plugged into XACC, and implementations exist
for Nelder-Mead, Bayesian Optimization, Conjugate Gradient, Particle Swarm, etc. (its extensible, so
more to come in the future).

XACC-VQE provides an ``xacc::Compiler`` implementation called the ``FermionCompiler``
which takes XACC quantum kernel source strings representing a fermionic second-quantized
Hamiltonian and compiles it to the gate model XACC IR. To do so, it delegates to an
extensible XACC IRTransformation that represents Jordan-Wigner, Bravyi-Kitaev, or any other
fermion-to-spin transformation (JW, BK are currently implemented). The ``FermionCompiler``
reads quantum kernels with the following language syntax

.. code::

   __qpu__ kernel(AcceleratorBuffer b) {
      1.0 1 1 0 0 // adag_1 a_0
      2.0 2 1 3 1 1 0 0 0 // adag_2 adag_3 a_1 a_0
      ...
   }

Each line in this language represents a term of the Hamiltonian. The first value is the
term coefficient, followed by (site, creation/annhilation (1/0)) pairs.

The XACC-VQE C++ and Python API expose a class called the ``PauliOperator`` that
models a spin Hamiltonian (terms made up of I, X, Y, Z on various qubit sites, with an
associated coefficient). Instantiation of these types in Python is accomplished in the following way

.. code::

   ham = PauliOperator(5.906709445) + \
        PauliOperator({0:'X',1:'X'}, -2.1433) + \
        PauliOperator({0:'Y',1:'Y'}, -2.1433) + \
        PauliOperator({0:'Z'}, .21829) + \
        PauliOperator({1:'Z'}, -6.125)

Note the ``PauliOperator`` respects the usual algebraic manipulations
on these types of Hamiltonians and auto-simplifies itself during the process. Its
constructor takes a map of site:Pauli key-value pairs, and the term coefficient.

XACC-VQE leverages the extensible ``IRGenerator`` interface to auto-generate problem-specific
VQE circuit ansatz's. Currently implemented are the UCCSD (unitary coupled cluster
singlet doublet) and HWE (hardware efficient) ``IRGenerators``.

Installation
++++++++++++
With XACC installed, run the following to build XACC-VQE

.. code::

   $ git clone --recursive https://github.com/ornl-qci/xacc-vqe
   $ cd xacc-vqe && mkdir build && cd build
   $ cmake .. -DXACC_DIR=~/.xacc -DPYTHON_INCLUDE_DIR=/usr/include/python3.5 (or wherever XACC and Python are installed)
   $ make install

This will build and install all XACC-VQE provided plugins as well as the ``xacc-vqe``
command line utility and the ``xaccvqe`` Python module. ``xacc-vqe`` will be installed
to $XACC_DIR/bin.

If you installed XACC via ``pip``, then you can run

.. code::

   $ python -m pip install xacc-vqe (with --user if you used that flag for your xacc install)

``xacc-vqe`` Command Line
+++++++++++++++++++++++++
Suppose we have a file ``h2-sto3g.hpp`` with the following XACC quantum kernel

.. code::

   __qpu__ kernel() {
      0.7080240949826064
      -1.248846801817026 0 1 0 0
      -1.248846801817026 1 1 1 0
      -0.4796778151607899 2 1 2 0
      -0.4796778151607899 3 1 3 0
      0.33667197218932576 0 1 1 1 1 0 0 0
      0.0908126658307406 0 1 1 1 3 0 2 0
      0.09081266583074038 0 1 2 1 0 0 2 0
      0.331213646878486 0 1 2 1 2 0 0 0
      0.09081266583074038 0 1 3 1 1 0 2 0
      0.331213646878486 0 1 3 1 3 0 0 0
      0.33667197218932576 1 1 0 1 0 0 1 0
      0.0908126658307406 1 1 0 1 2 0 3 0
      0.09081266583074038 1 1 2 1 0 0 3 0
      0.331213646878486 1 1 2 1 2 0 1 0
      0.09081266583074038 1 1 3 1 1 0 3 0
      0.331213646878486 1 1 3 1 3 0 1 0
      0.331213646878486 2 1 0 1 0 0 2 0
      0.09081266583074052 2 1 0 1 2 0 0 0
      0.331213646878486 2 1 1 1 1 0 2 0
      0.09081266583074052 2 1 1 1 3 0 0 0
      0.09081266583074048 2 1 3 1 1 0 0 0
      0.34814578469185886 2 1 3 1 3 0 2 0
      0.331213646878486 3 1 0 1 0 0 3 0
      0.09081266583074052 3 1 0 1 2 0 1 0
      0.331213646878486 3 1 1 1 1 0 3 0
      0.09081266583074052 3 1 1 1 3 0 1 0
      0.09081266583074048 3 1 2 1 0 0 1 0
      0.34814578469185886 3 1 2 1 2 0 3 0
   }

and we would like to use the UCCSD ansatz to compute the ground state energy of
this corresponding Hamiltonian. One could run the following

.. code::

   $ xacc-vqe -f h2-sto3g.hpp -t vqe --n-electrons 2

To run the compute-energy task, one could

.. code::

   $ xacc-vqe -f h2-sto3g.hpp -t compute-energy --n-electrons 2 --vqe-parameters "0.0,-.05"

To run with a custom ansatz written as an XACC quantum kernel in a file ``ansatz.hpp``,

.. code::

   $ xacc-vqe -f h2-sto3g.hpp -a ansatz.hpp -t vqe --n-electrons 2

The previous examples will run by default on the TNQVM Accelerator (if installed). To switch
just pass ``--accelerator ACCELERATORNAME`` to the command line arguments.

Python API
++++++++++
XACC-VQE exposes a Python API to enable ease of scripting for the VQE algorithm
targeting available quantum computers. In essence, the Python API exposes two functions:
``compile`` and ``execute``. ``compile`` takes as input a high-level fermionic Hamiltonian represented as a
XACC quantum kernel string (just like the one above for H2) and runs the appropriate
``FermionCompiler`` to map it to a ``PauliOperator`` instance. Imagine we had the above
quantum kernel as a string in Python, ``h2Src``

.. code::

   import xaccvqe as vqe
   pauliOp = vqe.compile(h2Src)
   print(pauliOp)

would produce the following PauliOperator output

.. code::

   (0.174073,0) Z2 Z3 + (0.1202,0) Z1 Z3 + (0.165607,0) Z1 Z2 + (0.165607,0) Z0 Z3 +
   (0.1202,0) Z0 Z2 + (-0.0454063,0) Y0 Y1 X2 X3 + (-0.220041,0) Z3 + (-0.106477,0) +
   (0.17028,0) Z0 + (-0.220041,0) Z2 + (0.17028,0) Z1 + (-0.0454063,0) X0 X1 Y2 Y3 +
   (0.0454063,0) X0 Y1 Y2 X3 + (0.168336,0) Z0 Z1 + (0.0454063,0) Y0 X1 X2 Y3

XACC-VQE integrates nicely with `OpenFermion <https://github.com/quantumlib/openfermion>`_. As such
this ``compile`` API function can also take ``FermionOperators`` as input and compile them to
``PauliOperators``. If one has an OpenFermion ``QubitOperator``, ``xaccvqe`` also
provides a ``QubitOperator2XACC`` function that maps the ``QubitOperator`` to a ``PauliOperator``.

The ``execute`` method takes 3 arguments. The first is a representation of the Hamiltonian
provided as a ``PauliOperator`` or a ``FermionOperator``. The second is the ``AcceleratorBuffer``
to operate on (the user keeps this reference until after execution in order to
retrieve the results of the computation). And the final argument is a Python ``**kwargs``
key-value dictionary of parameters that direct the overall compilation and execution
workflow. This dictionary should detail the ``'task'``, ``'ansatz'``, ``'vqe-params'``, and
``'accelerator'`` keys.

Here are some examples of using the ``compile`` and ``execute`` API. This will run
the H2 example, with the default ansatz of UCCSD.

.. code::

   import xacc
   import xaccvqe as vqe
   xacc.Initialize()

   # See above kernel for full definition
   h2Src = '__qpu__ h2(AcceleratorBuffer ab) {...}'

   pauliOp = vqe.compile(h2Src)

   qpu = xacc.getAccelerator('ibm')
   qubits = qpu.createBuffer('q',4)

   vqe.execute(pauliOp, qubits, **{'task':'vqe', 'accelerator':qpu.name()})

   print(qubits.getInformation('vqe-energy'))
   print(qubits.getInformation('vqe-angles'))

   xacc.Finalize()

Running with a custom ansatz can be seen in the below example (ansatz expressed as
xacc quantum kernel written in Quil)

.. code::

   import xacc, xaccvqe as vqe
   from xaccvqe import PauliOperator

   xacc.Initialize(['--compiler','quil'])

   qpu = xacc.getAccelerator('rigetti')
   qubits = qpu.createBuffer('q',2)

   ham = PauliOperator(5.906709445) + \
        PauliOperator({0:'X',1:'X'}, -2.1433) + \
        PauliOperator({0:'Y',1:'Y'}, -2.1433) + \
        PauliOperator({0:'Z'}, .21829) + \
        PauliOperator({1:'Z'}, -6.125)

   src = """__qpu__ f(AcceleratorBuffer b, double t0) {
              X 0
              RY(t0) 1
              CNOT 1 0
           }"""
   ansatz = xacc.compileKernel(qpu, src)

   vqe.execute(pauliOp, qubits, **{'task':'vqe', 'ansatz':ansatz, 'accelerator':qpu.name()})

   print(qubits.getInformation('vqe-energy'))
   print(qubits.getInformation('vqe-angles'))

   xacc.Finalize()


Python JIT VQE Decorator
++++++++++++++++++++++++
The easiest way to run the VQE algorithm with XACC-VQE is to leverage the
``xacc.qpu.vqe`` decorator. This decorator builds on the ``xacc.qpu`` decorator
and gives users an opportunity to define the VQE ansatz as a single Python function,
and through this algorithmic decoration, execute the VQE algorithm given some set
of decorator arguments. Let's look at an example

.. code::

   import xacc
   import xaccvqe
   from xaccvqe import PauliOperator

   xacc.Initialize()

   tnqvm = xacc.getAccelerator('tnqvm')
   buffer = tnqvm.createBuffer('q', 2)

   ham = PauliOperator(5.906709445) + \
        PauliOperator({0:'X',1:'X'}, -2.1433) + \
        PauliOperator({0:'Y',1:'Y'}, -2.1433) + \
        PauliOperator({0:'Z'}, .21829) + \
        PauliOperator({1:'Z'}, -6.125)

   @xaccvqe.qpu.vqe(accelerator=tnqvm, observable=ham)
   def ansatz(buffer, t0):
      X(0)
      Ry(t0, 1)
      CNOT(1, 0)

   # Run VQE with given ansatz kernel
   initAngle = .5

   ansatz(buffer, initAngle)

   print('Energy = ', buffer.getInformation('vqe-energy'))
   print('Opt Angles = ', buffer.getInformation('vqe-angles'))
