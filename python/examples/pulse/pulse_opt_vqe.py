# This is a helper to compute the expectation value 
# of an observable from Aer pulse simulation state-vector.
# Aer returns the state vector in the *qutrit* basis,
# i.e. 3-level systems => 3^N length rather than the conventional 2^N.
# Hence, we need to have a custom method to compute the expectation.

# Note: the current pulse lib (cmd-def) and the Hamiltonian (used by Aer)
# has mismatches. Hence, the observe procedure by partial tomography (adding H, Rx, etc.)
# doesn't work (the pulse cmd-def for H doesn't work as expected on the Aer simulator)

import numpy as np
from scipy.linalg import block_diag
import xacc, json

# Pauli operators in an extended basis (e.g. qutrit)
def s(n_qubits, dim, site, pauli):
    """
    Parameters:
    -----------
    n_qubits: number of qubits \in (1,2,..)
    dim: dimension 
    site: site for pauli \in (0,1,..)
    pauli: 'X','Y','Z'
    
    Returns:
    --------
    single-site pauli matrix defined over tensor product hilbert space (arbitray dimension)
    """
    oplist = [np.eye(dim)]*n_qubits
    if pauli == 'X': spin_op = block_diag([[0,1],[1,0]],0*np.eye(dim-2))
    elif pauli == 'Y': spin_op = block_diag([[0,-1j],[1j,0]],0*np.eye(dim-2))
    elif pauli == 'Z': spin_op = block_diag([[1,0],[0,-1]],0*np.eye(dim-2))
    else: return TypeError
    oplist[site] = spin_op
    result = oplist[0]
    for i in range(1, n_qubits):
        result = np.kron(oplist[i], result)
    return result

def pauli_op_to_matrix(obs, n_qubits, dim = 3):
    """
    Parameters:
    -----------
    obs: xacc observable (Pauli type)
    n_qubits: number of qubits of the total system. Should be equal to the number of qubits on the device. 
    dim: qubit dimension
    
    Returns:
    --------
    The observable matrix in the extended space (qutrit)
    """
    def getTermCoeff(term):
        if term is None:
            return 0.0
        for op in term:
            coeff = op[1].coeff()
            return coeff
    
    result = getTermCoeff(obs.getIdentitySubTerm()) * np.eye(dim**n_qubits, dtype=complex)
    for term in obs.getNonIdentitySubTerms():
        coeff = getTermCoeff(term)
        termMat = coeff * np.eye(dim**n_qubits)
        for op in term:
            for idx in op[1].ops():
                opName = op[1].ops()[idx]
                termMat = termMat @ s(n_qubits, dim, idx, opName)
            result += termMat
    return result

def calculateExpVal(state_vec, observable):
    """
    Parameters:
    -----------
    state_vec: state vector (1D list/array)
    observable: observable matrix
    
    Returns:
    --------
    expectation value (real)
    """
    state_vec = np.array(state_vec)
    # Check dimension
    if (len(state_vec) != observable.shape[0] or len(state_vec) != observable.shape[1]):
        return ValueError
    temp = observable @ state_vec
    return np.real(state_vec.conj().T @ temp)

# Retrieve the state vector from accelerator buffer result
def getStateVectorFromBuffer(buffer):
    state_vec = buffer["state"]
    result = []
    for pair in state_vec:
        if (len(pair) != 2):
            return TypeError
        else:
            result.append(pair[0] + 1j * pair[1])
    return result


class PulseOptParams:
    def __init__(self, nb_qubits=2, nb_segments=2, total_length=100.0):
        self.nBits = nb_qubits
        self.nSegs = nb_segments
        self.amplitude = []
        self.freq = []
        self.tSeg = []
        for qIdx in range(nb_qubits):
            # Amplitude in the range of [0.0, 1.0]
            self.amplitude.append(np.random.rand(nb_segments))
            # Assuming the frequency shift in the range of [-1.0, 1.0]
            self.freq.append(np.zeros(nb_segments))
            rand_segs = np.random.rand(nb_segments)
            # Time segments: using a random sequence to divide the total time into
            # time windows (sum up to the total length)
            segments = [
                total_length * r / np.sum(rand_segs) for r in rand_segs
            ]
            self.tSeg.append(segments)

    # Returns the pulse samples driving a particular qubit.
    # (constructed from the current parameters)
    def getPulseSamples(self, qubit, dt):
        amps = self.amplitude[qubit]
        freqs = self.freq[qubit]
        time_windows = self.tSeg[qubit]
        # Note: this is hardcoded to square pulses (step function)
        time_list = np.arange(0.0, np.sum(time_windows), dt)
        # which time segment are we on?
        segment_idx = 0
        pulse_vals = []
        next_switching_time = time_windows[segment_idx]
        for time in time_list:
            if time > next_switching_time:
                # Switch to the next window
                segment_idx = segment_idx + 1
                next_switching_time = next_switching_time + time_windows[
                    segment_idx]
            # Retrieve the amplitude and frequencies
            amp = amps[segment_idx]
            freq = freqs[segment_idx]

            # Compute the sample:
            # Note: We treat the frequency param here [-1, 1]
            # as modulation before LO mixing (at resonance frequency)
            # We can change the LO freq. for each pulse, but for simplicity,
            # we pre-modulate the pulse (AER simulator doesn't support per-pulse LO freq. change)
            # i.e.
            # signal = pulse * exp(-i * (w0 + dw) * t) = pulse * exp(-i * dw * t) * exp(-i * w0 * t)
            # hence, pulse at different frequency can be transformed into [pulse * exp(-i * dw * t)]
            # to create LO freq. shift effect.
            pulse_val = amp * np.exp(-1j * 2.0 * np.pi * freq * time)
            pulse_vals.append(pulse_val)
        
        # Pulse length must be a multiple of 16 to be able to run on actual backend
        while (len(pulse_vals) % 16 != 0):
            # padding
            pulse_vals.append(0.0*1j) 
            
        return pulse_vals

# Query backend info (dt)
# Aer simulator
qpu = xacc.getAccelerator("aer:ibmq_bogota")
# IBM backend
#qpu = xacc.getAccelerator("ibm:ibmq_bogota")

backend_properties = qpu.getProperties()
config = json.loads(backend_properties["config-json"])
# Number of qubits of the backend
nbQubits = config["n_qubits"]
# Sampling time (dt)
dt = config["dt"]

# Simple test: single qubit
ham = xacc.getObservable("pauli", "1.0 Z0")
# Pulse parameter:
# Simple: single segment, i.e. square pulse.
pulse_opt = PulseOptParams(nb_qubits=1, nb_segments=1, total_length=100.0)
provider = xacc.getIRProvider("quantum")

# Optimization function:
# Note: We need to unpack the flattened x array into {amp, freq, time segment}.
def pulse_opt_func(x):
    # Handle both remote (ibm) and simulator (aer)
    isRemote = (qpu.name() == "ibm")
    # This is a one-segment optimization:
    amp = x[0]
    freq = x[1]
    # Update the params in the PulseOptParams
    pulse_opt.freqs = [[freq]]
    pulse_opt.amplitude = [[amp]]
    # Construct the pulse program:
    program = provider.createComposite("vqe_pulse_composite")
    # Create the pulse instructions
    # TODO: handle multiple channels....
    pulse_inst = provider.createInstruction(
        "pulse", [0], [], {
            "channel": "d0",
            "samples": pulse_opt.getPulseSamples(0, dt)
        })
    program.addInstruction(pulse_inst)
    energy = 0.0
    if not isRemote:
        buffer = xacc.qalloc(nbQubits)
        qpu.execute(buffer, program)
        state_vec = getStateVectorFromBuffer(buffer)
        obs = pauli_op_to_matrix(ham, buffer.size())
        energy = calculateExpVal(state_vec, obs)
        #print(state_vec, "->", energy)
    else:
        # Observe the pulse program: For use on IBM backend 
        fs_to_exe = ham.observe(program)
        # Execute
        for i in range(len(fs_to_exe)):
            fs = fs_to_exe[i]
            term = ham.getNonIdentitySubTerms()[i]
            coeff = 0.0
            for op in term:
                coeff = op[1].coeff().real
            buffer = xacc.qalloc(1)
            qpu.execute(buffer, fs)
            print(buffer)
            # print("Exp-Z =", buffer.getExpectationValueZ())
            energy = energy + coeff * buffer.getExpectationValueZ()
    print("Energy(", x, ") =", energy)
    return energy

# Run the optimization loop:
# Optimizer:
# Single segment: 1 amplitude and 1 freq
# Make sure we don't create a pulse with amplitude > 1.0 (error)
optimizer = xacc.getOptimizer('nlopt', {
    "lower-bounds": [-1.0, -1.0],
    "upper-bounds": [1.0, 1.0],
    "maxeval": 100
})
result = optimizer.optimize(pulse_opt_func, 2)
print("Optimization Result:", result)

# Not changing the freq., just varying the amplitude:
# Should see a Rabi oscillation....
# for ampl in np.linspace(0.0, 1.0, 100):
#     val = pulse_opt_func([ampl, 0.0])
#     print("E(", ampl, ") =", val)
