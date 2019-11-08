Extensions
==========
Here we detail concrete implementations of various XACC interfaces as well as any
input parameters they expose.

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
   url: https://q-console-api.mybluemix.net
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


Accelerator Decorators
----------------------
ROErrorDecorator
++++++++++++++++

RichExtrapDecorator
+++++++++++++++++++

RDMPurificationDecorator
++++++++++++++++++++++++

ImprovedSamplingDecorator
+++++++++++++++++++++++++

IR Transformations
------------------

CircuitOptimizer
+++++++++++++++++
