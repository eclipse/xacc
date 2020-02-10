Basics
===========

Here we demonstrate leveraging the XACC framework for various
quantum-classical programming tasks. We provide examples in both
C++ and Python.

Accelerator Buffer
-------------------------
The ``AcceleratorBuffer`` represents a register of qubits. Programmers allocate
this register of a certain size, and pass it by reference to all execution tasks.
These execution tasks are carried out by concrete instances of the `Accelerator`
interface, and these instances are responsible for persisting information to the
provided buffer reference. This ensures programmers have access to all execution results
and metadata upon execution completion.

Programmers can allocate a buffer through the ``xacc::qalloc(const int n)`` (``xacc.qalloc(int)`` in Python) call.
After execution, measurement results can be queried as well as backend-specific
execution metadata. Below demonstrate some basic usage of the ``AcceleratorBuffer``

.. code:: cpp

   #include "xacc.hpp"
   ...
   // Create program somehow... detailed later
   program = ...
   auto buffer = xacc::qalloc(3);
   auto qpu = xacc::getAccelerator("ibm:ibmq_valencia");
   qpu->execute(buffer, program);
   std::map<std::string, int> results = buffer->getMeasurementCounts();
   auto fidelities = (*buffer)["1q-gate-fidelities"].as<std::vector<double>>();
   auto expValZ = buffer->getExpectationValueZ();

in Python

.. code:: python

   import xacc
   ...
   // Create program somehow... detailed later
   program = ...
   buffer = xacc.qalloc(3)
   qpu = xacc.getAccelerator('ibm:ibmq_valencia', {'shots':8192})
   qpu.execute(buffer, program)
   results = buffer.getMeasurementCounts()
   fidelities = buffer['1q-gate-fidelities']
   expValZ = buffer.getExpectationValueZ()

Intermediate Representation, Kernels, and Compilers
----------------------------------------------------
Above we mentioned a ``program`` variable but did not detail how it was created. This instance
is represented in XACC as a ``CompositeInstruction``. The creation of ``Instruction`` and
``CompositeInstruction`` is demonstrated below. First, we create this instances via an
implementation of the ``IRProvider``, specifically a 3 instruction circuit with one
parameterized ``Ry`` on a variable ``theta``.

.. code:: cpp

   #include "xacc.hpp"
   ...
   auto provider = xacc::getIRProvider("quantum");
   auto program = provider->createComposite("foo", {"theta"});
   auto x = provider->createInstruction("X", {0});
   auto ry = provider->createInstruction("Ry", {1}, {"theta"});
   auto cx = provider->createInstruction("CX", {1,0});
   program->addInstructions({x, ry, cx});

in Python

.. code:: python

   import xacc
   ...
   provider = xacc.getIRProvider('quantum')
   program = provider.createComposite('foo', ['theta'])
   x = provider.createInstruction('X', [0])
   ry = provider.createInstruction('Ry', [1], ['theta'])
   cx = provider.createInstruction('CX', [1,0])
   program.addInstructions([x,ry,cx])

We could also create IR through textual source code representations in a language
that is available to the framework. Availability here implies that there exists
a ``Compiler`` implementation for the language being used. ``Compilers`` take kernel
source strings and produce IR (one or many ``CompositeInstructions``). Here we demonstrate
the same circuit as above, but using a Quil kernel

.. code:: cpp

   #include "xacc.hpp"
   ...
   auto qpu = xacc::getAccelerator("ibm");
   auto quil = xacc::getCompiler("quil");
   auto ir = quil->compile(R"(
   __qpu__ void ansatz(AcceleratorBuffer q, double x) {
       X 0
       Ry(x) 1
       CX 1 0
   }
   __qpu__ void X0X1(AcceleratorBuffer q, double x) {
       ansatz(q, x)
       H 0
       H 1
       MEASURE 0 [0]
       MEASURE 1 [1]
   }
   )", qpu);
   auto ansatz = ir->getComposite("ansatz");
   auto x0x1 = ir->getComposite("X0X1");

in Python

.. code:: python

   import xacc
   ...
   qpu = xacc.getAccelerator('ibm')
   quil = xacc.getCompiler('quil')
   ir = quil.compile('''
   __qpu__ void ansatz(AcceleratorBuffer q, double x) {
       X 0
       Ry(x) 1
       CX 1 0
   }
   __qpu__ void X0X1(AcceleratorBuffer q, double x) {
       ansatz(q, x)
       H 0
       H 1
       MEASURE 0 [0]
       MEASURE 1 [1]
   }
   ''', qpu)
   ansatz = ir.getComposite('ansatz')
   x0x1 = ir.getComposite('X0X1')

Here, ``x0x1`` is a ``CompositeInstruction`` that can be passed to ``Accelerator::execute()`` for
backend execution.

Next we demonstrate how one might leverate ``IRTransformation`` to perform general optimizations
on IR instances.

.. code:: cpp

   #include "xacc.hpp"
   ...
   auto xasmCompiler = xacc::getCompiler("xasm");
   auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
       H(q[0]);
       CX(q[0], q[1]);
       CX(q[0], q[1]);
       CX(q[0], q[1]);
       Measure(q[0]);
       Measure(q[1]);
   })", nullptr);
   auto f = ir->getComposite("bell");
   assert(6 == f->nInstructions());

   auto opt = xacc::getIRTransformation("circuit-optimizer");
   opt->apply(f, nullptr);

   assert (4 == f->nInstructions());

in Python

.. code:: python

   import xacc
   ...
   # Create a bell state program with too many cnots
   xasm = xacc.getCompiler('xasm')
   ir = xasm.compile('''__qpu__ void bell(qbit q) {
   H(q[0]);
   CX(q[0],q[1]);
   CX(q[0],q[1]);
   CX(q[0], q[1]);
   Measure(q[0]);
   Measure(q[1]);
   }''')
   f = ir.getComposite('bell')
   assert(6 == f.nInstructions())

   # Run the circuit-optimizer IRTransformation
   optimizer = xacc.getIRTransformation('circuit-optimizer')
   optimizer.apply(f, None, {})

   # should have 4 instructions, not 6
   assert(4 == f.nInstructions())
   print(f.toString())


Observable
----------
The ``Observable`` concept in XACC dictates measurements to be performed
on unmeasured an ``CompositeInstruction``. XACC provides ``pauli`` and ``fermion``
``Observable`` implementations. Below we demonstrate how one might create these objects.

.. code:: cpp

   #include "xacc.hpp"
   #include "xacc_observable.hpp"
   ...
   auto x0x1 = xacc::quantum::getObservable("pauli");
   x0x1->fromString('X0 X1');

   // observe() returns a list of measured circuits
   // here we only have one
   auto measured_circuit = x0x1->observe(program)[0];

   auto fermion = xacc::getObservable("fermion");
   fermion->fromString("1^ 0");
   auto jw = xacc::getService<ObservableTransform>("jordan-wigner");
   auto spin = jw->transform(fermion);

in Python

.. code:: python

   import xacc
   ...
   x0x1 = xacc.getObservable('pauli', 'X0 X1')

   // observe() returns a list of measured circuits
   // here we only have one
   measured_circuit = x0x1.observe(program)[0]

   fermion = xacc.getObservable('fermion', '1^ 0')
   jw = xacc.getObservableTransform('jordan-wigner')
   spin = jw.transform(fermion)

Accelerator
-----------
The ``Accelerator`` is the primary interface to backend quantum computers and simulators for XACC.
The can be initialized with a heterogeneous map of input parameters, expose qubit connectivity information,
and implement execution capabilities given a valid ``AcceleratorBuffer`` and ``CompositeInstruction``.
Here we demonstrate getting reference to an ``Accelerator`` and using it to execute a simple bell state.
Note this is a full example, that leverages the ``xasm`` compiler as well as requisite C++ framework
initialization and finalization.

.. code:: cpp

   #include "xacc.hpp"
   int main(int argc, char **argv) {
     xacc::Initialize(argc, argv);

     // Get reference to the Accelerator
     auto accelerator =
       xacc::getAccelerator("local-ibm", {std::make_pair("shots", 5000)});

     // Allocate some qubits
     auto buffer = xacc::qalloc(2);

     auto xasmCompiler = xacc::getCompiler("xasm");
     auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
         H(q[0]);
         CX(q[0], q[1]);
         Measure(q[0]);
         Measure(q[1]);
     })", accelerator);

     accelerator->execute(buffer, ir->getComposites()[0]);

     buffer->print();

     xacc::Finalize();

     return 0;
   }

in Python

.. code:: python

   import xacc

   accelerator = xacc.getAccelerator('local-ibm', {'shots':5000})
   buffer = xacc.qalloc(2)
   xasm = xacc.getCompiler('xasm')
   ir = xasm.compile('''__qpu__ void bell(qbit q) {
   H(q[0]);
   CX(q[0],q[1]);
   Measure(q[0]);
   Measure(q[1]);
   }''', accelerator)

   accelerator.execute(buffer, ir.getComposites()[0])
   # note accelerators can execute lists of CompositeInstructions too
   # usefule for executing many circuits with one remote qpu call
   # accelerator.execute(buffer, ir.getComposites())

Optimizer
-------------
This abstraction is meant for the injection of general classical multi-variate function
optimization routines. XACC provides implementations leveraging NLOpt and MLPack C++ libraries.
``Optimizer``s expose an ``optimize()`` method that takes as input an ``OptFunction``, which serves as
a thin wrapper for functor-like objects exposing a specific argument structure (must take as first
arg a ``vector<double>`` representing current iterate's parameters, and another one representing the mutable
gradient vector). Below is a demonstration of how one might use this utility:

.. code:: cpp

   auto optimizer =
      xacc::getOptimizer("nlopt");

   optimizer->setOptions(
      HeterogeneousMap{std::make_pair("nlopt-maxeval", 200),
                       std::make_pair("nlopt-optimizer", "l-bfgs")});
   OptFunction f(
      [](const std::vector<double> &x, std::vector<double> &grad) {
        if (!grad.empty()) {
          grad[0] = -2 * (1 - x[0]) + 400 * (std::pow(x[0], 3) - x[1] * x[0]);
          grad[1] = 200 * (x[1] - std::pow(x[0],2));
        }
        return = 100 * std::pow(x[1] - std::pow(x[0], 2), 2) + std::pow(1 - x[0], 2);
      },
      2);

   auto result = optimizer->optimize(f);
   auto opt_val = result.first;
   auto opt_params = result.second;

or in Python

.. code:: python

   def rosen_with_grad(x):
       g = [-2*(1-x[0]) + 400.*(x[0]**3 - x[1]*x[0]), 200 * (x[1] - x[0]**2)]
       xx = (1.-x[0])**2 + 100*(x[1]-x[0]**2)**2
       return xx, g

   optimizer = xacc.getOptimizer('mlpack',{'mlpack-optimizer':'l-bfgs'})
   opt_val, opt_params = optimizer.optimize(rosen_with_grad,2)

xacc::qasm()
------------------
To improve programming efficiency, readability, and utility of the quantum kernel string
compilation, XACC exposes a ``qasm()`` function. This function takes as input an enhanced quantum
kernel source string syntax and compiles it to XACC IR. This source string is *enhanced* in that
it requires that extra metadata be present in order to adequately compile the quantum code.
Specifically, the source string must contain the following key words:

- a single *.compiler NAME*, to indicate which XACC compiler implementation to use.
- one or many *.circuit NAME* calls to give the created CompositeInstruction (circuit) a name.
- one *.parameters PARAM 1, PARAM 2, .., PARAM N* for each parameterized circuit to tell the Compiler the names of the parameters.
- A *.qbit NAME* optional keyword can be provided when the source code itself makes reference to the ``qbit`` or ``AcceleratorBuffer``

Running this command with the appropriately provided keywords will compile the source string to XACC IR and store it an
internal compilation database (standard map of CompositeInstruction names to CompositeInstructions), and users
can get reference to the individual CompositeInstructions via an exposed ``getCompiled()`` XACC API call. The
code below demonstrates how one would use ``qasm()`` and its overall utility.

.. code:: cpp

   #include "xacc.hpp"
   ...
   xacc::qasm(R"(
   .compiler xasm
   .circuit deuteron_ansatz
   .parameters x
   .qbit q
   for (int i = 0; i < 2; i++) {
     H(q[0]);
   }
   exp_i_theta(q, x, {{"pauli", "X0 Y1 - Y0 X1"}});
   )");
   auto ansatz =
     xacc::getCompiled("deuteron_ansatz");

   // Quil example, multiple kernels
   xacc::qasm(R"(.compiler quil
   .circuit ansatz
   .parameters theta, phi
   X 0
   H 2
   CNOT 2 1
   CNOT 0 1
   Rz(theta) 0
   Ry(phi) 1
   H 0
   .circuit x0x1
   ansatz(theta, phi)
   H 0
   H 1
   MEASURE 0 [0]
   MEASURE 1 [1]
   )");
   auto x0x1 = xacc::getCompiled("x0x1");

or in Python

.. code:: python

   import xacc
   ...
    xacc.qasm('''
   .compiler xasm
   .circuit deuteron_ansatz
   .parameters x
   .qbit q
   for (int i = 0; i < 2; i++) {
     X(q[0]);
   }
   exp_i_theta(q, x, {{"pauli", "X0 Y1 - Y0 X1"}});
   ''')
   ansatz =
    xacc.getCompiled('deuteron_ansatz')

   # Quil example, multiple kernels
   xacc.qasm('''.compiler quil
   .circuit ansatz
   .parameters theta, phi
   X 0
   H 2
   CNOT 2 1
   CNOT 0 1
   Rz(theta) 0
   Ry(phi) 1
   H 0
   .circuit x0x1
   ansatz(theta, phi)
   H 0
   H 1
   MEASURE 0 [0]
   MEASURE 1 [1]
   ''')
   x0x1 = xacc.getCompiled('x0x1')

Single-source Pythonic Programming
----------------------------------

Benchmarks
----------
Since XACC provides a hardware-agnostic framework
for quantum-classical computing, it is well-suited
for the development of general benchmark tasks that run
on available backend quantum computers. XACC provides a
Pythonic benchmarking tool that enables users to define
benchmarks via an input file or python dictionary, and then
distribute those files to be executed on available backends.
Benchmarks can be low-level and hardware-specific, or high-level,
application-style benchmarks.

The suite is extensible in the benchmark itself, as well
as input data required for the benchmark.

All benchmarks can be defined as INI files. These files describe
named sections of key-value pairs. Each benchmark requires an
XACC section (for the definition of the backend accelerator, number of shots, etc.) and
a Benchmark section (specifying the benchmark name and algorithm).
Other sections are specified by the concrete benchmark sub-type.

Chemistry
+++++++++
This Benchmark implementation enables one to define an application-level
benchmark which attempts to compute the accuracy with which a given
quantum backend can compute the ground state energy of a specified
electronic structure computation. Below is an example of such a benchmark
input file

.. code:: bash

    [XACC]
    accelerator = ibm:ibmq_johannesburg
    shots = 1024
    verbose = True

    [Decorators]
    readout_error = True

    [Benchmark]
    name = chemistry
    algorithm = vqe

    [Ansatz]
    source = .compiler xasm
        .circuit ansatz2
        .parameters x
        .qbit q
        X(q[0]);
        X(q[2]);
        ucc1(q, x[0]);

    [Observable]
    name = psi4
    basis = sto-3g
    geometry = 0 1
           Na  0.000000   0.0      0.0
           H   0.0        0.0  1.914388
           symmetry c1
    fo = [0, 1, 2, 3, 4, 10, 11, 12, 13, 14]
    ao = [5, 9, 15, 19]

    [Optimizer]
    name = nlopt
    nlopt-maxeval = 20

Stepping, through this, we see the benchmark is to be executed
on the IBM Johannesburg backend, with 1024 shots. Next, we specify
what benchmark algorithm to run - the Chemistry benchmark using the VQE
algorithm. After that, this benchmark
enables one to specify any AcceleratorDecorators to be used, here we
turn on readout-error decoration to correct computed expectation values
with respact to measurement readout errors. Moving down the file, one now
specifies the specific state-preparation ansatz to be used for this VQE
run, using the usual XACC qasm() format. Finally, we specify the Observable
we are interested in studying, and the classical optimizer to be used in
searching for the optimal expecation value for that observable.

One can run this benchmark with the following command (presuming it is in a
file named chem_nah_vqe_ibm.ini)

.. code:: bash

    $ python3 -m xacc --benchmark chem_nah_vqe_ibm.ini


Quantum Process Tomography
++++++++++++++++++++++++++

.. code:: bash

    [XACC]
    accelerator = ibm:ibmq_poughkeepsie
    verbose = True

    [Benchmark]
    name = qpt
    analysis = ['fidelity', 'heat-maps']
    chi-theoretical-real = [0., 0., 0., 0., 0., 1., 0., 1., 0., 0., 0., 0., 0., 1., 0., 1.]

    [Circuit]
    # Logical circuit source code
    source = .compiler xasm
        .circuit hadamard
        .qbit q
        H(q[0]);

    # Can specify physical qubit to run on
    qubit-map = [1]

