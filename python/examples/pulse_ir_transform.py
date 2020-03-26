import sys, os, json, numpy as np
import xacc

# Adapt from:
# https://github.com/qutip/qutip-notebooks/blob/master/examples/control-grape-cnot.ipynb
# The Hamiltonian JSON object (OpenPulse format)
# see H_ops array in the notebook
hamiltonianJson = {
        "description": "Hamiltonian of a two-qubit system.\n",
        "h_str": ["a0*X0||D0", "a1*Y0||D1", "a2*Z0||D2", "a3*X1||D3", "a4*Y1||D4", "a5*Z1||D5", "a6*X0*X1||D6", "a7*Y0*Y1||D7", "a8*Z0*Z1||D8"],
        "osc": {},
        "qub": {
            "0": 2,
            "1": 2
        },
        "vars": {
           "a0": 1.0, 
           "a1": 1.0,
           "a2": 1.0,
           "a3": 1.0,
           "a4": 1.0,
           "a5": 1.0,
           "a6": 1.0,
           "a7": 1.0,
           "a8": 1.0
        } 
    }

# Create a pulse system model object 
model = xacc.createPulseModel()

# Load the Hamiltonian JSON (string) to the system model
loadResult = model.loadHamiltonianJson(json.dumps(hamiltonianJson))

if loadResult is True :
    qpu = xacc.getAccelerator('QuaC', {'system-model': model.name()})    
    channelConfigs = xacc.BackendChannelConfigs()
    T = 2 * np.pi 
    # Number of data samples
    R = 500
    channelConfigs.dt = T / (R + 1)

    # This example assumes that we have complete control over individual terms in the Hamiltonian
    # and don't need any LO modulation.
    channelConfigs.loFregs_dChannels = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    model.setChannelConfigs(channelConfigs)
    # Set control qubit to 1 -> initial state 10, expect to get 11
    model.setQubitInitialPopulation(0, 1.0)
    # Get the XASM compiler
    xasmCompiler = xacc.getCompiler('xasm');
    # Composite to be transform to pulse
    ir = xasmCompiler.compile('''__qpu__ void f(qbit q) {
        CNOT(q[0], q[1]);
    }''', qpu);
    program = ir.getComposites()[0]

    # Run the pulse IRTransformation 
    optimizer = xacc.getIRTransformation('quantum-control')
    optimizer.apply(program, qpu, {
        # Using the Python-contributed pulse optimizer
        # This will propagate to setOptions() then optimize()
        # calls on the optimizer implementation. 
        # Note: this is currently doing nothing
        'method': 'ml_optimizer',
        'max-time': T
    })
    # This composite should be a pulse composite now
    print(program)
    
    # Run the simulation of the optimized pulse program
    qubitReg = xacc.qalloc(2)
    qpu.execute(qubitReg, program)
    print(qubitReg)




