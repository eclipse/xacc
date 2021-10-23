Ion Trap Testbed Backend
========================

This is a backend that targets the quantum testbed developed by the CIPHER
[Quantum Systems Division][1] at Georgia Tech Research Institute (GTRI). The
following 2016 paper describes the testbed:
https://doi.org/10.1088/1367-2630/18/2/023048. However, due to recent hardware
upgrades, we assume that the gate beam is now tightly-focused enough for
single-ion addressing. (That is, such that the cascading scheme is no longer
necessary for single-qubit gates.) We also assume that the MS phases are 0,
meaning the native entangling gate is an XX-Ising gate. We also do not consider
the MS echo scheme described in the paper.

This is designed for modified version of the control software, which is written
in IGOR Pro and is not currently public.

Please contact Austin Adams at <aja@gatech.edu> if you have any questions about
this backend.

[1]: https://www.gtri.gatech.edu/focus-areas/quantum-computing-sensing
