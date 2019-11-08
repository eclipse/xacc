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

   ibm_valencia = xacc,getAccelerator('ibm:ibmq_valencia');
   ... or ...
   ibm_valencia = xacc.getAccelerator('ibm', {'backend':'ibmq_valencia')});

You can specify the number of shots in this way as well

.. code:: cpp

   auto ibm_valencia = xacc::getAccelerator("ibm:ibmq_valencia", {std::make_pair("shots", 2048)});

or in Python

.. code:: Python

   ibm_valencia = xacc.getAccelerator('ibm:ibmq_valencia', {'shots':2048)});

In order to target the remote backend (for ``initialize()`` or ``execute()``) you must provide
you IBM credentials to XACC. To do this add the following to a plain text file ``$HOME/.ibm_config``

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


DWave
+++++

TNQVM
+++++

Algorithms
----------

Accelerator Decorators
----------------------

