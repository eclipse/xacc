import xacc
xacc.Initialize()
u = xacc.getIRGenerator('uccsd')
f = u.generate([2,4]).eval([0,0])
print('Before:\n',f.toString('q'))

opt = xacc.getIRTransformation('circuit-optimizer')

ir = xacc.gate.createIR()
ir.addKernel(f)

opt.transform(ir)

print('After:\n', ir.getKernels()[0].toString('q'))

xacc.Finalize()