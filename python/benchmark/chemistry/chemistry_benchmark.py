import xacc
import ast

from xacc import Benchmark
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
Validate, Invalidate, Instantiate


@ComponentFactory("chemistry_benchmark_factory")
@Provides("benchmark")
@Property("_benchmark", "benchmark", "chemistry")
@Property("_name", "name", "chemistry")
@Instantiate("chemistry_benchmark_instance")
class Chemistry(Benchmark):

    def execute(self, inputParams):
        qpu = xacc.getAccelerator(inputParams['XACC']['accelerator'], {'shots':inputParams['XACC']['shots'] if 'shots' in inputParams else 1024})

        H = None
        if inputParams['Observable']['name'] == 'pauli':
            obs_str = inputParams['Observable']['obs_str']
            H = xacc.getObservable('pauli', obs_str)

        elif inputParams['Observable']['name'] == 'fermion':
            obs_str = inputParams['Observable']['obs_str']
            H = xacc.getObservable('fermion', obs_str)

        elif inputParams['Observable']['name'] == 'psi4-frozen-core':
            basis = inputParams['Observable']['basis']
            geometry = inputParams['Observable']['geometry']
            fo = ast.literal_eval(inputParams['Observable']['fo'])
            ao = ast.literal_eval(inputParams['Observable']['ao'])
            H = xacc.getObservable('psi4-frozen-core', {'basis': basis,'geometry': geometry,
                                                        'frozen-spin-orbitals': fo, 'active-spin-orbitals': ao})

        #print('Ham: ', H)

        buffer = xacc.qalloc(H.nBits())
        optimizer = xacc.getOptimizer(inputParams['Optimizer']['name'])

        provider = xacc.getIRProvider('quantum')

        if inputParams['Ansatz']['name'] == 'xasm':
            print('in here')
            src = inputParams['Ansatz']['source']
            xacc.qasm(src)
            ansatz = xacc.getCompiled(inputParams['Ansatz']['circuit_name'])
            # ansatz.expand(xacc.HeterogeneousMap())
            print(ansatz.toString())
        else:
            print('here instead')
            ansatz = provider.createInstruction(inputParams['Ansatz']['ansatz'])
            ansatz = xacc.asComposite(ansatz)

        alg = xacc.getAlgorithm(inputParams['Benchmark']['algorithm'], {
                                'ansatz': ansatz,
                                'accelerator': qpu,
                                'observable': H,
                                'optimizer': optimizer,
                                })

        alg.execute(buffer)
        return buffer


    def analyze(self, buffer, inputParams):
        if inputParams['XACC']['readout_error'] == True:
            qpu = xacc.getAcceleratorDecorator('ro-error', qpu)
            

        print('Energy = ', buffer['opt-val'])
        print('Opt Params = ', buffer['opt-params'])




