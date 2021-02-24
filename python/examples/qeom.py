import xacc

accelerator = xacc.getAccelerator("qpp")

buffer = xacc.qalloc(4)

opstr = '''
(-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 +
(-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 +
(0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 +
(0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 +
(-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 +
(0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 +
(-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 +
(-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 +
(0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 +
(0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 +
(-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 +
(0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 +
(0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 +
(0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 +
(0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 +
(0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 +
(0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 +
(0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 +
(-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 +
(0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 +
(0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 +
(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.7080240981,0)
'''
H = xacc.getObservable('fermion', opstr)

pool = xacc.quantum.getOperatorPool("singlet-adapted-uccsd")
pool.optionalParameters({"n-electrons": 2})
pool.generate(buffer.size())
provider = xacc.getIRProvider('quantum')
ansatz = provider.createComposite('initial-state')
ansatz.addInstruction(provider.createInstruction('X', [0]))
ansatz.addInstruction(provider.createInstruction('X', [2]))
ansatz.addVariable("x0")
for inst in pool.getOperatorInstructions(2, 0).getInstructions():
    ansatz.addInstruction(inst)
kernel = ansatz.eval([0.0808])

qeom = xacc.getAlgorithm('qeom', {
                        'accelerator': accelerator,
                        'observable': H,
                        'ansatz': kernel,
                        'n-electrons': 2
                        })

qeom.execute(buffer)
spectrum = buffer.getInformation("excitation-energies")
print(spectrum)
