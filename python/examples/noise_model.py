import xacc

# Noise model string for a simple depolarizing noise.
depol_json = """{"gate_noise": [{"gate_name": "X", "register_location": ["0"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "MSB"}"""
noise_model = xacc.getNoiseModel("json", {"noise-model": depol_json})
# Using Aer simulator in density matrix mode.
# Note: toJson() will convert the noise_model to IBM format to use w/ Aer
qpu = xacc.getAccelerator('aer', {
    "noise-model": noise_model.toJson(),
    "sim-type": "density_matrix"
})

# Define the quantum kernel in standard Python
@xacc.qpu(accelerator=qpu)
def test(q):
    X(q[0])

q = xacc.qalloc(1)

# run the circuit
test(q)
# print the density matrix (flattened)
print(q["density_matrix"])
