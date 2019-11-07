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

Usage:

.. code:: cpp

   auto optimizer = xacc::getOptimizer("mlpack", {std::make_pair("mlpack-optimizer", "sgd")});

or in Python

.. code:: python

   optimizer = xacc.getOptimizer('mlpack', {'mlpack-optimizer':'sgd'})


NLOpt
+++++

Accelerators
------------

Algorithms
----------

Accelerator Decorators
----------------------

