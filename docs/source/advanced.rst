Advanced
========

AcceleratorBuffer Execution Data
--------------------------------

Error Mitigation
----------------

Pulse-level Programming
-----------------------

Pulse-level results in AcceleratorBuffer
+++++++++++++++++++++++++++++++++++++++++
By default, if using the QuaC simulator backend, the following information is embedded into the Accelerator Buffer 
at the end of the simulation:

    - ``<O>``: expectation value of the number/occupation operator (``n``) on each qubit sub-system. This is an array of floating-point numbers, one entry for each qubit.
    
    - ``DensityMatrixDiags``: diagonal elements of the density matrix at the end of the simulation (length = ``dim^n``, ``dim`` is the dimension of sub-systems (2 for qubits, 3 for qutrits, etc.) and ``n`` is the number of sub-systems)

To optimize the execution speed, we don't record time-stepping data by default when integrating the master equation. 
This can be enabled manually by specifying a ``logging-period`` parameter when requesting the QuaC simulator as follows:

.. code:: python

    qpu = xacc.getAccelerator('QuaC', {'system-model': model.name(), 'logging-period': 0.1 }) 

Once requested, time-stepping data will be saved as a CSV file whose path is recorded in the Accelerator Buffer's ``csvFile`` field. 
The following data is recorded for each logging period: current time, signals on channels, expectation values of the number operator and Pauli operators on each qubit sub-system. 

Users can load the data, e.g. for plotting purposes, as follows:

.. code:: python

    csvFile = qubitReg['csvFile']
    data = np.genfromtxt(csvFile, delimiter = ',', dtype=float, names=True)
    # Each field can then be referred to by name
    time = data['Time']
    expectationX0 = data['X0']

Lab-frame vs. Rotating frame
+++++++++++++++++++++++++++++

Qubit (two-level) systems always have a ground-to-excited state transition frequency which corresponds to rotation around the z-axis of the excited state. 
Hence, driving signals are often mixed with a local-oscillator at the frequency in order to be in resonance with that transition.

In QuaC, this is done by setting the loFregs_dChannels array of the ``BackendChannelConfigs``:

.. code:: python

    channelConfigs = xacc.BackendChannelConfigs()
    channelConfigs.loFregs_dChannels = [4.98, 4.34] 

The QuaC accelerator will mix pulse instructions assigned on each channel with its corresponding LO signals at the specified frequency.

This is the most accurate form of simulation. However, it often requires a very fine time-stepping procedure due to the oscillatory nature of the modulated signals.
Users can opt for a simplified simulation setting whereby the system dynamics are specified in the rotating frame which is rotating at that transition frequency.

This can be done by:

    - Setting the transition frequency variable in the Hamiltonian JSON to zero.

    - Setting the LO frequency to zero.


Initial Population & Qubit Decay
+++++++++++++++++++++++++++++++++

By default, all qubits are initialized in the ground state. 
This can be changed by using the ``setQubitInitialPopulation`` function 
(first parameter is the qubit index and second parameter is the initial value of the number operator). 

Similarly, qubit decay rate can be specified by proving a T1 value via the ``setQubitT1`` function which corresponds to a Linbladian decay rate of 1/T1 in the master equation. 

.. code:: python

    model = xacc.createPulseModel()
    model.setQubitInitialPopulation(0, 1.0)
    model.setQubitT1(0, 1.0)


Higher-dimensional systems
++++++++++++++++++++++++++

Higher-dimensional systems are also supported by QuaC. 
The sub-system dimensions can be specified in the ``qub`` field of the Hamiltonian JSON.

For example, to model transmon qubits as three-level systems (e.g. to investigate qubit leakage), 
one can use the following Hamiltonian JSON.

.. code:: python

    hamiltonianJson = {
        "description": "Two-qutrit Hamiltonian",
        "h_latex": "",
        "h_str": ["w_0*O0", "w_1*O1", "d*O0*(O0-I0)", "d*O1*(O1-I1)", "J*(SP0*SM1 + SM0*SP1)", "O*(SM0 + SP0)||D0"],
        "osc": {},
        "qub": {
            "0": 3,
            "1": 3
        },
        "vars": {
            "w_0": 5.114,
            "w_1": 4.914,
            "d": -0.33,
            "J": 0.004,
            "O": 0.060
        }
    }

A few limitations of using non-qubit systems:

    - The shot-count distribution (binary bit strings) simulation is not supported. Users have access to the list of diagonal elements of the density matrix embedded in the Accelerator Buffer which contains the state distribution. 

    - Some automatic IR transformation services are not compatible with non-qubit systems.


Pulse-level IR Transformation
++++++++++++++++++++++++++++++

Automatic quantum-circuit-to-pulse transformation is a service within the XACC which can be used in conjunction with the QuaC simulator backend to find a pulse program representing arbitrary quantum circuit.

The XACC pulse-level IR transformation service can be requested by its name, which is ``quantum-control``.

.. code:: python

    optimizer = xacc.getIRTransformation('quantum-control')

In order to transform a quantum circuit (CompositeInstruction) into pulses, the optimizer will need access to an instance of the QuaC simulator backend which has been initialized with the system dynamics. 
Also, users will need to provide optimization options to the IR Transformation service. The available options are:

+------------------------+-----------------------------------------------------------------+--------------------------------------+
|   Parameter            |                  Parameter Description                          |             type                     |
+========================+=================================================================+======================================+
|    method              | Optimization method ('GOAT' or 'GRAPE')                         | string                               |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    max-time            | Max time horizon for pulse optimization                         | double                               |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    dt                  | Sample duration (GRAPE-only)                                    | double                               |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    control-params      | Control parameters to optimize (GOAT-only)                      | std::vector<string>                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    control-funcs       | Analytical forms of control functions (GOAT-only)               | std::vector<string>                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+
|    initial-parameters  | Initial values of control parameters (GOAT-only)                | std::vector<double>                  |
+------------------------+-----------------------------------------------------------------+--------------------------------------+

For example, we can transform a quantum circuit into an optimized pulse (Gaussian form) then
verify the result by simulating with QuaC:

.. code:: python

    # Get the XASM compiler
    xasmCompiler = xacc.getCompiler('xasm');
    # Composite to be transform to pulse
    ir = xasmCompiler.compile('''__qpu__ void f(qbit q) {
        Rx(q[0], 1.57);
    }''', qpu);
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

    # This composite should be a pulse composite now
    print(program)
    
    # Run the simulation of the optimized pulse program
    qubitReg = xacc.qalloc(1)
    qpu.execute(qubitReg, program)


Enable MPI
++++++++++

Users can enable MPI multi-processing on QuaC (C++ only) by setting the ``execution-mode`` option to ``MPI::<number of MPI processes>`` 
when requesting the QuaC accelerator. 

For example, to request a QuaC accelerator which will run on 4 MPI processes:

.. code:: cpp

    auto quaC = xacc::getAccelerator("QuaC", std::make_pair("execution-mode", "MPI::4") ... });

A few notes:

    - The compiled executable must be started on a single MPI process, i.e. ``-n 1`` (or ``-np 1``). QuaC runtime will spawn additional processes as required.

    - We recommend the Hydra process manager (``mpiexec.hydra``) that is installed with PETSc (``--download-mpich`` when ``configure`` PETSc).

    - MPI multi-processing should only be used for large systems (>5 qubits.) There is no performance gain when using MPI for small systems.




