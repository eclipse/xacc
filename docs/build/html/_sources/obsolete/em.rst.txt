Error Mitigation
================
XACC provides built-in error mitigation strategies through the ``AcceleratorDecorator`` interface.

AcceleratorDecorator
++++++++++++++++++++
The ``AcceleratorDecorator`` is an interface that follows the familiar decorator design pattern to
enable the addition of specific behaviors to the XACC ``Accelerator`` implementation.
In XACC, the ``AcceleratorDecorator`` interface is used to employ error mitigation strategies such as
readout-error correction, mixed-state purification, and improved sampling.

Readout Error Correction Decorator
----------------------------------
The XACC ``ROErrorDecorator`` is an ``AcceleratorDecorator`` subclass that requires information
about the probability of an unexpected bit flip occuring during qubit measurement to correct measurement readout errors.

To acquire this information in Python, XACC exposes the ``xacc.compute_readout_error_probabilities(qubit_range[], buffer, qpu)`` method to
connect to a noisy quantum accelerator, compute the error probabilities, and store them in a JSON file.
The location of this file is then given to the framework which can then use it within the ``ROErrorDecorator``
to correct the errors in the obtained measurement results of an execution. An example script to generate this
error probability file, pass its location to the framework, decorate an ``Accelerator``, then execute the
VQE algorithm to compute the binding energy of deuteron and mitigate qubit measurement errors using the IBM
local simulator with bit flip errors activated is shown below.

.. code::

   import xacc
   from xaccvqe import PauliOperator

   xacc.Initialize()

   # Get access to IBM Accelerator,
   # allocate qubits,
   # target the local-ibm accelerator
   qpu = xacc.getAccelerator('local-ibm')
   buffer = qpu.createBuffer('q', 2)

   # introduce readout error probability in the simulator
   xacc.setOption('local-ibm-ro-error', '.01, .1')

   # Compute the readout error probabilities
   # and store it in a file
   p01s, p10s, filename = xacc.compute_readout_error_probabilities(range(buffer.size()), buffer, qpu)

   # Point the XACC framework to the error probability file
   xacc.setOption('ro-error-file', filename)

   # Decorate the Accelerator with the ROErrorDecorator
   qpu = xacc.getAcceleratorDecorator('ro-error', qpu)

   # Construct the deuteron Hamiltonian
   ham = PauliOperator(5.906709445) + \
         PauliOperator({0: 'X', 1: 'X'}, -2.1433) + \
         PauliOperator({0: 'Y', 1: 'Y'}, -2.1433) + \
         PauliOperator({0: 'Z'}, .21829) + \
         PauliOperator({1: 'Z'}, -6.125)

   # Construct the XACC Kernel and direct the framework to
   # run VQE on the defined ansatz, using the decorated accelerator
   # with the deuteron Hamiltonian as the measurable observable
   @xacc.qpu(algo='vqe', accelerator=qpu, observable=ham)
   def ansatz(buffer, t0):
       X(0)
       Ry(t0, 1)
       CNOT(1, 0)

   ansatz(buffer, 0.5)

   xacc.Finalize()

The resulting ``AcceleratorBuffer`` object contains all of the results of the execution like usual,
except now it also stores the readout error corrected measurement bit strings.

Each child buffer in the resulting buffer object now has an additional `ExtraInformation` key-value pair to store
the measured expectation value of the readout error corrected measurement bit strings with key `ro-exp-val-z`

Reduced Density Matrix Purification Decorator
---------------------------------------------
The ``RDMPurification`` decorator can be used to correct for errors caused by
qubit depolarization when using noisy near-term quantum hardware. This decorator implements a
mixed-state purification  approach  that depends  on  the  computation  of  the two-body reduced density matrix (RDM)
and which performs the well-known McWeeny purification formula on the computed RDM.

This decorator requires information about the fermionic source of the Hamiltonian observable to be measured, which
is given to the XACC framework using the ``xacc.setOption('rdm-source', fermion_ham_source)`` method.

Each child buffer in the resulting buffer now has additional `ExtraInformation` key-value pairs to store information
about the RDM purification results, namely `contributing_coeffs`, `contributing_rho_pqrs`, `non-purified-energy`, and `purified-energy`.

This ``AcceleratorDecorator`` implementation comes installed with the XACC-VQE plugin.

Improved Sampling Decorator
---------------------------
The ``ImprovedSampling`` decorator can be used to automatically execute quantum circuits multiple times
in order to increase the number of measurements performed. This decorator is used to increase the probability
of acquiring desired results from quantum acceleration.