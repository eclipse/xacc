import xacc

opstr = '''(0.174073,0) Z2 Z3 +
           (0.1202,0) Z1 Z3 +
           (0.165607,0) Z1 Z2 +
           (0.165607,0) Z0 Z3 +
           (0.1202,0) Z0 Z2 +
           (-0.0454063,0) Y0 Y1 X2 X3 +
           (-0.220041,0) Z3 +
           (-0.106477,0) +
           (0.17028,0) Z0 +
           (-0.220041,0) Z2 +
           (0.17028,0) Z1 +
           (-0.0454063,0) X0 X1 Y2 Y3 +
           (0.0454063,0) X0 Y1 Y2 X3 +
           (0.168336,0) Z0 Z1 +
           (0.0454063,0) Y0 X1 X2 Y3'''

op = xacc.getObservable('pauli',opstr)

qpu = xacc.getAccelerator('tnqvm')

@xacc.qpu(algo='vqe', accelerator=qpu, observable=op)
def ansatz_vqe(buffer, *args):
    uccsd(nelectrons=2,nqubits=4)

buffer = xacc.qalloc(4)
ansatz_vqe(buffer, 0,0)