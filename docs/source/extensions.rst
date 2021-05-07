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

TNQVM
+++++
`TNQVM <https://github.com/ORNL-QCI/tnqvm>`_ provides an Accelerator implementation that leverages tensor network theory to simulate quantum circuits.

TNQVM implements a few tensor-based quantum circuit simulators that can be specified by the `tnqvm-visitor` configuration key.
The list of visitors and their descriptions are:

+--------------------+------------------------------------------------------------------------+
|   `tnqvm-visitor`  |                  Description                                           |    
+====================+========================================================================+
|    `itensor-mps`   | MPS simulator based on itensor library.                                | 
+--------------------+------------------------------------------------------------------------+
|    `exatn`         | Full tensor contraction simulator based on ExaTN library.              |  
|                    | Tensor element type (`float` or `double`) can also specified           |    
|                    | after a '`:`' separator,                                               |
|                    | e.g., `exatn:double` (default) or `exatn:float`                        |    
+--------------------+------------------------------------------------------------------------+
|    `exatn-mps`     | MPS simulator based on ExaTN library.                                  |    
+--------------------+------------------------------------------------------------------------+
|    `exatn-pmps`    | Purified-MPS (density matrix) simulator based on ExaTN library.        |   
+--------------------+------------------------------------------------------------------------+

.. code:: cpp

   auto qpu = xacc::getAccelerator("tnqvm", {{"tnqvm-visitor", "exatn"}});


For the `exatn` simulator, there are additional options that users can set during initialization:

+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|  Initialization Parameter   |                  Parameter Description                                 |    type     |         default          |
+=============================+========================================================================+=============+==========================+
| exatn-buffer-size-gb        | ExaTN's host memory buffer size (in GB)                                |    int      | 8 (GB)                   |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| exatn-contract-seq-optimizer| ExaTN's contraction sequence optimizer to use.                         |    string   | metis                    |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| calc-contract-cost-flops    | Estimate the Flops and Memory requirements only (no tensor contraction)|    bool     | false                    |
|                             |                                                                        |             |                          |
|                             | If true, the following info will be added to the AcceleratorBuffer:    |             |                          |
|                             |                                                                        |             |                          |
|                             | - `contract-flops`: Flops count.                                       |             |                          |
|                             |                                                                        |             |                          |
|                             | - `max-node-bytes`: Max intermediate tensor size in memory.            |             |                          |
|                             |                                                                        |             |                          |
|                             | - `optimizer-elapsed-time-ms`: optimization walltime.                  |             |                          |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| bitstring                   | If provided, the output amplitude/partial state vector associated with | vector<int> | <unused>                 |
|                             | that `bitstring` will be computed.                                     |             |                          |
|                             |                                                                        |             |                          |
|                             | The length of the input `bitstring` must match the number of qubits.   |             |                          |
|                             | Non-projected bits (partial state vector) are indicated by `-1` values.|             |                          |
|                             |                                                                        |             |                          |
|                             | Returned values in the AcceleratorBuffer:                              |             |                          |
|                             |                                                                        |             |                          |
|                             | - `amplitude-real`/`amplitude-real-vec`: Real part of the result.      |             |                          |
|                             |                                                                        |             |                          |
|                             | - `amplitude-imag`/`amplitude-imag-vec`: Imaginary part of the result. |             |                          |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| contract-with-conjugate     | If true, we append the conjugate of the input circuit.                 |    bool     | false                    |
|                             | This is used to validate internal tensor contraction.                  |             |                          |
|                             | `contract-with-conjugate-result` key in the AcceleratorBuffer will be  |             |                          |
|                             | set to `true` if the validation is successful.                         |             |                          |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| mpi-communicator            | The MPI communicator to initialize ExaTN runtime with.                 |    void*    | <unused>                 |
|                             | If not provided, by default, ExaTN will use `MPI_COMM_WORLD`.          |             |                          |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| exp-val-by-conjugate        | If true, the expectation value of *large* circuits (exceed memory      |    bool     | false                    |
|                             | limit) is computed by closing the tensor network with its conjugate.   |             |                          |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+

For the `exatn-mps` simulator, there are additional options that users can set during initialization:

+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|  Initialization Parameter   |                  Parameter Description                                 |    type     |         default          |
+=============================+========================================================================+=============+==========================+
| svd-cutoff                  | SVD cut-off limit.                                                     |    double   | numeric_limits::min      |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| max-bond-dim                | Max bond dimension to keep.                                            |    int      | no limit                 |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| mpi-communicator            | The MPI communicator to initialize ExaTN runtime with.                 |    void*    | <unused>                 |
|                             | If not provided, by default, ExaTN will use `MPI_COMM_WORLD`.          |             |                          |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+

For the `exatn-pmps` simulator, there are additional options that users can set during initialization:

+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|  Initialization Parameter   |                  Parameter Description                                 |    type     |         default          |
+=============================+========================================================================+=============+==========================+
| backend-json                | Backend configuration JSON to estimate the noise model from.           |    string   | None                     |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
| backend                     | Name of the IBMQ backend to query the backend configuration.           |    string   | None                     |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+

If either `backend-json` or `backend` is provided, the `exatn-pmps` simulator will simulate the backend noise associated with each quantum gate.

Atos QLM
++++++++

XACC provides an Accelerator interface to the Atos QLM simulator (plugin name: "atos-qlm") via the QLMaaS (QLM-as-a-service) API.
Users need to have a remote account (QLMaaS) to use this functionality.

Prerequisites: ``myqlm`` and ``qlmaas`` Python packages on your local machine.
These packages can be installed via pip.

Connection configuration set-up:

- Method 1 (**recommended**): create a file named `.qlm_config` at the `$HOME` directory with the following information. 
Please note that the host address can be changed to access other QLM machines if you have access to them.

.. code::

 host: quantumbull.ornl.gov
 username: <your QLM user name>
 password: <your QLM password>

- Method 2: provide the "username" and "password" fields when retrieving the qlm Accelerator, e.g.,

.. code:: cpp

  auto qlm = xacc::getAccelerator("atos-qlm", {{"username", "<your QLM user name>"}, {"password", "<your QLM password>"}});


- Method 3: reusing your QLMaaS configuration if you have one.

For example, you have created the configuration using the Python `qlmaas` package, which is saved to `$HOME/.qlmaas/config.ini`. In this case, you don't need to provide any additional information. It should just work.

Here we list all configurations that this Accelerator supports.

+------------------------+---------------------------------------------------------------------------+--------------------------------------+
|  Parameter             |                  Parameter Description                                    |             type                     |
+========================+===========================================================================+======================================+
|    sim-type            | Name of the simulator. Options: LinAlg (default), MPS, Feynman, Bdd       | std::string                          |
+------------------------+---------------------------------------------------------------------------+--------------------------------------+
|    shots               | Number of measurement shots.                                              | int                                  |
+------------------------+---------------------------------------------------------------------------+--------------------------------------+
|    mps-threshold       | [MPS] SVD truncation threshold.                                           | double                               |
+------------------------+---------------------------------------------------------------------------+--------------------------------------+
|    max-bond            | [MPS] Max bond dimension.                                                 | int                                  |
+------------------------+---------------------------------------------------------------------------+--------------------------------------+
|    threads             | Number of threads to use.                                                 | int                                  |
+------------------------+---------------------------------------------------------------------------+--------------------------------------+

.. code:: python

   qpu = xacc.getAccelerator('atos-qlm', {'sim-type': 'MPS', 'max-bond': 512})

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

Qrack
+++++
The `vm6502q/qrack <https://github.com/vm6502q/qrack>`_ simulator-based accelerator provides optional OpenCL-based GPU acceleration, as well as a novel simulator optimization layer.

.. code:: cpp

   auto qrk = xacc::getAccelerator("qrack", {std::make_pair("shots", 2048)});

By default, it selects initialization parameters that are commonly best for a wide range of use cases. However, it is highly configurable through a number of exposed parameters:

+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|  Initialization Parameter   |                  Parameter Description                                 |    type     |         default          |
+=============================+========================================================================+=============+==========================+
|    shots                    | Number of iterations to repeat the circuit for                         |    int      | -1 (Z-expectation only)  |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|    use_opencl               | Use OpenCL acceleration if available, (otherwise native C++11)         |    bool     | true                     |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|    use_qunit                | Turn on the novel optimization layer, (otherwise "Schrödinger method") |    bool     | true                     |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|    device_id                | The (Qrack) device ID number of the OpenCL accelerator to use          |    int      | -1 (auto-select)         |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|    do_normalize             | Enable small norm probability amplitude flooring and normalization     |    bool     | true                     |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+
|    zero_threshold           | Norm threshold for clamping probability amplitudes to 0                |    double   | 1e-14/1e-30 float/double |
+-----------------------------+------------------------------------------------------------------------+-------------+--------------------------+

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

QAOA
++++
The QAOA Algorithm requires the following input information:

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    observable          | The hermitian operator represents the cost Hamiltonian.         | std::shared_ptr<Observable>          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    optimizer           | The classical optimizer to use                                  | std::shared_ptr<Optimizer>           |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    accelerator         | The Accelerator backend to target                               | std::shared_ptr<Accelerator>         |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    steps               | The number of timesteps. Corresponds to 'p' in the literature.  | int                                  |
|                        | This is optional, default = 1 if not provided.                  |                                      |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    parameter-scheme    | The QAOA parameterization scheme ('Extended' or 'Standard').    | string                               |
|                        | This is optional, default = 'Extended' if not provided.         |                                      |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    graph               | The MaxCut graph problem.                                       | std::shared_ptr<Graph>               |
|                        | If provided, the cost Hamiltonian is constructed automatically. |                                      |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

This Algorithm will add ``opt-val`` (``double``) and ``opt-params`` (``std::vector<double>``) to the provided ``AcceleratorBuffer``.
The results of the algorithm are therefore retrieved via these keys (see snippet below). Note you can
control the initial QAOA parameters with the ``Optimizer`` ``initial-parameters`` key (by default all zeros).

.. code:: cpp

   #include "xacc.hpp"
   #include "xacc_observable.hpp"
   #include "xacc_service.hpp"
   #include <random>

   // Use XACC built-in QAOA to solve a QUBO problem
   // QUBO function:
   // y = -5x1 - 3x2 - 8x3 - 6x4 + 4x1x2 + 8x1x3 + 2x2x3 + 10x3x4
   int main(int argc, char **argv) {
      xacc::Initialize(argc, argv);
      // Use the Qpp simulator as the accelerator
      auto acc = xacc::getAccelerator("qpp");
      
      auto buffer = xacc::qalloc(4);
      // The corresponding QUBO Hamiltonian is:
      auto observable = xacc::quantum::getObservable(
            "pauli",
            std::string("-5.0 - 0.5 Z0 - 1.0 Z2 + 0.5 Z3 + 1.0 Z0 Z1 + 2.0 Z0 Z2 + 0.5 Z1 Z2 + 2.5 Z2 Z3"));
      
      const int nbSteps = 12;
      const int nbParams = nbSteps*11;
      std::vector<double> initialParams;
      std::random_device rd;  
      std::mt19937 gen(rd()); 
      std::uniform_real_distribution<> dis(-2.0, 2.0);
      
      // Init random parameters
      for (int i = 0; i < nbParams; ++i)
      {
         initialParams.emplace_back(dis(gen));
      } 
      
      auto optimizer = xacc::getOptimizer("nlopt", 
         xacc::HeterogeneousMap { 
            std::make_pair("initial-parameters", initialParams),
            std::make_pair("nlopt-maxeval", nbParams*100) });
      
      auto qaoa = xacc::getService<xacc::Algorithm>("QAOA");
      
      const bool initOk = qaoa->initialize({
                              std::make_pair("accelerator", acc),
                              std::make_pair("optimizer", optimizer),
                              std::make_pair("observable", observable),
                              // number of time steps (p) param
                              std::make_pair("steps", nbSteps)
                           });
      qaoa->execute(buffer);
      std::cout << "Min QUBO: " << (*buffer)["opt-val"].as<double>() << "\n";
   }

In Python:

.. code:: python

   import xacc,sys, numpy as np

   # Get access to the desired QPU and
   # allocate some qubits to run on
   qpu = xacc.getAccelerator('qpp')

   # Construct the Hamiltonian as an XACC PauliOperator
   # This Hamiltonian corresponds to the QUBO problem:
   # y = -5x_1 -3x_2 -8x_3 -6x_4 + 4x_1x_2 + 8x_1x_3 + 2x_2x_3 + 10x_3x_4
   ham = xacc.getObservable('pauli', '-5.0 - 0.5 Z0 - 1.0 Z2 + 0.5 Z3 + 1.0 Z0 Z1 + 2.0 Z0 Z2 + 0.5 Z1 Z2 + 2.5 Z2 Z3')

   # We need 4 qubits
   buffer = xacc.qalloc(4)

   # There are 7 gamma terms (non-identity) in the cost Hamiltonian 
   # and 4 beta terms for mixer Hamiltonian
   nbParamsPerStep = 7 + 4

   # The number of steps (often referred to as 'p' parameter): 
   # alternating layers of mixer and cost Hamiltonian exponential.
   nbSteps = 4

   # Total number of params
   nbTotalParams = nbParamsPerStep * nbSteps

   # Init params randomly: 
   initParams = np.random.rand(nbTotalParams)

   # The optimizer: nlopt
   opt = xacc.getOptimizer('nlopt', { 'initial-parameters': initParams })

   # Create the QAOA algorithm
   qaoa = xacc.getAlgorithm('QAOA', {
                           'accelerator': qpu,
                           'observable': ham,
                           'optimizer': opt,
                           'steps': nbSteps
                           })

   result = qaoa.execute(buffer)
   print('Min QUBO value = ', buffer.getInformation('opt-val'))


Quantum Phase Estimation
++++++++++++++++++++++++
The ``QPE`` algorithm (also known as quantum eigenvalue estimation algorithm) provides 
an implementation of Algorithm that estimates the phase (or eigenvalue) of an eigenvector of a unitary operator.

Here the unitary operator is called an `oracle` which is a quantum subroutine 
that acts upon a set of qubits and returns the answer as a phase. 
The bits precision is automatically inferred from the size of the input buffer.

+------------------------+------------------------------------------------------------------------+------------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                                 |             type                         |
+========================+========================================================================+==========================================+
|    oracle              | The circuit represents the unitary operator.                           | pointer-like CompositeInstruction        |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|    accelerator         | The backend quantum computer to use.                                   | pointer-like Accelerator                 |
+------------------------+------------------------------------------------------------------------+------------------------------------------+
|   state-preparation    | The circuit to prepare the eigen state.                                | pointer-like CompositeInstruction        |
+------------------------+------------------------------------------------------------------------+------------------------------------------+


.. code:: cpp

   #include "xacc.hpp"
   #include "xacc_service.hpp"

   int main(int argc, char **argv) {
   xacc::Initialize(argc, argv);
   // Accelerator:
   auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 4096)});
   
   // In this example: we want to estimate the *phase* of an arbitrary 'oracle'
   // i.e. Oracle(|State>) = exp(i*Phase)*|State>
   // and we need to estimate that Phase.

   // Oracle: CPhase(theta) or CU1(theta) which is defined as
   // 1 0 0 0
   // 0 1 0 0
   // 0 0 1 0
   // 0 0 0 e^(i*theta)
   // The eigenstate is |11>; i.e. CPhase(theta)|11> = e^(i*theta)|11>

   // Since this oracle operates on 2 qubits, we need to add more qubits to the buffer.
   // The more qubits we have, the more accurate the estimate.
   // Resolution := 2^(number qubits in the calculation register).
   // 5-bit precision => 7 qubits in total
   auto buffer = xacc::qalloc(7);
   auto qpe = xacc::getService<xacc::Algorithm>("QPE");
   auto compiler = xacc::getCompiler("xasm");
   
   // Create oracle: CPhase gate with theta = 2pi/3
   // i.e. the phase value to estimate is 1/3 ~ 0.33333.
   auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
   auto oracle = gateRegistry->createComposite("oracle");
   oracle->addInstruction(gateRegistry->createInstruction("CPhase", { 0, 1 }, { 2.0 * M_PI/ 3.0 }));

   // Eigenstate preparation = |11> state
   auto statePrep = compiler->compile(R"(__qpu__ void prep1(qbit q) {
      X(q[0]); 
      X(q[1]); 
   })", nullptr)->getComposite("prep1");  
   
   // Initialize the Quantum Phase Estimation:
   qpe->initialize({
                     std::make_pair("accelerator", acc),
                     std::make_pair("oracle", oracle),
                     std::make_pair("state-preparation", statePrep)
                     });
   
   // Run the algorithm
   qpe->execute(buffer);
   // Expected result: 
   // The factor here is 2^5 (precision) = 32
   // we expect the two most-likely bitstring is 10 and 11
   // i.e. the true result is between 10/32 = 0.3125 and 11/32 = 0.34375  
   std::cout << "Probability of the two most-likely bitstrings 10 (theta = 0.3125) and 11 (theta = 0.34375 ): \n";
   std::cout << "Probability of |11010> (11) = " << buffer->computeMeasurementProbability("11010") << "\n";
   std::cout << "Probability of |01010> (10) = " << buffer->computeMeasurementProbability("01010") << "\n";

   xacc::Finalize();
   }

or in Python

.. code:: python

   import xacc,sys, numpy as np

   # Get access to the desired QPU and
   # allocate some qubits to run on
   qpu = xacc.getAccelerator('qpp',  { 'shots': 4096 })

   # In this example: we want to estimate the *phase* of an arbitrary 'oracle'
   # i.e. Oracle(|State>) = exp(i*Phase)*|State>
   # and we need to estimate that Phase.

   # The oracle is a simple T gate, and the eigenstate is |1>
   # T|1> = e^(i*pi/4)|1> 
   # The phase value of pi/4 = 2pi * (1/8)
   # i.e. if we use a 3-bit register for estimation, 
   # we will get the correct answer of 1 deterministically.

   xacc.qasm('''.compiler xasm
   .circuit oracle
   .qbit q
   T(q[0]);
   ''')
   oracle = xacc.getCompiled('oracle')

   # We need to prepare the eigenstate |1>
   xacc.qasm('''.compiler xasm
   .circuit prep
   .qbit q
   X(q[0]);
   ''')
   statePrep = xacc.getCompiled('prep')

   # We need 4 qubits (3-bit precision)
   buffer = xacc.qalloc(4)

   # Create the QPE algorithm
   qpe = xacc.getAlgorithm('QPE', {
                           'accelerator': qpu,
                           'oracle': oracle,
                           'state-preparation': statePrep
                           })

   qpe.execute(buffer)
   # We should only get the bit string of |100> = 1
   # i.e. phase value of 1/2^3 = 1/8.
   print(buffer)

QITE
++++
The Quantum Imaginary Time Evolution (QITE) Algorithm requires the following input information:
(`Motta et al. (2020) <https://arxiv.org/pdf/1901.07653.pdf>`_)

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    observable          | The hermitian operator represents the cost Hamiltonian.         | std::shared_ptr<Observable>          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    accelerator         | The Accelerator backend to target                               | std::shared_ptr<Accelerator>         |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    steps               | The number of Trotter steps.                                    | int                                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    step-size           | The Trotter step size.                                          | double                               |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

Optionally, users can provide these parameters:

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    ansatz              | State preparation circuit.                                      | std::shared_ptr<CompositeInstruction>|
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    analytical          | If true, perform an analytical run rather than                  | boolean                              |
|                        | executing quantum circuits on the Accelerator backend.          |                                      |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    initial-state       | For `analytical` mode only, select the initial state.           | int                                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

This Algorithm will add ``opt-val`` (``double``) which is the energy value at the final Trotter step to the provided ``AcceleratorBuffer``.
The results of the algorithm are therefore retrieved via these keys (see snippet below). 
Also, energy values at each Trotter step are stored in the ``exp-vals`` field (``vector<double>``).

Note: during execution, the following line may be logged to the output console:

.. code:: cpp

   warning: solve(): system seems singular; attempting approx solution

This is completely normal and can be safely ignored.

.. code:: cpp

   #include "xacc.hpp"
   #include "xacc_observable.hpp"
   #include "xacc_service.hpp"

   int main(int argc, char **argv) {
      xacc::Initialize(argc, argv);
      // Use the Qpp simulator as the accelerator
      auto acc = xacc::getAccelerator("qpp");
      
      auto buffer = xacc::qalloc(1);
      auto observable = xacc::quantum::getObservable(
            "pauli",
            std::string("0.7071067811865475 X0 + 0.7071067811865475 Z0"));
      
      auto qite = xacc::getService<xacc::Algorithm>("qite");
      const int nbSteps = 25;
      const double stepSize = 0.1;

      const bool initOk =  qite->initialize({
         std::make_pair("accelerator", acc),
         std::make_pair("steps", nbSteps),
         std::make_pair("observable", observable),
         std::make_pair("step-size", stepSize)
      });

      qite->execute(buffer);
      std::cout << "Min Energy: " << (*buffer)["opt-val"].as<double>() << "\n";
   }

In Python:

.. code:: python

   import xacc,sys, numpy as np
   import matplotlib.pyplot as plt

   # Get access to the desired QPU and
   # allocate some qubits to run on
   qpu = xacc.getAccelerator('qpp')

   # Construct the Hamiltonian as an XACC PauliOperator
   ham = xacc.getObservable('pauli', '0.70710678118 X0 + 0.70710678118 Z0')

   # We just need 1 qubit
   buffer = xacc.qalloc(1)

   # Horizontal axis: 0 -> 2.5
   # The number of Trotter steps 
   nbSteps = 25

   # The Trotter step size
   stepSize = 0.1

   # Create the QITE algorithm
   qite = xacc.getAlgorithm('qite', {
                           'accelerator': qpu,
                           'observable': ham,
                           'step-size': stepSize,
                           'steps': nbSteps
                           })

   result = qite.execute(buffer)

   # Expected result: ~ -1
   print('Min energy value = ', buffer.getInformation('opt-val'))
   E = buffer.getInformation('exp-vals')
   # Plot energy vs. beta
   plt.plot(np.arange(0, nbSteps + 1) * stepSize, E, 'ro', label = 'XACC QITE')
   plt.grid()
   plt.show()


ADAPT
++++
The Adaptive Derivative Assembled Problem Tailored (ADAPT) Algorithm requires the following input information:
(`Grismley et al. (2018) <https://arxiv.org/pdf/1812.11173.pdf>`_, `Tang et al. (2019) <https://arxiv.org/pdf/1911.10205.pdf>`_, `Zhu et al. (2020) <https://arxiv.org/pdf/2005.10258.pdf>`_)

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    observable          | The hermitian operator represents the Hamiltonian               | std::shared_ptr<Observable>          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    accelerator         | The Accelerator backend to target                               | std::shared_ptr<Accelerator>         |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    optimizer           | The classical optimizer to use                                  | std::shared_ptr<Optimizer>           |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    pool                | Pool of operators to construct adaptive ansatz                  | std::string                          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    sub-algorithm       | Algorithm called by ADAPT (VQE or QAOA)                         | std::string                          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

Optionally, users can provide these parameters:

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    initial-state       | State preparation circuit.                                      | std::shared_ptr<CompositeInstruction>|
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    n-electrons         | Required parameter for VQE, unless initial-state is provided    | int                                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    maxiter             | Maximum number of ADAPT cycles/number of layers in QAOA         | int                                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    print-threshold     | Value above which commutators are printed (Default 1.0e-10)     | double                               |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    adapt-threshold     | Stops ADAPT when norm of gradient vector falls below this value | double                               |
|                        | (Default 1.0e-2)                                                |                                      |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

ADAPT-VQE

.. code:: cpp

    #include "xacc.hpp"
    #include "xacc_observable.hpp"
    #include "xacc_service.hpp"

    int main(int argc, char **argv) {
      xacc::Initialize(argc, argv);

      // Get reference to the Accelerator
      // specified by --accelerator argument
      auto accelerator = xacc::getAccelerator("qpp");

      // Get reference to the Optimizer
      // specified by --optimizer argument
      auto optimizer = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-optimizer", "l-bfgs")});

      // Allocate 4 qubits in the buffer
      auto buffer = xacc::qalloc(4);

      // Instantiate ADAPT algorithm
      auto adapt = xacc::getService<xacc::Algorithm>("adapt");

      // Number of electrons
      int nElectrons = 2;

      // Specify the operator pool
      auto pool = "qubit-pool";

      // Specify the sub algorithm
      auto subAlgo_vqe = "vqe"; 

      // This is the H2 Hamiltonian in a fermionic basis
      auto str = std::string("(-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + "
                              "(-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + "
                              "(0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + "
                              "(0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + "
                              "(-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + "
                              "(0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + "
                              "(-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + "
                              "(-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + "
                              "(0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + "
                              "(0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + "
                              "(-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + "
                              "(0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + "
                              "(0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + "
                              "(0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + "
                              "(0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + "
                              "(0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + "
                              "(0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + "
                              "(0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + "
                              "(-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + "
                              "(0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + "
                              "(0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + "
                              "(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.7080240981,0)");


      // Create Observable from Hamiltonian string
      auto H = xacc::quantum::getObservable("fermion", str);

      // Pass parameters to ADAPT algorithm
      adapt->initialize({std::make_pair("accelerator", accelerator),
                                    std::make_pair("observable", H),
                                    std::make_pair("optimizer", optimizer),
                                    std::make_pair("pool", pool),
                                    std::make_pair("n-electrons", nElectrons),
                                    std::make_pair("sub-algorithm", subAlgo_vqe)
                                    });

      // Execute ADAPT-VQE
      adapt->execute(buffer);

      xacc::Finalize();
      return 0;
    }

In Python:

.. code:: python

    import xacc

    qpu = xacc.getAccelerator('qpp')
    optimizer = xacc.getOptimizer('nlopt',{'nlopt-optimizer':'l-bfgs'})
    buffer = xacc.qalloc(4)

    opstr = '''
    (-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + 
    (-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + 
    (0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + 
    (0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + 
    (-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + 
    (0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + 
    (-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + 
    (-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + 
    (0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + 
    (0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + 
    (-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + 
    (0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + 
    (0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + 
    (0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + 
    (0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + 
    (0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + 
    (0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + 
    (0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + 
    (-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + 
    (0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + 
    (0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + 
    (-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.7080240981,0)
    '''

    H = xacc.getObservable('fermion', opstr)

    adapt = xacc.getAlgorithm('adapt', {'accelerator': qpu,
                                      'optimizer': optimizer,
                                      'observable': H,
                                      'n-electrons': 2,
                                      'maxiter': 2,
                                      'sub-algorithm': 'vqe',
                                      'pool': 'qubit-pool'})

    adapt.execute(buffer)


ADAPT-QAOA

 .. code:: cpp

    #include "xacc.hpp"
    #include "xacc_observable.hpp"
    #include "xacc_service.hpp"

    int main(int argc, char **argv) {
      xacc::Initialize(argc, argv);

      // Get reference to the Accelerator
      // specified by --accelerator argument
      auto accelerator = xacc::getAccelerator("qpp");

      // Get reference to the Optimizer
      // specified by --optimizer argument
      auto optimizer = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-optimizer", "l-bfgs")});

      // Allocate 4 qubits in the buffer
      auto buffer = xacc::qalloc(4);

      // Instantiate ADAPT algorithm
      auto adapt = xacc::getService<xacc::Algorithm>("adapt");

      // Specify the operator pool
      auto pool = "multi-qubit-qaoa";

      // Specify the sub algorithm
      auto subAlgo_qaoa = "QAOA"; 

      // Number of layers
      auto nLayers = 2;

      // This is the cost Hamiltonian 
      auto H = xacc::quantum::getObservable(
          "pauli", std::string("-5.0 - 0.5 Z0 - 1.0 Z2 + 0.5 Z3 + 1.0 Z0 Z1 + 2.0 Z0 Z2 + 0.5 Z1 Z2 + 2.5 Z2 Z3"));

      // Pass parameters to ADAPT algorithm
      adapt->initialize({std::make_pair("accelerator", accelerator),
                                    std::make_pair("observable", H),
                                    std::make_pair("optimizer", optimizer),
                                    std::make_pair("pool", pool),
                                    std::make_pair("maxiter", nLayers),
                                    std::make_pair("sub-algorithm", subAlgo_qaoa)
                                    });

      // Execute ADAPT-QAOA
      adapt->execute(buffer);

      xacc::Finalize();
      return 0;
    }

In Python:

.. code:: python

    import xacc
      
    accelerator = xacc.getAccelerator("qpp")

    buffer = xacc.qalloc(4)

    optimizer = xacc.getOptimizer('nlopt',{'nlopt-optimizer':'l-bfgs'})

    pool = "multi-qubit-qaoa"

    nLayers = 2

    subAlgo_qaoa = "QAOA"

    H = xacc.getObservable('pauli', '-5.0 - 0.5 Z0 - 1.0 Z2 + 0.5 Z3 + 1.0 Z0 Z1 + 2.0 Z0 Z2 + 0.5 Z1 Z2 + 2.5 Z2 Z3')
    
    adapt = xacc.getAlgorithm('adapt', {
                            'accelerator': accelerator,
                            'observable': H,
                            'optimizer': optimizer,
                            'pool': pool,
                            'maxiter': nLayers,
                            'sub-algorithm': subAlgo_qaoa
                            })

    adapt.execute(buffer)


QCMX
++++
The Quantum Connected Moments eXpansion (QCMX) Algorithm requires the following input information:
(`Kowalski, K. and Peng, Bo. (2018) <https://arxiv.org/pdf/2009.05709>`_)

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    observable          | The hermitian operator represents the Hamiltonian               | std::shared_ptr<Observable>          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    accelerator         | The Accelerator backend to target                               | std::shared_ptr<Accelerator>         |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    ansatz              | State preparation circuit                                       | std::shared_ptr<CompositeInstruction>|
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    cmx-order           | The order of the leading term in the CMX                        | int                                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    expansion-type      | Expansion type (Cioslowski, Knowles, PDS)                       | std::string                          |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

 .. code:: cpp

    #include "xacc.hpp"
    #include "xacc_observable.hpp"
    #include "xacc_service.hpp"

    int main(int argc, char **argv) {
      xacc::Initialize(argc, argv);

      // Get reference to the Accelerator
      // specified by --accelerator argument
      auto accelerator = xacc::getAccelerator("qpp");

      // Get reference to the Hamiltonian
      // specified by the --observable argument
      auto H = xacc::quantum::getObservable("pauli", std::string("0.2976 + 0.3593 Z0 - 0.4826 Z1 + 0.5818 Z0 Z1 + 0.0896 X0 X1 + 0.0896 Y0 Y1"));

      // Specify the expansion type
      auto expansion = "Cioslowski";

      // Specify the CMX expansion order
      auto order = 2;

      // Create reference to the initial state
      // specified by the --ansatz argument
      auto provider = xacc::getService<xacc::IRProvider>("quantum");
      auto ansatz = provider->createComposite("initial-state");
      ansatz->addInstruction(provider->createInstruction("X", {(size_t)0}));

      // Allocate 2 qubits in the buffer
      auto buffer = xacc::qalloc(2);

      // Instantiate the QCMX algorithm
      auto qcmx = xacc::getService<xacc::Algorithm>("qcmx");

      // Pass parameters to QCMX algorithm
      qcmx->initialize({{"accelerator",accelerator}, 
                      {"observable", H},
                      {"ansatz", ansatz},
                      {"cmx-order", order}, 
                      {"expansion-type", expansion}});

      // Execute QCMX
      qcmx->execute(buffer);

      xacc::Finalize();
      return 0;
    }

In Python:

.. code:: python

  import xacc
    
  accelerator = xacc.getAccelerator("qpp")

  H = xacc.getObservable('pauli', '0.2976 + 0.3593 Z0 - 0.4826 Z1 + 0.5818 Z0 Z1 + 0.0896 X0 X1 + 0.0896 Y0 Y1')

  expansion = 'Cioslowski'

  order = 2

  provider = xacc.getIRProvider('quantum')
  ansatz = provider.createComposite('initial-state')
  ansatz.addInstruction(provider.createInstruction('X', [0]))

  buffer = xacc.qalloc(2)

  qcmx = xacc.getAlgorithm('qcmx', {
                          'accelerator': accelerator,
                          'observable': H,
                          'ansatz': ansatz,
                          'cmx-order': order,
                          'expansion-type': expansion
                          })

  qcmx.execute(buffer)

QEOM
++++
The Quantum Equation of Motion (QEOM) Algorithm requires the following input information:
(`Ollitrault et al (2020) <https://arxiv.org/abs/1910.12890>`_)

+------------------------+------------------------------------------------------+------------------------------------------+
|  Algorithm Parameter   |                  Parameter Description               |             type                         |
+========================+======================================================+==========================================+
|    observable          | The hermitian operator represents the Hamiltonian    | std::shared_ptr<Observable>              |
+------------------------+------------------------------------------------------+------------------------------------------+
|    accelerator         | The Accelerator backend to target                    | std::shared_ptr<Accelerator>             |
+------------------------+------------------------------------------------------+------------------------------------------+
|    ansatz              | State preparation circuit                            | std::shared_ptr<CompositeInstruction>    |
+------------------------+------------------------------------------------------+------------------------------------------+
|    n-electrons         | The number of electrons                              | int                                      |
+------------------------+------------------------------------------------------+------------------------------------------+
|    operators           | Excitation operators                                 | std::vector<std::shared_ptr<Observable>> |
+------------------------+------------------------------------------------------+------------------------------------------+

Please note that the algorithm requires either ``n-electrons`` or ``operators``. In the former case, it will default to single and double excitation operators. 

 .. code:: cpp

    #include "xacc.hpp"
    #include "xacc_service.hpp"
    #include "Algorithm.hpp"
    #include "xacc_observable.hpp"
    #include "OperatorPool.hpp"

    int main(int argc, char **argv) {
      xacc::Initialize(argc, argv);

      // Get reference to the Accelerator
      // specified by --accelerator argument
      auto accelerator = xacc::getAccelerator("qpp");

      // Allocate 4 qubits in the buffer
      auto buffer = xacc::qalloc(4);

      // Get reference to the Hamiltonian
      // specified by the --observable argument
        auto str = std::string(
      "(-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + "
      "(-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + "
      "(0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + "
      "(0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + "
      "(-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + "
      "(0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + "
      "(-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + "
      "(-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + "
      "(0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + "
      "(0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + "
      "(-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + "
      "(0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + "
      "(0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + "
      "(0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + "
      "(0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + "
      "(0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + "
      "(0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + "
      "(0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + "
      "(-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + "
      "(0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + "
      "(0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + "
      "(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + "
      "(0.7080240981,0)");
      auto H = xacc::quantum::getObservable("fermion", str);

      // Create reference to the initial state
      // specified by the --ansatz argument
      auto pool = xacc::getService<OperatorPool>("singlet-adapted-uccsd");
      pool->optionalParameters({{"n-electrons", 2}});
      pool->generate(buffer->size());
      auto ansatz = xacc::getIRProvider("quantum")->createComposite("ansatz");
      ansatz->addInstruction(
          xacc::getIRProvider("quantum")->createInstruction("X", {0}));
      ansatz->addInstruction(
          xacc::getIRProvider("quantum")->createInstruction("X", {2}));
      ansatz->addVariable("x0");
      for (auto &inst : pool->getOperatorInstructions(2, 0)->getInstructions()) {
        ansatz->addInstruction(inst);
      }
      auto kernel = ansatz->operator()({0.0808});

      // Instantiate the QEOM algorithm
      auto qeom = xacc::getAlgorithm("qeom");

      // Pass parameters to QEOM algorithm
      qeom->initialize({{"accelerator",accelerator}, 
                      {"observable", H},
                      {"ansatz", kernel},
                      {"n-electrons", 2}});

      // Execute QEOM
      qeom->execute(buffer);

      xacc::Finalize();
      return 0;
    }

In Python:

.. code:: python

  import xacc
    
  accelerator = xacc.getAccelerator("qpp")

  buffer = xacc.qalloc(4)

  opstr = '''
  (-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + 
  (-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + 
  (0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + 
  (0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + 
  (-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + 
  (0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + 
  (-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + 
  (-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + 
  (0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + 
  (0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + 
  (-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + 
  (0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + 
  (0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + 
  (0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + 
  (0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + 
  (0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + 
  (0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + 
  (0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + 
  (-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + 
  (0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + 
  (0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + 
  (-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.7080240981,0)
  '''
  H = xacc.getObservable('fermion', opstr)

  pool = xacc.quantum.getOperatorPool("singlet-adapted-uccsd")
  pool.optionalParameters({"n-electrons": 2})
  pool.generate(buffer.size())
  provider = xacc.getIRProvider('quantum')
  ansatz = provider.createComposite('initial-state')
  ansatz.addInstruction(provider.createInstruction('X', [0]))
  ansatz.addInstruction(provider.createInstruction('X', [2]))
  ansatz.addVariable("x0")
  for inst in pool.getOperatorInstructions(2, 0).getInstructions():
      ansatz.addInstruction(inst)
  kernel = ansatz.eval([0.0808])
  
  qeom = xacc.getAlgorithm('qeom', {
                          'accelerator': accelerator,
                          'observable': H,
                          'ansatz': kernel,
                          'n-electrons': 2,
                          })

  qeom.execute(buffer)


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


QFAST Circuit Synthesis
+++++++++++++++++++++++
The ``QFAST`` circuit generator generates a quantum circuit for an arbitary unitary matrix.
(See `Ed Younis, et al. <https://arxiv.org/pdf/2003.04462.pdf>`_)

The ``QFAST`` circuit generator only requires the ``unitary`` input information. 
Optionally, we can provide additional configurations as listed below.

+------------------------+------------------------------------------------------------------------+--------------------------------------+
|  Algorithm Parameter   |                  Parameter Description                                 |             type                     |
+========================+========================================================================+======================================+
|  unitary               | The unitary matrix.                                                    | Eigen::MatrixXcd/numpy 2-D array     |
+------------------------+------------------------------------------------------------------------+--------------------------------------+
|  trace-distance        | The target trace distance of the Hilbert-Schmidt inner produc          | double (default = 0.01)              |
+------------------------+------------------------------------------------------------------------+--------------------------------------+
| explore-trace-distance | The `stopping-condition` trace distance for the Explore phase.         | double (default = 0.1)               |
+------------------------+------------------------------------------------------------------------+--------------------------------------+
|  initial-depth         | The initial number of circuit layers.                                  | int (default = 1)                    |
+------------------------+------------------------------------------------------------------------+--------------------------------------+

By default, after each unitary matrix decomposition, the QFAST plugin will save the result (in terms of smaller block matrices) into a cache file located at ``$XACC_INSTALL_DIR/tmp``. If a cache entry is found, the QFAST plugin will re-use the result automatically.

Users can modify the cache filename via the configuration key ``cache-file-name``.

Example:

In Cpp,

.. code:: cpp

  auto qfast = std::dynamic_pointer_cast<quantum::Circuit>(xacc::getService<Instruction>("QFAST"));
  Eigen::MatrixXcd ccnotMat = Eigen::MatrixXcd::Identity(8, 8);
  ccnotMat(6, 6) = 0.0;
  ccnotMat(7, 7) = 0.0;
  ccnotMat(6, 7) = 1.0;
  ccnotMat(7, 6) = 1.0;
  
  const bool expandOk = qfast->expand({ 
    std::make_pair("unitary", ccnotMat)
  });

In Python,

.. code:: python

   import xacc, numpy as np

   # CCNOT matrix:
   # This takes a 2-D numpy array.
   ccnotMat = np.identity(8, dtype = np.cdouble)
   ccnotMat[6][6] = 0.0
   ccnotMat[7][7] = 0.0
   ccnotMat[6][7] = 1.0
   ccnotMat[7][6] = 1.0

   composite = xacc.createCompositeInstruction('QFAST', { 'unitary' : ccnotMat })
   print(composite)

Placement
---------

Noise Adaptive Layout
+++++++++++++++++++++
The ``triQ`` placement IRTransformation plugin implements the noise adaptive layout mapping method
(See `Prakash Murali, et al. <https://arxiv.org/pdf/1901.11054.pdf>`_)

The ``triQ`` plugin will automatically retrieve backend information (e.g. gate and readout fidelity) from the backend
``Accelerator`` if available.

Otherwise, users can provide the backend configurations in JSON format using the ``backend-json`` key.
The backend Json schema must conform to IBMQ specifications.

The ``triQ`` plugin depends on the Z3 SMT solver (version >= 4.8).

Z3 can be installed via:

- Ubuntu:

.. code:: bash

   apt-get install -y libz3-dev

- MacOS:

.. code:: bash

   brew install z3

Example:

.. code:: cpp

  auto compiler = xacc::getCompiler("staq");
  // Specify a remote IBM backend
  auto accelerator = xacc::getAccelerator("ibm:ibmq_16_melbourne");
  auto q = xacc::qalloc(5);
  q->setName("q");
  xacc::storeBuffer(q);
  auto IR = compiler->compile(R"(__qpu__ void f(qreg q) {
                                    OPENQASM 2.0;
                                    include "qelib1.inc";
                                    creg c[16];
                                    cx q[2],q[1];
                                    cx q[1],q[2];
                                    cx q[3],q[2];
                                    cx q[2],q[3];
                                    cx q[4],q[3];
                                    cx q[3],q[4];
                                    h q[0];
                                    t q[4];
                                    t q[3];
                                    t q[0];
                                    cx q[3],q[4];
                                    cx q[0],q[3];
                                    cx q[4],q[0];
                                    tdg q[3];
                                    })");

  auto program = IR->getComposites()[0];
  // Apply noise adaptive layout (TriQ) 
  auto irt = xacc::getIRTransformation("triQ");
  irt->apply(program, accelerator);