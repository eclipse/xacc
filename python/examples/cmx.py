import xacc

accelerator = xacc.getAccelerator("qpp")

H = xacc.getObservable('pauli', '0.2976 + 0.3593 Z0 - 0.4826 Z1 + 0.5818 Z0 Z1 + 0.0896 X0 X1 + 0.0896 Y0 Y1')

expansion = 'Cioslowski'

order = 2

provider = xacc.getIRProvider('quantum')
ansatz = provider.createComposite('initial-state')
ansatz.addInstruction(provider.createInstruction('X', [0]))

buffer = xacc.qalloc(2)

cmx = xacc.getAlgorithm('cmx', {
                        'accelerator': accelerator,
                        'observable': H,
                        'ansatz': ansatz,
                        'cmx-order': order,
                        'expansion-type': expansion
                        })

cmx.execute(buffer)
print('Energy = ', buffer.getInformation('opt-val'))
