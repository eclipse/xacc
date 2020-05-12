Extensions
==========
Here we detail concrete implementations of various XACC interfaces as well as any
input parameters they expose.

Compilers
---------

xasm
++++
The XASM Compiler is the default compiler in XACC. It is the closest language to the underlying 
XACC IR data model. The XASM compiler provides a quantum assembly like language with support 
for custom Instructions and Composite Instruction generators as part of the language. Instructions 
are provided to the language via the usual XACC service registry. Most common digital gates are provided 
by default, and it is straightforward to add new Instructions. 



quilc
++++++
XACC provides a Compiler implementation that delegates to the Rigetti-developed 
quilc compiler. This is acheieved through the ``rigetti/quilc`` Docker image and 
the internal XACC REST client API. The Quilc Compiler implementation makes direct 
REST POSTs and GETs to the users local Docker Engine. Therefore, 
in order to use this Compiler, you must pull down this image. 

.. code:: bash 

   $ docker pull rigetti/quilc 

With that image pulled, you can now use the Quilc compiler via the usual XACC API calls. 

.. code:: cpp

   auto compiler = xacc::getCompiler("quilc");
   auto ir = compiler->compile(R"##(H 0
   CNOT 0 1
   )##");
   std::cout << ir->getComposites()[0]->toString() << "\n";

or in Python

.. code:: python

   compiler = xacc.getCompiler('quilc')
   ir = compiler.compile('''__qpu__ void ansatz(qbit q, double x) {
       X 0
       CNOT 1 0
       RY(x) 1
   }''')

Note that you can structure your input to the compiler as a typical XACC kernel source string 
or as a raw Quil string. 

Optimizers
----------
XACC provides implementations for the ``Optimizer`` that delegate to NLOpt and MLPack. Here we demonstrate
the various ways to configure these optimizers for a number of different solver types.

In addition to the enumerated parameters below, all ``Optimizers`` expose an ``initial-parameters`` key
that must be a list or vector of doubles with size equal to the number of parameters. By default, ``[0.,0.,...,0.,0.]`` is used.

MLPack
++++++
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
| ``mlpack-optimizer``   | Optimizer Parameter    |                  Parameter Description                          | default | type   |
+========================+========================+=================================================================+=========+========+
|        adam            | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-beta1           | Exponential decay rate for the first moment estimates.          | .7      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-beta2           | Exponential decay rate for the weighted infinity norm estimates.| .999    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-eps             | Value used to initialize the mean squared gradient parameter.   | 1e-8    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        l-bfgs          |        None            |                                                                 |         |        |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        adagrad         | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-eps             | Value used to initialize the mean squared gradient parameter.   | 1e-8    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        adadelta        | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-eps             | Value used to initialize the mean squared gradient parameter.   | 1e-8    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-rho             | Smoothing constant.                                             | .95     | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        cmaes           | mlpack-cmaes-lambda    | The population size.                                            | 0       | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        |mlpack-cmaes-upper-bound| Upper bound of decision variables.                              | 10.     | duoble |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        |mlpack-cmaes-lower-bound| Lower bound of decision variables.                              | -10.0   | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        gd              | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        momentum-sgd    | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-momentum        | Maximum absolute tolerance to terminate algorithm.              | .05     | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|   momentum-nesterov    | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-momentum        | Maximum absolute tolerance to terminate algorithm.              | .05     | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        sgd             | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        rms-prop        | mlpack-step-size       | Step size for each iteration.                                   | .5      | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-max-iter        | Maximum number of iterations allowed                            | 500000  | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-tolerance       | Maximum absolute tolerance to terminate algorithm.              | 1e-4    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-alpha           | Smoothing constant                                              | .99     | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | mlpack-eps             | Value used to initialize the mean squared gradient parameter.   | 1e-8    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+

Various examples of using the mlpack optimizer:

.. code:: cpp

   // sgd with defaults
   auto optimizer = xacc::getOptimizer("mlpack", {std::make_pair("mlpack-optimizer", "sgd")});
   // default adam
   optimizer = xacc::getOptimizer("mlpack")
   // adagrad with 30 max iters and .01 step size
   auto optimizer = xacc::getOptimizer("mlpack", {std::make_pair("mlpack-optimizer", "adagrad"),
                                                  std::make_pair("mlpack-step-size", .01),
                                                  std::make_pair("mlpack-max-iter", 30)});


or in Python

.. code:: python

   optimizer = xacc.getOptimizer('mlpack', {'mlpack-optimizer':'sgd'})
   // default adam
   optimizer = xacc.getOptimizer("mlpack")
   // adagrad with 30 max iters and .01 step size
   optimizer = xacc.getOptimizer("mlpack", {'mlpack-optimizer':'adagrad',
                                            'mlpack-step-size':.01,
                                            'mlpack-max-iter':30})
NLOpt
+++++
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
| ``nlopt-optimizer``    | Optimizer Parameter    |                  Parameter Description                          | default | type   |
+========================+========================+=================================================================+=========+========+
|        cobyla          | nlopt-ftol             | Maximum absolute tolerance to terminate algorithm.              | 1e-6    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | nlopt-maxeval          | Maximum number of iterations allowed                            | 1000    | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|        l-bfgs          | nlopt-ftol             | Maximum absolute tolerance to terminate algorithm.              |   1e-6  | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | nlopt-maxeval          | Maximum number of iterations allowed                            | 1000    | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|      nelder-mead       | nlopt-ftol             | Maximum absolute tolerance to terminate algorithm.              | 1e-6    | double |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+
|                        | nlopt-maxeval          | Maximum number of iterations allowed                            | 1000    | int    |
+------------------------+------------------------+-----------------------------------------------------------------+---------+--------+

Accelerators
------------
Here we detail all available XACC ``Accelerators`` and their exposed input parameters.

IBM
+++
The IBM Accelerator by default targets the remote ``ibmq_qasm_simulator``. You can point to a
different backend in two ways:

.. code:: cpp

   auto ibm_valencia = xacc::getAccelerator("ibm:ibmq_valencia");
   ... or ...
   auto ibm_valencia = xacc::getAccelerator("ibm", {std::make_pair("backend", "ibmq_valencia")});

in Python

.. code:: python

   ibm_valencia = xacc.getAccelerator('ibm:ibmq_valencia');
   ... or ...
   ibm_valencia = xacc.getAccelerator('ibm', {'backend':'ibmq_valencia')});

You can specify the number of shots in this way as well

.. code:: cpp

   auto ibm_valencia = xacc::getAccelerator("ibm:ibmq_valencia", {std::make_pair("shots", 2048)});

or in Python

.. code:: Python

   ibm_valencia = xacc.getAccelerator('ibm:ibmq_valencia', {'shots':2048)});

In order to target the remote backend (for ``initialize()`` or ``execute()``) you must provide
your IBM credentials to XACC. To do this add the following to a plain text file ``$HOME/.ibm_config``

.. code:: bash

   key: YOUR_KEY_HERE
   hub: HUB
   group: GROUP
   project: PROJECT

You can also create this file using the ``xacc`` Python module

.. code:: bash

   $ python3 -m xacc -c ibm -k YOUR_KEY --group GROUP --hub HUB --project PROJECT --url URL
   [ for public API ]
   $ python3 -m xacc -c ibm -k YOUR_KEY

where you provide URL, HUB, PROJECT, GROUP, and YOUR_KEY.

Aer
+++
The Aer Accelerator provides a great example of contributing plugins or extensions to core C++ XACC interfaces
from Python. To see how this is done, checkout the code `here <https://github.com/eclipse/xacc/blob/master/python/plugins/aer/aer_accelerator.py>`_.
This Accelerator connects the XACC IR infrastructure with the ``qiskit-aer`` simulator, providing a
robust simulator that can mimic noise models published by IBM backends. Note to use these noise models you must
have setup your ``$HOME/.ibm_config`` file (see above discussion on IBM Accelerator).

.. code:: python

   aer = xacc.getAccelerator('aer')
   ... or ...
   aer = xacc.getAccelerator('aer', {'shots':8192})
   ... or ...
   # For ibmq_johannesburg-like readout error
   aer = xacc.getAccelerator('aer', {'shots':2048, 'backend':'ibmq_johannesburg', 'readout_error':True})
   ... or ...
   # For all ibmq_johannesburg-like errors
   aer = xacc.getAccelerator('aer', {'shots':2048, 'backend':'ibmq_johannesburg',
                                    'readout_error':True,
                                    'thermal_relaxation':True,
                                    'gate_error':True})

You can also use this simulator from C++, just make sure you load the Python external language plugin.

.. code:: cpp

   xacc::Initialize();
   xacc::external::load_external_language_plugins();
   auto accelerator = xacc::getAccelerator("aer", {std::make_pair("shots", 8192),
                                                   std::make_pair("readout_error", true)});
   .. run simulation

   xacc::external::unload_external_language_plugins();
   xacc::Finalize();

QCS
+++
XACC provides support for the Rigetti QCS platform through the QCS Accelerator implementation. This
Accelerator requires a few extra third-party libraries that you will need to install in order
to get QCS support. Specifically we need ``libzmq``, ``cppzmq``, ``msgpack-c``, and ``uuid-dev``.
Note that more than likely this will only be built on the QCS Centos 7 VM, so the following
instructions are specifically for that OS.

.. code:: bash

   $ git clone https://github.com/zeromq/libzmq
   $ cd libzmq/ && mkdir build && cd build
   $ cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq
   $ make -j12 install

   $ cd ../..
   $ git clone https://github.com/zeromq/cppzmq
   $ cd cppzmq/ && mkdir build && cd build/
   $ cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq -DCMAKE_PREFIX_PATH=~/.zmq
   $ make -j12 install

   $ cd ../..
   $ git clone https://github.com/msgpack/msgpack-c/
   $ cd msgpack-c/ && mkdir build && cd build
   $ cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq
   $ make -j12 install
   $ cd ../..

   $ sudo yum install uuid-dev devtoolset-8-gcc devtoolset-8-gcc-c++
   $ scl enable devtoolset-8 -- bash

   [go to your xacc build directory]
   cmake .. -DUUID_LIBRARY=/usr/lib64/libuuid.so.1
   make install

There is no further configuration for using the QCS platform.

To use the QCS Accelerator targeting something like ``Aspen-4-2Q-A`` (for example, replace with your lattice):

.. code:: cpp

   auto qcs = xacc::getAccelerator("qcs:Aspen-4-2Q-A", {std::make_pair("shots", 10000)});

or in Python

.. code:: python

   qcs = xacc.getAccelerator('qcs:Aspen-4-2Q-A', {'shots':10000)});

For now you must manually map your ``CompositeInstruction`` to the correct physical bits
provided by your lattice. To do so, run

.. code:: python

   qpu = xacc.getAccelerator('qcs:Aspen-4-2Q-A')
   [given CompositeInstruction f]
   f.defaultPlacement(qpu)
   [or manually]
   f.mapBits([5,9])

IonQ
++++
The IonQ Accelerator by default targets the remote ``simulator`` backend. You can point to the physical
QPU in two ways:

.. code:: cpp

   auto ionq = xacc::getAccelerator("ionq:qpu");
   ... or ...
   auto ionq = xacc::getAccelerator("ionq", {std::make_pair("backend", "qpu")});

in Python

.. code:: python

   ionq = xacc.getAccelerator('ionq:qpu');
   ... or ...
   ionq = xacc.getAccelerator('ionq', {'backend':'qpu')});

You can specify the number of shots in this way as well

.. code:: cpp

   auto ionq = xacc::getAccelerator("ionq", {std::make_pair("shots", 2048)});

or in Python

.. code:: Python

   ionq = xacc.getAccelerator('ionq', {'shots':2048)});

In order to target the simulator or QPU (for ``initialize()`` or ``execute()``) you must provide
your IonQ credentials to XACC. To do this add the following to a plain text file ``$HOME/.ionq_config``

.. code:: bash

   key: YOUR_KEY_HERE
   url: https://api.ionq.co/v0

DWave
+++++
The DWave Accelerator by default targets the remote ``DW_2000Q_VFYC_2_1`` backend. You can point to a
different backend in two ways:

.. code:: cpp

   auto dw = xacc::getAccelerator("dwave:DW_2000Q");
   ... or ...
   auto dw = xacc::getAccelerator("dwave", {std::make_pair("backend", "DW_2000Q")});

in Python

.. code:: python

   dw = xacc.getAccelerator('dwave:DW_2000Q');
   ... or ...
   dw = xacc.getAccelerator('dwave', {'backend':'DW_2000Q')});

You can specify the number of shots in this way as well

.. code:: cpp

   auto dw = xacc::getAccelerator("dwave", {std::make_pair("shots", 2048)});

or in Python

.. code:: Python

   dw = xacc.getAccelerator('dwave', {'shots':2048)});

In order to target the remote backend (for ``initialize()`` or ``execute()``) you must provide
your DWave credentials to XACC. To do this add the following to a plain text file ``$HOME/.dwave_config``

.. code:: bash

   key: YOUR_KEY_HERE
   url: https://cloud.dwavesys.com

You can also create this file using the ``xacc`` Python module

.. code:: bash

   $ python3 -m xacc -c dwave -k YOUR_KEY

where you provide YOUR_KEY.

DWave Neal
++++++++++
The DWave Neal Accelerator provides another example of contributing plugins or extensions to core C++ XACC interfaces
from Python. To see how this is done, checkout the code `here <https://github.com/eclipse/xacc/blob/master/python/plugins/dwave/dwave_neal_accelerator.py>`_.
This Accelerator connects the XACC IR infrastructure with the ``dwave-neal`` simulator, providing a local
simulator that can mimic DWave QPU execution.

.. code:: python

   aer = xacc.getAccelerator('dwave-neal')
   ... or ...
   aer = xacc.getAccelerator('dwave-neal', {'shots':2000})

You can also use this simulator from C++, just make sure you load the Python external language plugin.

.. code:: cpp

   xacc::Initialize();
   xacc::external::load_external_language_plugins();
   auto accelerator = xacc::getAccelerator("dwave-neal", {std::make_pair("shots", 8192)});
   .. run simulation

   xacc::external::unload_external_language_plugins();
   xacc::Finalize();

QuaC
++++
The `QuaC <https://github.com/ORNL-QCI/QuaC/tree/xacc-integration>`_ accelerator is a pulse-level accelerator (simulation only) that can execute quantum circuits at both gate and pulse (analog) level.

To use this accelerator, you need to build and install QuaC (see `here <https://github.com/ORNL-QCI/QuaC/tree/xacc-integration#build-xacc-quac-accelerator-plugin>`_ for instructions.) 

In pulse mode, you need to provide the QuaC accelerator a dynamical system model 
which can be constructed from an OpenPulse-format Hamiltonian JSON:

.. code:: python

   hamiltonianJson = {
      "description": "Hamiltonian of a one-qubit system.\n",
      "h_str": ["-0.5*omega0*Z0", "omegaa*X0||D0"],
      "osc": {},
      "qub": {
         "0": 2
      },
      "vars": {
         "omega0": 6.2831853,
         "omegaa": 0.0314159
      } 
   }
   # Create a pulse system model object 
   model = xacc.createPulseModel()
   # Load the Hamiltonian JSON (string) to the system model
   loadResult = model.loadHamiltonianJson(json.dumps(hamiltonianJson))

The QuaC simulator can then be requested by

.. code:: python

   qpu = xacc.getAccelerator('QuaC', {'system-model': model.name()})

Pulse-level instructions can be constructed manually (assigning sample points)

.. code:: python

   pulseData = np.ones(pulseLength)
   # Register the pulse named 'square' as an XACC instruction
   xacc.addPulse('square', pulseData) 
   provider = xacc.getIRProvider('quantum')
   squarePulseInst = provider.createInstruction('square', [0])
   squarePulseInst.setChannel('d0')
   # This instruction can be added to any XACC quantum Composite Instruction
   prog.addInstruction(squarePulseInst)

or automatically (converting from quantum gates to pulses). 
To use automatic gate-to-pulse functionality, we need to load a pulse library to the accelerator as follows:

.. code:: python

   # Load the backend JSON file which contains a pulse library
   backendJson = open('backends.json', 'r').read()
   qpu.contributeInstructions(backendJson) 

For more information, please check out these `examples <https://github.com/ORNL-QCI/QuaC/tree/xacc-integration/xacc_examples/python>`_.

Algorithms
----------
XACC exposes hybrid quantum-classical Algorithm implementations for the variational quantum eigensolver (VQE), data-driven
circuit learning (DDCL), and chemistry reduced density matrix generation (RDM).

VQE
+++
The VQE Algorithm requires the following input information:

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    observable          | The hermitian operator, vqe computes ground eigenvalue of this  | std::shared_ptr<Observable>          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    ansatz              | The unmeasured, parameterized quantum circuit                   | std::shared_ptr<CompositeInstruction>|
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    optimizer           | The classical optimizer to use                                  | std::shared_ptr<Optimizer>           |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    accelerator         | The Accelerator backend to target                               | std::shared_ptr<Accelerator>         |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

This Algorithm will add ``opt-val`` (``double``) and ``opt-params`` (``std::vector<double>``) to the provided ``AcceleratorBuffer``.
The results of the algorithm are therefore retrieved via these keys (see snippet below). Note you can
control the initial VQE parameters with the ``Optimizer`` ``initial-parameters`` key (by default all zeros).

.. code:: cpp

   #include "xacc.hpp"
   #include "xacc_observable.hpp"

   int main(int argc, char **argv) {
     xacc::Initialize(argc, argv);

     // Get reference to the Accelerator
     // specified by --accelerator argument
     auto accelerator = xacc::getAccelerator();

     // Create the N=2 deuteron Hamiltonian
     auto H_N_2 = xacc::quantum::getObservable(
         "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

     auto optimizer = xacc::getOptimizer("nlopt",
                            {std::make_pair("initial-parameters", {.5})});

     // JIT map Quil QASM Ansatz to IR
     xacc::qasm(R"(
    .compiler xasm
    .circuit deuteron_ansatz
    .parameters theta
    .qbit q
    X(q[0]);
    Ry(q[1], theta);
    CNOT(q[1],q[0]);
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz");

    // Get the VQE Algorithm and initialize it
    auto vqe = xacc::getAlgorithm("vqe");
    vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", H_N_2),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("optimizer", optimizer)});

    // Allocate some qubits and execute
    auto buffer = xacc::qalloc(2);
    vqe->execute(buffer);

    auto ground_energy = (*buffer)["opt-val"].as<double>();
    auto params = (*buffer)["opt-params"].as<std::vector<double>>();
  }

In Python:

.. code:: python

   import xacc

   # Get access to the desired QPU and
   # allocate some qubits to run on
   qpu = xacc.getAccelerator('tnqvm')
   buffer = xacc.qalloc(2)

   # Construct the Hamiltonian as an XACC-VQE PauliOperator
   ham = xacc.getObservable('pauli', '5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1')


   xacc.qasm('''.compiler xasm
   .circuit ansatz2
   .parameters t0
   .qbit q
   X(q[0]);
   Ry(q[1],t0);
   CX(q[1],q[0]);
   ''')
   ansatz2 = xacc.getCompiled('ansatz2')

   opt = xacc.getOptimizer('nlopt', {'initial-parameters':[.5]})

   # Create the VQE algorithm
   vqe = xacc.getAlgorithm('vqe', {
                        'ansatz': ansatz2,
                        'accelerator': qpu,
                        'observable': ham,
                        'optimizer': opt
                        })
   vqe.execute(buffer)
   energy = buffer['opt-val']
   params = buffer['opt-params']


DDCL
++++
The DDCL Algorithm implements the following algorithm - given a target probability distribution,
propose a parameterized quantum circuit and train (minimize loss) the circuit to reproduce
that given target distribution. We design DDCL to be extensible in loss function computation and
gradient computation strategies.

The DDCL Algorithm requires the following input information:

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    target_dist         | The target probability distribution to reproduce                | std::vector<double>                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    ansatz              | The unmeasured, parameterized quantum circuit                   | std::shared_ptr<CompositeInstruction>|
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    optimizer           | The classical optimizer to use, can be gradient based           | std::shared_ptr<Optimizer>           |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    accelerator         | The Accelerator backend to target                               | std::shared_ptr<Accelerator>         |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    loss                | The loss strategy to use                                        |          std::string                 |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    gradient            | The gradient strategy to use                                    |  std::string                         |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

As of this writing, loss can take ``js`` and ``mmd`` values for Jansen-Shannon divergence and Maximum Mean Discrepancy, respectively.
More are being added. Also, gradient can take ``js-parameter-shift`` and ``mmd-parameter-shift`` values. These gradient
strategies will shift each parameter by plus or minus pi over 2.

.. code:: cpp

   #include "xacc.hpp"

   int main(int argc, char **argv) {
     xacc::Initialize(argc, argv);

     xacc::external::load_external_language_plugins();
     xacc::set_verbose(true);

     // Get reference to the Accelerator
     auto accelerator = xacc::getAccelerator("aer");

     auto optimizer = xacc::getOptimizer("mlpack");
     xacc::qasm(R"(
    .compiler xasm
    .circuit qubit2_depth1
    .parameters x
    .qbit q
    U(q[0], x[0], -pi/2, pi/2 );
    U(q[0], 0, 0, x[1]);
    U(q[1], x[2], -pi/2, pi/2);
    U(q[1], 0, 0, x[3]);
    CNOT(q[0], q[1]);
    U(q[0], 0, 0, x[4]);
    U(q[0], x[5], -pi/2, pi/2);
    U(q[1], 0, 0, x[6]);
    U(q[1], x[7], -pi/2, pi/2);
    )");
     auto ansatz = xacc::getCompiled("qubit2_depth1");

     std::vector<double> target_distribution {.5, .5, .5, .5};

     auto ddcl = xacc::getAlgorithm("ddcl");
     ddcl->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("target_dist", target_distribution),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("loss", "js"),
                   std::make_pair("gradient", "js-parameter-shift"),
                   std::make_pair("optimizer", optimizer)});

     // Allocate some qubits and execute
     auto buffer = xacc::qalloc(2);
     ddcl->execute(buffer);

     // Print the result
     std::cout << "Loss: " << buffer["opt-val"].as<double>()
            << "\n";

     xacc::external::unload_external_language_plugins();
     xacc::Finalize();
   }

or in Python

.. code:: python

   import xacc
   # Get the QPU and allocate a single qubit
   qpu = xacc.getAccelerator('aer')
   qbits = xacc.qalloc(1)

   # Get the MLPack Optimizer, default is Adam
   optimizer = xacc.getOptimizer('mlpack')

   # Create a simple quantum program
   xacc.qasm('''
   .compiler xasm
   .circuit foo
   .parameters x,y,z
   .qbit q
   Ry(q[0], x);
   Ry(q[0], y);
   Ry(q[0], z);
   ''')
   f = xacc.getCompiled('foo')

   # Get the DDCL Algorithm, initialize it
   # with necessary parameters
   ddcl = xacc.getAlgorithm('ddcl', {'ansatz': f,
                                  'accelerator': qpu,
                                  'target_dist': [.5,.5],
                                  'optimizer': optimizer,
                                  'loss': 'js',
                                  'gradient': 'js-parameter-shift'})
   # execute
   ddcl.execute(qbits)

   print(qbits.keys())
   print(qbits['opt-val'])
   print(qbits['opt-params'])

Rotoselect
++++++++++
The ``Rotoselect`` Quantum Circuit Structure Learning Algorithm (`Ostaszewski et al. (2019) <https://arxiv.org/abs/1905.09692>`_) requires the following input information:

+------------------------+------------------------------------------------------------------------+------------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                                 |             type                         |
+========================+========================================================================+==========================================+
|    observable          | The hermitian operator, Rotoselect computes ground eigenvalue of this  | std::shared_ptr<Observable>/Observable*  |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    layers              | Number of circuit layers. Each layer consists of parametrized          | int                                      |
|                        | single-qubit rotations followed by a ladder of controlled-Z gates.     |                                          |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    iterations          | The number of training iterations                                      | int                                      |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    accelerator         | The Accelerator backend to target                                      | std::shared_ptr<Accelerator>/Accelerator*|
+------------------------+------------------------------------------------------------------------+------------------------------------------+

This ``Rotoselect`` algorithm is designed to learn a good circuit structure
(generators of rotation are selected from the set of Pauli gates)
at fixed depth (``layers``) to minimize the cost function (``observable``).
This Algorithm will add ``opt-val`` (``double``) to the provided ``AcceleratorBuffer``.
The result of the algorithm is therefore retrieved via this key (see snippet below).

.. code:: cpp
  #include "xacc.hpp"
  #include "xacc_observable.hpp"
  #include "xacc_service.hpp"

  int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);
    // Use the Qpp simulator as the accelerator
    auto acc = xacc::getAccelerator("qpp");

    // In this example, we try to find the ground state energy of the 5-qubit Heisenberg model on a
    // 1D lattice with periodic boundary conditions.
    auto buffer = xacc::qalloc(5);
    // The corresponding Hamiltonian is:
    // Note: we set all scalar params to 1.0 for simplicity.
    auto observable = xacc::quantum::getObservable(
        "pauli",
        std::string("X0 X1 + X1 X2 + X2 X3 + X3 X4 + X4 X0 + "
                    "Y0 Y1 + Y1 Y2 + Y2 Y3 + Y3 Y4 + Y4 Y0 +"
                    "Z0 Z1 + Z1 Z2 + Z2 Z3 + Z3 Z4 + Z4 Z0 + "
                    "Z0 + Z1 + Z2 + Z3 + Z4"));

    auto rotoselect = xacc::getService<xacc::Algorithm>("rotoselect");
    // We train the circuit that has 6 layers for 1000 iterations.
    // Note: we can repeat this training multiple times to collect the mean and standard deviation
    // as reported in https://arxiv.org/abs/1905.09692
    rotoselect->initialize({
                            std::make_pair("accelerator", acc),
                            std::make_pair("observable", observable),
                            std::make_pair("layers", 6),
                            std::make_pair("iterations", 1000),
                          });

    rotoselect->execute(buffer);
    // Expected value ~ -8.4 -> -8.5
    // Ref: FIG. 2 of https://arxiv.org/abs/1905.09692
    std::cout << "Energy: " << (*buffer)["opt-val"].as<double>() << "\n";
  }


RBM Classification
++++++++++++++++++
The ``rbm_classification`` algorithm provides an implementation that trains a
restricted boltzmann machine via sampling of a quantum annealer for the purpose of
classification. (`Caldeira et al. (2019) <https://arxiv.org/abs/1911.06259>`_)
It exposes the following input information:

+------------------------+------------------------------------------------------------------------+------------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                                 |             type                         |
+========================+========================================================================+==========================================+
|    nv                  | The number of visible units                                            | int                                      |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    nh                  | The number of hidden units                                             | int                                      |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    batch-size          | The batch size, defaults to 1                                          | int                                      |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    n-gibbs-steps       | The number of gibbs steps to use in post-processing of dwave data      | int                                      |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    train-steps         | Hard-code the number of training iterations/steps, by default this is  | int                                      |
|                        | set to -1, meaning unlimited iterations                                |                                          |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    epochs              | The number of training epochs, defaults to 1                           | int                                      |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    train-file          | The location (relative to pwd) of the training data (as npy file)      | string                                   |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|  expectation-strategy  | Strategy to use in computing model expectation values, can be gibbs,   | string                                   |
|                        | quantum-annealing, discriminative, or cd                               |                                          |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    backend             | The desired quantum-annealing backend (defaults to dwave-neal), can be | string                                   |
|                        | any of the available D-Wave backends, must be provided as dwave:BEND   |                                          |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    shots               | The number of samples to draw from the dwave backend                   | int                                      |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    embedding           | The minor graph embedding to use, if not provided, one will be         |                                          |
|                        | computed and used for subsequent calls to the dwave backend.           | map<int, vector<int>>                    |
+------------------------+------------------------------------------------------------------------+------------------------------------------+

Example usage in Python:

.. code:: python

    import xacc

    # Create the RBM Classification algorithm
    algo = xacc.getAlgorithm('rbm-classification',
                {
                'nv':64,
                'nh':64,
                'train-file':'sg_train_64bits.npy',
                'expectation-strategy':'quantum-annealing',
                'backend':'dwave:DW_2000Q_5',
                'shots':100,
                })

    qbits = xacc.qalloc()
    algo.execute(qbits)

    # get the trained RBM weights
    # for further use and post-processing
    w = qbits['w']
    bv = qbits['bv']
    bh = qbits['bh']

Quantum Process Tomography
++++++++++++++++++++++++++
The ``qpt`` algorithm provides an implementation of Algorithm that uses linear
inversion to compute the chi process matrix for a desired circuit.

+------------------------+------------------------------------------------------------------------+------------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                                 |             type                         |
+========================+========================================================================+==========================================+
|    circuit             | The circuit to characterize                                            | pointer-like CompositeInstruction        |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    accelerator         | The backend quantum computer to use                                    | pointer-like Accelerator                 |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    qubit-map           | The physical qubits to map the logical circuit onto                    | vector<int>                              |
+------------------------+------------------------------------------------------------------------+------------------------------------------+


.. code:: cpp

   #include "xacc.hpp"

   int main(int argc, char **argv) {
     xacc::Initialize(argc, argv);
     auto acc = xacc::getAccelerator("ibm::ibmq_poughkeepsie");

     auto compiler = xacc::getCompiler("xasm");
     auto ir = compiler->compile(R"(__qpu__ void f(qbit q) {
         H(q[0]);
     })", nullptr);
     auto h = ir->getComposite("f");

     auto qpt = xacc::getAlgorithm("qpt", {
                             std::make_pair("circuit", h),
                             std::make_pair("accelerator", acc)
                             });

     auto buffer = xacc::qalloc(1);
     qpt->execute(buffer);

     auto chi_real = (*buffer)["chi-real"];
     auto chi_imag = (*buffer)["chi-imag"];

   }

or in Python

.. code:: python

   import xacc
   # Choose the QPU on which to
   # characterize the process matrix for a Hadamard
   qpu = xacc.getAccelerator('ibm:ibmq_poughkeepsie')

   # Create the CompositeInstruction containing a
   # single Hadamard instruction
   provider = xacc.getIRProvider('quantum')
   circuit = provider.createComposite('U')
   hadamard = provider.createInstruction('H', [0])
   circuit.addInstruction(hadamard)

   # Create the Algorithm, give it the circuit
   # to characterize and the backend to target
   qpt = xacc.getAlgorithm('qpt', {'circuit':circuit, 'accelerator':qpu})

   # Allocate a qubit, this will
   # store our tomography results
   buffer = xacc.qalloc(1)

   # Execute
   qpt.execute(buffer)

   # Compute the fidelity with respect to
   # the ideal hadamard process
   F = qpt.calculate('fidelity', buffer, {'chi-theoretical-real':[0., 0., 0., 0., 0., 1., 0., 1., 0., 0., 0., 0., 0., 1., 0., 1.]})
   print('\nFidelity: ', F)


Accelerator Decorators
----------------------
ROErrorDecorator
++++++++++++++++
The ``ROErrorDecorator`` provides an ``AcceleratorDecorator`` implementation for affecting
readout error mitigation as in the `deuteron paper <https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.120.210501>`_.
It takes as input readout error probabilities ``p(0|1)`` and ``p(1|0)`` for all qubits and shifts expecation values
accordingly (see paper).

By default it will request the backend properties from the decorated ``Accelerator`` (``Accelerator::getProperties()``). This method
returns a ``HeterogeneousMap``. If this map contains a vector of doubles at keys ``p01s`` and ``p10s``, then these
values will be used in the readout error correction. Alternatively, if the backend does not provide this data,
users can provide a custom JSON file containing the probabilities. This file should be structured as such

.. code:: bash

   {
       "shots": 1024,
       "backend": "qcs:Aspen-2Q-A",
       "0": {
           "0|1": 0.0565185546875,
           "1|0": 0.0089111328125,
           "+": 0.0654296875,
           "-": 0.047607421875
       },
       "1": {
           "0|1": 0.095458984375,
           "1|0": 0.0115966796875,
           "+": 0.1070556640625,
           "-": 0.0838623046875
       }
   }

Automating readout error mitigation with this decorator can be done in the following way:

.. code:: python

   qpu = xacc.getAccelerator('ibm:ibmq_johannesburg', {'shots':1024})

   # Turn on readout error correction by decorating qpu
   qpu = xacc.getAcceleratorDecorator('ro-error', qpu)

   # Now use qpu as your Accelerator...
   # execution will be automatically readout
   # error corrected

Similarly, with a provided configuration file

.. code:: cpp

   auto qpu = xacc::getAccelerator("qcs:Aspen-2Q-A");
   qpu = xacc::getAcceleratorDecorator("ro-error", qpu, {std::make_pair("file", "probs.json")});

See `readout_error_correction_aer.py <https://github.com/eclipse/xacc/blob/master/python/examples/readout_error_correction_aer.py>`_
for a full example demonstrating the utility of the ``ROErrorDecorator``.

RDMPurificationDecorator
++++++++++++++++++++++++

ImprovedSamplingDecorator
+++++++++++++++++++++++++

VQE Restart Decorator
+++++++++++++++++++++


IR Transformations
------------------

CircuitOptimizer
+++++++++++++++++
This ``IRTransformation`` of type ``Optimization`` will search the DAG representation
of a quantum circuit and remove all zero-rotations, hadamard and cnot pairs, and merge
adjacent common rotations (e.g. ``Rx(.1)Rx(.1) -> Rx(.2)``).

.. code:: python

   # Create a bell state program with too many cnots
   xacc.qasm('''
   .compiler xasm
   .circuit foo
   .qbit q
   H(q[0]);
   CX(q[0], q[1]);
   CX(q[0], q[1]);
   CX(q[0], q[1]);
   Measure(q[0]);
   Measure(q[1]);
   ''')
   f = xacc.getCompiled('foo')
   assert(6 == f.nInstructions())

   # Run the circuit-optimizer IRTransformation, can pass
   # accelerator (here None) and options (here empty dict())
   optimizer = xacc.getIRTransformation('circuit-optimizer')
   optimizer.apply(f, None, {})

   # should have 4 instructions, not 6
   assert(4 == f.nInstructions())


Observables
-----------

Psi4 Frozen-Core
++++++++++++++++
The ``psi4-frozen-core`` observable generates an fermionic
observable using Psi4 and based on a user provided dictionary of options.
To use this Observable, ensure you have Psi4 installed under the same
``python3`` used for the XACC Python API.

.. code:: bash

   $ git clone https://github.com/psi4/psi4 && cd psi4 && mkdir build && cd build
   $ cmake .. -DPYTHON_EXECUTABLE=$(which python3) -DCMAKE_INSTALL_PREFIX=$(python3 -m site --user-site)/psi4
   $ make -j8 install
   $ export PYTHONPATH=$(python3 -m site --user-site)/psi4/lib:$PYTHONPATH

This observable type takes a dictionary of options describing the
molecular geometry (key ``geometry``), the basis set (key ``basis``),
and the list of frozen (key ``frozen-spin-orbitals``) and active (key ``active-spin-orbitals``) spin
orbital lists.

With Psi4 and XACC installed, you can use the frozen-core
Observable in the following way in python.

.. code:: python

   import xacc

   geom = '''
   0 1
   Na  0.000000   0.0      0.0
   H   0.0        0.0  1.914388
   symmetry c1
   '''
   fo = [0, 1, 2, 3, 4, 10, 11, 12, 13, 14]
   ao = [5, 9, 15, 19]

   H = xacc.getObservable('psi4-frozen-core', {'basis': 'sto-3g',
                                       'geometry': geom,
                                       'frozen-spin-orbitals': fo,
                                       'active-spin-orbitals': ao})

Circuit Generator
-----------------

ASWAP Ansatz Circuit
++++++++++++++++++++
The ``ASWAP`` circuit generator generates a state preparation (ansatz) circuit for the VQE Algorithm.
(See `Gard, Bryan T., et al. <https://arxiv.org/abs/1904.10910>`_)

The ``ASWAP`` circuit generator requires the following input information:

+------------------------+------------------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                                 |             type                     |
+========================+========================================================================+======================================+
|  nbQubits              | The number of qubits in the circuit.                                   | int                                  |
+------------------------+------------------------------------------------------------------------+--------------------------------------+
|  nbParticles           | The number of particles.                                               | int                                  |
+------------------------+------------------------------------------------------------------------+--------------------------------------+
|  timeReversalSymmetry  | Do we have time-reversal symmetry?                                     | boolean                              |
+------------------------+------------------------------------------------------------------------+--------------------------------------+

Example:

.. code:: cpp

  #include "xacc.hpp"
  #include "xacc_observable.hpp"
  #include "xacc_service.hpp"

  int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);

    auto accelerator = xacc::getAccelerator("qpp");
    auto H_N_2 = xacc::quantum::getObservable(
        "pauli", std::string("5.907 - 2.1433 X0X1 "
                            "- 2.1433 Y0Y1"
                            "+ .21829 Z0 - 6.125 Z1"));

    auto optimizer = xacc::getOptimizer("nlopt");
    // Use the ASWAP circuit as the ansatz
    xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz
        .parameters t0
        .qbit q
        ASWAP(q, t0, {{"nbQubits", 2}, {"nbParticles", 1}});
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz");

    // Get the VQE Algorithm and initialize it
    auto vqe = xacc::getAlgorithm("vqe");
    vqe->initialize({std::make_pair("ansatz", ansatz),
                    std::make_pair("observable", H_N_2),
                    std::make_pair("accelerator", accelerator),
                    std::make_pair("optimizer", optimizer)});

    // Allocate some qubits and execute
    auto buffer = xacc::qalloc(2);
    vqe->execute(buffer);
    // Expected result: -1.74886
    std::cout << "Energy: " << (*buffer)["opt-val"].as<double>() << "\n";
   }