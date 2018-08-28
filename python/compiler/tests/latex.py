import xacc
xacc.Initialize()
f = xacc.getIRGenerator('uccsd').generate([2,4])
f = f.eval([2,4])
print(xacc.functionToLatex(f))
xacc.Finalize()