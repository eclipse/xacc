Tutorials 
=========

\

Pulse Control Tutorial
----------------------

Here we describe how users can easily leverage XACC with the 
QuaC Open-Pulse Simulator to conduct optimal control experiments.
We currently support the following control algorithms:
GRAPE [1], CRAB [2], Krotov [3], GOAT [4], and DRAG [5] with near-term plans of supporting Deep Reinforcement Learning
and GRAFS [6].

Quick Start with Docker
++++++++++++++++++++++++

We have put together a docker image based on Ubuntu 18.04 that has all required
dependencies for building XACC and QuaC. Moreover, we have set this image up to serve an
Eclipse Theia IDE on ``localhost:3000``. To use this image run the following from some
scratch development directory:

.. code:: bash

   $ docker run --security-opt seccomp=unconfined --init -it -p 3000:3000 xacc/xacc-quac


Now navigate to ``localhost:3000`` in your web browser. This will open the 
Theia IDE and you are good to go. Open a terminal with ``ctrl + ```.

Basics of Manipulating Quantum Systems in XACC
++++++++++++++++++++++++++++++++++++++++++++++

We will begin by showing how to define a quantum system in XACC, and subsqequently demonstrate how to manipulate the system. 
The `next section <https://xacc.readthedocs.io/en/latest/tutorials.html#Optimizing-Controls-for-Quantum-Systems>`_
will cover optimizing controls for the system through the use of XACC's Quantum Control algorithms. 

Make sure to run the following imports:

.. code:: python

    import xacc
    import sys, os, json, numpy as np 
    
    # Alternative to the following two lines is to run 
    # from the IDE terminal: export PYTHONPATH=$PYTHONPATH:$HOME/.xacc
    from pathlib import Path
    sys.path.insert(1, str(Path.home()) + '/.xacc')

Each file then begins by defining the Hamiltonian of the system in jSON format:

.. code:: python

    hamiltonianJson = {
        "description": "One-qutrit Hamiltonian.",
        "h_latex": "",
        "h_str": ["(w - 0.5*alpha)*O0", "0.5*alpha*O0*O0", "O*(SM0 + SP0)||D0"],
        "osc": {},
        "qub": {
            "0": 3
        },
        "vars": {
            "w": 31.63772297724,
            "alpha": -1.47969,
            "O": 0.0314
        }
    }

with the above being an example of a single qutrit system. For more information on formatting the Hamiltonian, 
see `Advanced/Pulse-level Programming <https://xacc.readthedocs.io/en/latest/advanced.html>`_ . Alternatively, 
in `QuaC/xacc_examples/python <https://github.com/ORNL-QCI/QuaC/tree/xacc-integration/xacc_examples/python>`_, 
there are several example files outlining definitions for one-qubit, one-qutrit, two-qubit, 
and two-qutrit Hamiltonians that users can plug-and-play with. 

Next, a pulse model must be instantiated and the Hamiltonian is passed to the module by calling:

.. code:: python

    model = xacc.createPulseModel()
    loadResult = model.loadHamiltonianJson(json.dumps(hamiltonianJson))
    qpu = xacc.getAccelerator('QuaC', {'system-model': model.name()})
    channelConfig = xacc.BackendChannelConfigs()


See `Tutorials/Alternative Hamiltonian Declaration <https://xacc.readthedocs.io/en/latest/tutorials.html#Alternative-Hamiltonian-Declaration>`_
for declaring a Hamiltonian through default backends.

Let’s now define some of the parameters of the pulse, beginning with the total pulse time in nanoseconds, the number of samples, 
the time between the samples (dt), and the frequency of the driving envelope (typically chosen to be on resonance with the qubit):

.. code:: python

    T = 100
    nbSamples = 100
    # dt (time between data samples)
    channelConfig.dt = nbSamples / T 
    # Drive at resonance: 31.63772297724/(2pi)    
    channelConfig.loFregs_dChannels = [5.0353]

XACC currently supports several pre-installed pulse declarations:

+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|   Pulse Type           |           Parameters    |      Parameter Description                  |       type                     |
+========================+=========================+=============================================+================================+
|   SquarePulse          | nbSamples               | Number of samples in the pulse              | int                            |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|   GaussianPulse        | nbSamples               | Number of samples in the pulse              | int                            |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | sigma                   | Standard deviation of Gaussian distribution | double                         |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|   GaussianSquare       | duration                | Total pulse duration                        | int                            |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | amplitude               | Min/max amplitude of pulse                  | double                         |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | sigma                   | Standard deviation of Gaussian distribution | double                         |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | width                   | Width of pulse peak/trough                  | int                            |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|   DragPulse            | duration                | Total pulse duration                        | int                            |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | amplitude               | Amplitude of driving envelope               | double                         |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | sigma                   | Standard deviation of Gaussian distribution | double                         |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | beta                    | Correction amplitude                        | double                         |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+ 
|   SlepianPulse         | alpha_vector            | Weights for all k-orders of Slepians        | array                          |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | nbSamples               | Number of samples in the pulse              | int                            |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | in_bW                   | Half-bandwith of Slepian sequences          | double                         |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+
|                        | in_K                    | Max number of orders to use                 | int                            |
+------------------------+-------------------------+---------------------------------------------+--------------------------------+ 

which may be called as follows:

.. code:: python 

    channelConfig.addOrReplacePulse('square', xacc.SquarePulse(nbSamples))
    # channelConfig.addOrReplacePulse('gaussian', xacc.GaussianPulse(nSamples, sigma = 0.1))
    # etc.

XACC currently supports the use of Discrete Prolate Spheroidal Sequences [7], or Slepians, for creating
time and bandwidth limited discrete pulses. First applied directly to qubit control
in [6], these show promise at creating accurate and smooth controls in the NISQ era. 

.. code:: python

    # Typically want more samples here to maintain precision
    nbSamples = 500

    # Half-bandwidth \in (0.0, 0.5]
    in_bW = 0.02

    # Maximum number of Slepian orders to use 
    # Typically (2 * nbSamples * W) -- remember to make it an integer
    in_K = int(2 * nbSamples * in_bW)

    # Weight vector of length in_K as array. Just using one's
    # as an example, but for optimal control purposes, this vector
    # is the array that we seek to optimize.
    alpha_vector = np.ones(in_K)

    channelConfig.addOrReplacePulse('slepian', xacc.SlepianPulse(alpha_vector, nbSamples, in_bW, in_K))


Alternatively, one may define a custom pulse in numpy array format:

.. code:: python

    pulseData = np.ones(nbSamples)
    pulseName = 'custom'
    xacc.addPulse(pulseName, pulseData)

Now we allocate the amount of qubits needed for the program, create the program containing the pulse,
and set the channel to drive it on:

.. code:: python

    # Allocate qubits:
    q = xacc.qalloc(1)
    # Create the quantum program that contains the custom pulse
    # and the drive channel (D0) is set on the instruction
    provider = xacc.getIRProvider('quantum')
    prog = provider.createComposite('pulse')
    customPulse = provider.createInstruction(pulseName, [0])
    customPulse.setChannel('d0')
    prog.addInstruction(customPulse)

Finally, we instruct the program on what measurement we'd like it to make and execute the program:

.. code:: python 

    # Measure Q0
    prog.addInstruction(xacc.gate.create("Measure", [0]))
    qpu.execute(q, prog)


Returning the Fidelity
++++++++++++++++++++++

Depending on the backend that you're targetting, the gate operation you're attempting to do, 
and the number of qubits in your system, there are different ways to return the fidelity.

Case 1: Returning the probability of the |1> state for a single qubit:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: python 

    fidelity = q.computeMeasurementProbability('1')

Case 2: Returning the probability of the |1> and |2> states for a single qutrit:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: python

    fidelity = q['DensityMatrixDiags'][1]
    leakage = q['DensityMatrixDiags'][2]

Case 3: Fidelity Calculation using Density Matrices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this case, we can provide a target density matrix for the system (both the real and imaginary part)
and calculate the fidelity against that matrix. Here we outline the fidelity calculation for an X-Gate on a 
2-qubit system. 

.. code:: python  

    # Expected density matrix: rho = |10><10| for an X gate on the first qubit. 
    expectedDmReal = np.array([
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 0
    ], dtype = np.float64)
    
    expectedDmImag = np.zeros(16)
    
    # Add target density matrix info to the buffer before execution          
    q.addExtraInfo("target-dm-real", expectedDmReal)
    q.addExtraInfo("target-dm-imag", expectedDmImag)

    # Execute the program
    qpu.execute(q, prog)

    # Return the fidelity 
    fidelity = q["fidelity"]
    print("\nFidelity: {}".format(fidelity))

Case 4: Quantum Process Tomography:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The final method is to run XACC's Quantum Process Tomography algorithm on the system. In simulation, 
this method will take more time than the others listed above, but on actual hardware the difference 
in time will be marginal. The fidelity here is calculated against a user-provided target process matrix.

.. code:: python

    # This line should replace the previous provider.createComposite('pulse') call
    prog = provider.createComposite('pulse_qpt')

    # Create the Quantum Process Tomography Algorithm
    qpt = xacc.getAlgorithm('qpt', {'circuit': prog, 'accelerator': qpu, 'optimize-circuit': False})

    # Allocate qubit and execute
    q = xacc.qalloc(1)
    qpt.execute(q)

    # Target chi matrix (X-gate)
    chi_real_vec = [0., 0., 0., 0., 
                    0., 2., 0., 0., 
                    0., 0., 0., 0.,
                    0., 0., 0., 0.]
    fidelity = qpt.calculate('fidelity', q, {'chi-theoretical-real': chi_real_vec})

Case 4A: Quantum Process Tomography from the Gate-Level:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Instead of calcualting the target process matrix by hand, we can leverage XACC's 
Pulse-Level IR Transformation to convert a user-provided gate into its corresponding chi-matrix.

.. code:: python

    prog = provider.createComposite('pulse_qpt')

    # Get Quantum Process Tomography Algo
    qpt = xacc.getAlgorithm('qpt')

    # Compute Theoretical Chi Matrix
    q = xacc.qalloc(1)
    qpu = xacc.getAccelerator('q', {'shots': 10000})
    compiler = xacc.getCompiler('xasm')
    # Getting IR for an X gate
    ir = compiler.compile('''__qpu__ void f(qbit q) {X(q[0]);}''', None)
    qppCompositeInstr = ir.getComposites()[0]
    qpt.initialize({'circuit': qppCompositeInstr, 'accelerator': qpu})

    # Execute the algorithm and return real and imaginary parts of process matrix
    qpt.execute(q)
    chi_real_vec = q["chi-real"]
    chi_imag_vec = q["chi-imag"]
    


Optimizing Controls for Quantum Systems
+++++++++++++++++++++++++++++++++++++++

Using XACC's IR Transformation, similarly to in `Returning the Fidelity/Case 4A 
<https://xacc.readthedocs.io/en/latest/tutorials.html#case-4a-quantum-process-tomography-from-the-gate-level>`_, 
users can pass a Gate-Level instruction to the backend and return an optimized pulse with the algorithm of their 
choosing. The following is a short code snippet using GRAPE to construct a CNOT on a two-qubit system 
(for the full example, see `QuaC/xacc_examples/python/ir_transform_grape_cnot 
<https://github.com/ORNL-QCI/QuaC/blob/xacc-integration/xacc_examples/python/ir_transform_grape_cnot.py>`_).

.. code:: python

    # Assuming users have already defined the Hamiltonian, pulse system model, 
    # qpu = xacc.getAccelerator(), and the channelConfigs parameters

    # Get the XASM compiler
    xasmCompiler = xacc.getCompiler('xasm');

    # Composite to be transformed to pulse
    ir = xasmCompiler.compile('''__qpu__ void f(qbit q) {CNOT(q[0], q[1]);}''', qpu);
    program = ir.getComposites()[0]

    # Run the pulse IRTransformation 
    optimizer = xacc.getIRTransformation('quantum-control')
    optimizer.apply(program, qpu, {
        'method': 'GRAPE',
        'max-time': T,
        'dt': channelConfigs.dt
    })

    # Run the simulation of the optimized pulse program
    q = xacc.qalloc(2)
    qpu.execute(q, program)
    print(q)

After calling qpu.execute(), the `program` variable is no longer a gate, but is now the optimized pulse.
Similarly, here is how to optimize an X-gate on a single qubit using GOAT:

.. code:: python

    # Assuming users have already defined the Hamiltonian, pulse system model, 
    # qpu = xacc.getAccelerator(), and the channelConfigs parameters

    # Get the XASM compiler
    xasmCompiler = xacc.getCompiler('xasm');

    # Composite to be transform to pulse
    ir = xasmCompiler.compile('''__qpu__ void f(qbit q) {Rx(q[0], 1.57);}''', qpu);
    program = ir.getComposites()[0]

    # Run the pulse IRTransformation 
    optimizer = xacc.getIRTransformation('quantum-control')
    optimizer.apply(program, qpu, {
        'method': 'GOAT',
        'control-params': ['sigma'],
        # Gaussian pulse
        'control-funcs': ['exp(-t^2/(2*sigma^2))'],
        # Initial params
        'initial-parameters': [8.0],
        'max-time': 100.0
    })

See `Advanced/Pulse-evel Programming/Pulse-level IR Transformation <https://xacc.readthedocs.io/en/latest/advanced.html#pulse-level-ir-transformation>`_ 
for a more comprehensive list of each optimization method and its corresponding parameters.


Alternative Hamiltonian Declaration
+++++++++++++++++++++++++++++++++++

Currently, XACC provides a default two-qubit backend represented by the following Hamiltonian:

.. code:: python

    """
    {
        "description": "Two-qubit Hamiltonian",
        "h_str": ["_SUM[i,0,1,wq{i}*O{i}]", "_SUM[i,0,1,delta{i}*O{i}*(O{i}-I{i})]", "_SUM[i,0,1,omegad{i}*X{i}||D{i}]", "omegad1*X0||U0", "omegad0*X1||U1", "jq0q1*Sp0*Sm1", "jq0q1*Sm0*Sp1"],
        "osc": {},
        "qub": {
            "0": 2,
            "1": 2
        },
        "vars": {
            "wq0": 30.518812656662774, 
            "wq1": 31.238229295532093,
            "delta0": -2.011875935,
            "delta1": -2.008734343,
            "omegad0": -1.703999855,
            "omegad1": -1.703999855,
            "jq0q1": 0.011749557 
        }
    }
    """

Accessing this backend is as simple as:

.. code:: python

    qpu = xacc.getAccelerator('QuaC:Default2Q')

Additionally, this backend comes with the following pre-calibrated pulses:

+------------------------+-------------------------+-----------------------------------------------------+
|   Pulse Type           |   Gate Operation        |      Description                                    |
+========================+=========================+=====================================================+
|   Single Qubit         | X-Gate                  | pi/2 rotation over X-axis on Q0 or Q1               |
+------------------------+-------------------------+-----------------------------------------------------+
|                        | H-Gate                  | Hadamard Gate on Q0 or Q1                           | 
+------------------------+-------------------------+-----------------------------------------------------+
|                        | U3-Gate                 | U3 Operation on Q0 or Q1                            | 
+------------------------+-------------------------+-----------------------------------------------------+
|   Double Qubit         | CNOT                    | CNOT with U3 gates to correct local rotation errors | 
+------------------------+-------------------------+-----------------------------------------------------+

\
\
\
\

[1] `Data-driven gradient algorithm for high-precision quantum control <https://journals.aps.org/pra/abstract/10.1103/PhysRevA.97.042122>`_

[2] `Chopped random-basis quantum optimization <https://journals.aps.org/pra/abstract/10.1103/PhysRevA.84.022326>`_

[3] `Control of Photochemical Branching: Novel Procedures for Finding Optimal Pulses and Global Upper Bounds <https://link.springer.com/chapter/10.1007/978-1-4899-2326-4_24>`_

[4] `Tunable, Flexible, and Efficient Optimization of Control Pulses for Practical Qubits <https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.120.150401>`_

[5] `Simple Pulses for Elimination of Leakage in Weakly Nonlinear Qubits <https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.103.110501>`_ 

[6] `Quantum optimal control via gradient ascent in function space and the time-bandwidth quantum speed limit <https://journals.aps.org/pra/abstract/10.1103/PhysRevA.97.062346>`_

[7] `Prolate spheroidal wave functions, fourier analysis, and uncertainty — V: the discrete case <https://ieeexplore.ieee.org/document/6771595?arnumber=6771595&tag=1>`_