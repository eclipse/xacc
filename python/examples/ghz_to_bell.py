# Demonstrate mid-circuit measurement support
# GHZ to Bell conversion
# Adapted from:
# https://quantum-computing.ibm.com/docs/manage/systems/midcircuit-measurement/#GHZ-to-Bell-State

import xacc

# Mid-circuit measurement is supportted by:
# - Simulator: qpp, qsim, aer
# - Selected IBM backends.
# qpu = xacc.getAccelerator('aer', {"shots": 1024})
qpu = xacc.getAccelerator('qpp', {"shots": 1024})
q = xacc.qalloc(3)

xacc.qasm('''
.compiler xasm
.circuit ghz_to_bell
.qbit q
H(q[2]);
CX(q[2], q[1]);
CX(q[1], q[0]);
// Z -> X basis 
H(q[0]);
// Mid-circuit measurement => Bell state
Measure(q[0]);
CX(q[2], q[1]);
H(q[2]);
X(q[0]);
Measure(q[0]);
Measure(q[1]);
Measure(q[2]);
''')

f = xacc.getCompiled('ghz_to_bell')
qpu.execute(q, f)
print(q)
# Note: QPP is using a LSB bit-ordering
sim_counts_ghz_bell_1 = q.getMarginalCounts([0], xacc.BitOrder.LSB)
sim_counts_ghz_bell_2 = q.getMarginalCounts([1,2,3], xacc.BitOrder.LSB)
print("Sim meas 1 result: ", sim_counts_ghz_bell_1)
print("Sim meas 2 result: ", sim_counts_ghz_bell_2)