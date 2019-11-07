Plugins
=======

XACC provides a number of framework extension points that enable users to program
available quantum accelerators in a plug-and-play manner. This is accomplished
through the C++ native Open Service Gateway Initiative (OSGi) implementation provided
by the `CppMicroServices <https://github.com/cppmicroservices/cppmicroservices>`_
framework.

Developers can extend the following interfaces in the core framework:
`Compiler <https://github.com/eclipse/xacc/blob/master/xacc/compiler/Compiler.hpp>`_,
`Preprocessor <https://github.com/eclipse/xacc/blob/master/xacc/compiler/Preprocessor.hpp>`_,
`Accelerator <https://github.com/eclipse/xacc/blob/master/xacc/accelerator/Accelerator.hpp>`_,
`IRTransformation <https://github.com/eclipse/xacc/blob/master/xacc/ir/IRTransformation.hpp>`_,
`IRPreprocessor <https://github.com/eclipse/xacc/blob/master/xacc/ir/IRPreprocessor.hpp>`_,
`IRGenerator <https://github.com/eclipse/xacc/blob/master/xacc/ir/IRGenerator.hpp>`_,
`EmbeddingAlgorithm <https://github.com/eclipse/xacc/blob/master/quantum/aqc/compiler/EmbeddingAlgorithm.hpp>`_ and
`AcceleratorDecorator <https://github.com/zpparks314/xacc/blob/master/xacc/accelerator/AcceleratorDecorator.hpp>`_.

Plugins can be stand-alone, sand-boxed projects that provide an appropriate
implementation of an XACC extensible interface. Plugins should be compiled and built
as shared libraries, and those shared libraries can be dropped into the
``$XACC_INSTALL/plugins`` directory and immediately leveraged by the framework.

Below we detail the plugins that are currently available for use with XACC. When XACC is built and
installed from source, the XACC IBM, Rigetti, and D-Wave plugins are automatically
built and installed as well. These plugins enable the programming, compilation,
and execution of quantum algorithms for Rigetti, D-Wave, and IBM QPUs,
as well as a number of backend simulators. They additionally provide a mechanism to write XACC
quantum kernels using Quil, OpenQasm, XACC IR, and D-Wave QMI low-level languages.

Rigetti
-------
Installation
++++++++++++

The `Rigetti Plugin <https://github.com/eclipse/xacc/tree/master/quantum/plugins/rigetti>`_ provides
support to XACC for compiling kernels written in Quil, and executing programs
on the Rigetti Forest infrastructure.

This plugin is built and installed automatically when building XACC from source.

Credentials
+++++++++++

In order to target the Rigetti Forest infrastructure you must provide XACC with your API key. By default
XACC will use your PyQuil Config file at ``$HOME/.pyquil_config``.
See `Connecting to the Rigetti Forest <https://pyquil.readthedocs.io/en/stable/start.html#connecting-to-rigetti-forest>`_ for information on creating this file.

If you do not have a ``$HOME/.pyquil_config`` file, and you have installed the XACC Python bindings, then you can run

.. code::

   $ python -m xacc -c rigetti -k YOURAPIKEY -u YOURUSERID

Rigetti Runtime Options
+++++++++++++++++++++++
The Rigetti plugin exposes the following runtime options

+------------------------+----------------------------------------+
| Argument               |            Description                 |
+========================+========================================+
| rigetti-backend        | The backend to target (defaults to QVM)|
+------------------------+----------------------------------------+
| rigetti-trials         | The number of shots to execute per job |
+------------------------+----------------------------------------+

IBM
---

Installation
++++++++++++

The `IBM Plugin <https://github.com/eclipse/xacc/tree/master/quantum/plugins/ibm>`_ provides
support to XACC for compiling kernels written in OpenQasm, and executing programs
on both the IBM Quantum Experience and the IBM Q Network.

This plugin is built and installed automatically when building XACC from source.

.. note::

      The xacc-ibm project also provides an Accelerator implementation that
      delegates to a local build of the Qiskit C++ qasm simulator.
      This Accelerator implementation requires BLAS/LAPACK libraries to be installed.
      Here's how to install these as binaries on various popular platforms:

      .. code::

         $ (macosx) should already be there in Accelerate Framework, if not
         $ (macosx) brew install openblas lapack
         $ (fedora) dnf install blas-devel lapack-devel
         $ (ubuntu) apt-get install libblas-dev liblapack-dev

Credentials
+++++++++++

In order to target the IBM Quantum Experience or Q Network infrastructure you must provide XACC with your API key.

If you installed the XACC Python bindings, then you can run

.. code::

   $ python -m xacc -c ibm -k YOURAPIKEY (--hub HUB --group GROUP --project PROJECT)


Alternatively, open the file ``$HOME/.ibm_config``, and add the following contents

.. code:: bash

   key: YOUR_API_KEY
   url: https://quantumexperience.ng.bluemix.net
   hub: HUB
   group: GROUP
   project: PROJECT

Note the ``hub``, ``group``, and ``project`` lines are not required if you are targeting the public
Quantum Experience API.

IBM Runtime Options
+++++++++++++++++++
The IBM plugin exposes the following runtime options

+------------------------+----------------------------------------+
| Argument               |            Description                 |
+========================+========================================+
| ibm-backend            | The backend to target (e.g. ibmqx5)    |
+------------------------+----------------------------------------+
| ibm-shots              | The number of shots to execute per job |
+------------------------+----------------------------------------+

D-Wave
------

Installation
++++++++++++

The `D-Wave Plugin <https://github.com/eclipse/xacc/tree/master/quantum/plugins/dwave>`_ provides
support to XACC for executing programs on the D-Wave QPU via the D-Wave Accelerator.

This plugin is built and installed automatically when building XACC from source.

Credentials
+++++++++++

In order to target the D-Wave remote resources you must provide
XACC with your D-Wave Qubist API key. By default
XACC will search for a config file at ``$HOME/.dwave_config``.

If you installed the XACC Python bindings, then you can run

.. code::

   $ python -m xacc -c dwave -k YOURAPIKEY

D-Wave Runtime Options
++++++++++++++++++++++
The D-Wave plugin exposes the following runtime options

   +------------------------+----------------------------------------+
   | Argument               |            Description                 |
   +========================+========================================+
   | dwave-solver           | The backend to target (e.g. DW_2000Q_2)|
   +------------------------+----------------------------------------+
   | dwave-num-reads        | The number of shots to execute per job |
   +------------------------+----------------------------------------+

TNQVM
-----

Installation
++++++++++++

The `TNQVM Plugin <https://github.com/ornl-qci/tnqvm>`_ provides
support to XACC for executing programs
on the ORNL tensor network quantum virtual machine. TNQVM models the wave
function of a quantum register as a tensor network, and specifically a matrix
product state. For more information on the TNQVM simulation method, check out
`Validating Quantum-Classical Programming Models with Tensor Network Simulations <https://arxiv.org/abs/1807.07914>`_


.. note::

   TNQVM requires BLAS/LAPACK libraries to be installed.
   Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) should already be there in Accelerate Framework, if not
      $ (macosx) brew install openblas lapack
      $ (fedora) dnf install blas-devel lapack-devel
      $ (ubuntu) apt-get install libblas-dev liblapack-dev

To install this plugin, run the following

.. code::

   $ git clone https://github.com/ornl-qci/tnqvm
   $ cd tnqvm && mkdir build && cd build
   $ cmake .. -DXACC_DIR=$HOME/.xacc
   $ make install

If you installed the XACC Python bindings, then you can run

.. code::

   $ cmake .. -DXACC_DIR=$(python -m xacc -L)

ensuring that xacc is in your ``PYTHONPATH``.

If you installed XACC via ``pip``, then you can run

.. code::

   $ python -m pip install tnqvm (with --user if you used that flag for your xacc install)

Python JIT Compiler
-------------------
The XACC base framework provides a Compiler implementation called PyXACCCompiler that
defines an Antlr4 grammar and associated auto-generated Parser for expressing and
compiling Pythonic XACC kernel functions like this

.. code::

   def foo(buffer, theta0):
      H(0)
      Ry(theta0,1)
      CNOT(1,0)
      Measure(0,0)

or for D-Wave

.. code::

   def foo(buffer, h, j):
      qmi(0,0,h)
      qmi(1,1,h)
      qmi(0,1,j)

These Pythonic functions can then be consumed by a custom ``xacc.qpu`` class decorator,
the source code for these functions can be converted to a string with the ``inspect``
module, and compiled with this PyXACCCompiler implementation.

The PyXACC Antlr grammar also defines syntax for generating XACC ``IR`` function instances using any
of the installed and available XACC ``IRGenerator`` interfaces.

Imagine we have an ``IRGenerator`` that produces a UCCSD circuit based on the number of
qubits and electrons in the problem. We could define a Python function like this to
create this circuit (instead of arduously typing out all the instructions)

.. code::

   def uccsd(buffer, *args):
      uccsd(n_qubits=4, n_electrons=2)
      Measure(0,0)

The above code would generate the UCCSD circuit on 4 qubits and 2 fermions
and measure the first qubit, giving an estimated expectation value with respect to
the ``Z`` operator for Hamiltonian term ``<Z0>``.

.. note::

   The above code samples must be consumed by the ``@xacc.qpu()`` Python decorator.
   This decorator handles JIT compilation of the source code and execution
   on the desired Accelerator.

   For an example of using this Pythonic language in Python with the ``xacc.qpu``
   decorator, see the `XACC Python JIT Tutorial <tutorials.html#xacc-python-jit>`_
