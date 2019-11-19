Benchmarking
========================
The language and hardware agnostic design of the XACC framework allows for the capability to run
general benchmarking experiments on various quantum computers and simulators. To perform benchmarking experiments,
XACC implements an additional benchmark framework that uses the XACC Python API to allow for the easy design and execution
of quantum computer benchmarking experiments. The benchmark framework is enabled automatically when XACC is installed with Python support.

Architecture
++++++++++++

The XACC Benchmark framework is implemented using the Python-based Service-Oriented Component Model (SOCM), iPOPO, and
is designed to be modular and extensible. An experimenter can easily develop and install iPOPO 'bundles'
that implement various quantum algorithms to be executed through XACC on the desired quantum processor.

To install an iPOPO bundle to the XACC benchmark framework, simply copy the bundle to the `py-plugins` directory.

.. code::

   $ cp path/to/bundle.py $(XACC_DIR)/py-plugins


If XACC was installed using ``pip``, the bundles can be installed using

.. code::

   $ cp path/to/bundle.py $(python3 -m xacc -L)/py-plugins


Algorithm
+++++++++
The iPOPO algorithm bundles used by the benchmark framework are implemented as concrete Python ``Algorithm`` subclasses.
The base ``Algorithm`` class abstracts an ``execute`` method where the algorithm is to be implemented using the XACC Python API.
The ``Algorithm`` class also abstracts an ``analyze`` method where the results of the execution can be processed and manipulated,
such as generating a `.csv` file containing the significant results.

See below for the implementation of the abstract ``Algorithm`` class.

.. code::

   from abc import abstractmethod, ABC
   class Algorithm(ABC):

      # Override this `execute` method to implement the algorithm
      # @input inputParams
      # @return buffer
      @abstractmethod
      def execute(self, inputParams):
         pass

      # Override this `analyze` method called to manipulate result data from executing the algorithm
      # @input buffer
      # @input inputParams
      @abstractmethod
      def analyze(self, buffer, inputParams):
         pass


Input
+++++
The benchmark framework is executed using an input configuration file (`.ini` extension) that contains all
of the relevant settings and configurations required to run the quantum algorithms and experiments for benchmarking quantum computers.
These input files specify information such as which quantum accelerator to target (simulated or physical hardware),
which quantum algorithm to execute, and the quantum kernel to be used in the computation.

For an example of an input file that calculates the energy of the H\ :sub:`2`\  molecule at specific angles, see below.

.. code::

   [XACC]
   accelerator: tnqvm
   algorithm: vqe-energy

   [VQE]
   initial-parameters: 3.14184629521041, 1.513865908232456

   [Ansatz]
   name: uccsd
   n-electrons: 2

   [Hamiltonian]
   hamiltonian-generator: xaccKernelH2

Execution
+++++++++
The benchmark framework is executed through the command line using the ``--benchmark`` command.
If XACC has been installed with Python support, an example of an execution of a benchmark experiment looks like:

.. code::

   $ python3 -m xacc --benchmark example_input.ini

Running XACC with a properly configured input file will initialize XACC and the benchmarking framework, read the input file, and then execute the algorithm
using the XACC configurations and parameters specified in the input. The benchmarking framework will then output a `.ab` file containing the ``AcceleratorBuffer``,
which stores the results of the execution on the QPU.

Quantum Chemistry Benchmarking
------------------------------
The input file above is an example how the XACC benchmark framework can be used to run quantum chemistry benchmarking experiments on quantum computers.
After executing the VQE algorithm in a quantum chemistry experiment such as this, two output files are generated.
The first is the standard `.ab` extension file which contains the AcceleratorBuffer that stores all of the results obtained from the QPU.
The second is a `.csv` extension file containing all of the unique parameters and calculated energies of the VQE iterations.

Hamiltonian Generators
-------------------
The ``hamiltonian-generator`` configuration in the Hamiltonian section of the example input file refers to a concrete ``HamiltonianGenerator`` Python subclass implemented as an iPOPO bundle that
is used to generate a ``PauliOperator``, modeling a spin Hamiltonian for use in XACC-VQE. The ``PauliOperator`` can be generated in a number of ways,
including (but not limited to) compiling an XACC quantum kernel source string, returning a hard-coded XACC-VQE ``PauliOperator`` Python class expression, compiling an OpenFermion ``FermionOperator``,
or utilizing the XACC-VQE method ``QubitOperator2XACC`` to compile an OpenFermion ``QubitOperator`` to ``PauliOperator``.

The ``xaccKernelH2`` ``HamiltonianGenerator`` used in this example input file compiles an XACC quantum kernel source string to the ``PauliOperator``, and its abstract ``generate`` method is shown below.

.. code::

   def generate(self, inputParams):
       src = """__qpu__ kernel() {
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
           }"""

          return vqe.compile(src)

Ansatz Generators
-----------------
In the Ansatz section of the example input file above, the ``name`` configuration refers to a concrete ``AnsatzGenerator`` Python class implemented as an iPOPO bundle that is used to
generate an XACC Intermediate Representation (IR) ``Function`` instance which expresses a composition of instructions to be executed on the desired quantum accelerator.
The ansatz can be generated utilizing the XACC ``IRGenerator`` interface or directly through the XACC ``IR`` API.

The ``uccsd`` ``AnsatzGenerator`` used in the above example uses the ``IRGenerator`` interface to return a generated
unitary coupled-cluster single and double excitation ansatz, dependent on the number of electrons in the molecule of interest and the number of qubits required to simulate it.
The abstracted `generate` method implemented by the ``uccsd`` ``AnsatzGenerator`` is shown below.

.. code::

   def generate(self, inputParams, nQubits):
       ir_generator = xacc.getIRGenerator(inputParams['name'])
       function = ir_generator.generate([int(inputParams['n-electrons']), nQubits])
       return function
