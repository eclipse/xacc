import xacc
xacc.set_verbose(True)
opstr = '''(0.174073,0) Z1 Z3 +
           (0.1202,0) Z2 Z3 +
           (0.165607,0) Z1 Z2 +
           (0.165607,0) Z0 Z3 +
           (0.1202,0) Z0 Z1 +
           (-0.0454063,0) Y0 Y2 X1 X3 +
           (-0.220041,0) Z3 +
           (-0.106477,0) +
           (0.17028,0) Z0 +
           (-0.220041,0) Z1 +
           (0.17028,0) Z2 +
           (-0.0454063,0) X0 X2 Y1 Y3 +
           (0.0454063,0) X0 Y2 Y1 X3 +
           (0.168336,0) Z0 Z2 +
           (0.0454063,0) Y0 X2 X1 Y3'''

op = xacc.getObservable('pauli',opstr)

qpu = xacc.getAccelerator('qpp')

@xacc.qpu(algo='vqe', accelerator=qpu, observable=op)
def ansatz_vqe(q, theta0, theta1):
    uccsd(q,{"ne":2,"nq":4})

print(ansatz_vqe.getCompositeInstruction().toString())
buffer = xacc.qalloc(4)
ansatz_vqe(buffer, 0.,0.)
