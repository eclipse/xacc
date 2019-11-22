import xacc

src = '''
        .compiler xasm
            .circuit ansatz2
            .parameters x
            .qbit q
            X(q[0]);
            X(q[2]);
            ucc1(q, x[0]);'''

xacc.qasm(src)
#nsatz = xacc.getCompiled(xasm)