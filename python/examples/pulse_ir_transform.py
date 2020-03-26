import sys, os, json, numpy as np
import xacc

# The Hamiltonian JSON object (OpenPulse format)
hamiltonianJson = {
        "description": "Hamiltonian of a one-qubit system.\n",
        "h_str": ["-0.5*omega0*Z0", "omegaa*X0||D0"],
        "osc": {},
        "qub": {
            "0": 2
        },
        "vars": {
            "omega0": 0.0,
            "omegaa": 0.062832
        } 
}

# Create a pulse system model object 
model = xacc.createPulseModel()

# Load the Hamiltonian JSON (string) to the system model
loadResult = model.loadHamiltonianJson(json.dumps(hamiltonianJson))

if loadResult is True :
    qpu = xacc.getAccelerator('QuaC', {'system-model': model.name()})    
    channelConfigs = xacc.BackendChannelConfigs()
    channelConfigs.dt = 1.0
    channelConfigs.loFregs_dChannels = [0.0]
    model.setChannelConfigs(channelConfigs)

    # Get the XASM compiler
    xasmCompiler = xacc.getCompiler('xasm');
    # Composite to be transform to pulse
    ir = xasmCompiler.compile('''__qpu__ void f(qbit q) {
        X(q[0]);
    }''', qpu);
    program = ir.getComposites()[0]
    
    # Run the pulse IRTransformation 
    optimizer = xacc.getIRTransformation('quantum-control')
    optimizer.apply(program, qpu, {
        # Using the Python-contributed pulse optimizer
        # This will propagate to setOptions() then optimize()
        # calls on the optimizer implementation. 
        'method': 'ml_optimizer',
        'max-time': 100.0
    })








