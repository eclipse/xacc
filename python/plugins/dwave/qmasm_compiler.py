import xacc
import subprocess, os
from collections import defaultdict
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

TMP_FILE_NAME='.qmasm_tmp.qmasm'

@ComponentFactory("qmasm_compiler_factory")
@Provides("compiler")
@Property("_compiler", "compiler", "qmasm")
@Property("_name", "name", "qmasm")
@Instantiate("qmasm_compiler_instance")
class QmasmCompiler(xacc.Compiler):


    def __init__(self):
        xacc.Compiler.__init__(self)

    def name(self):
        return 'qmasm'

    def compile(self, src, qpu):
        import qmasm
        f = open(TMP_FILE_NAME, 'w')
        f.write(src)
        f.close()
        try:
            result = str(subprocess.check_output(['qmasm','-f','qbsolv',TMP_FILE_NAME]))
        except subprocess.CalledProcessError as e:
            print(e.output)
            print(e.returncode)
            return e.returncode
        os.remove(TMP_FILE_NAME)

        provider = xacc.getIRProvider('quantum')
        program = provider.createComposite('test', [], 'anneal')
        program.setTag('qubo')

        # Create q QUBO matrix
        startReading = False
        lines = result.split('\\n')
        q = dict()
        for l in lines:
            if startReading and l != '\'':
                bits_and_weight = l.split(' ')
                q[(int(bits_and_weight[0]), int(bits_and_weight[1]))] = float(bits_and_weight[2])
                inst = provider.createInstruction('dwqmi', [int(bits_and_weight[0]), int(bits_and_weight[1])], [float(bits_and_weight[2])])
                program.addInstruction(inst)
            if 'p qubo' in l:
                startReading = True

        # Convert to Ising
        # hdict = defaultdict(float)
        # j = {}
        # offset = 0
        # for (i, k), e in q.items():
        #     if i == k:
        #         hdict[i] += 0.5 * e
        #         offset += 0.5 * e
        #     else:
        #         j[(i, k)] = 0.25 * e
        #         hdict[i] += 0.25 * e
        #         hdict[k] += 0.25 * e
        #         offset += 0.25 * e
        # hdict = dict((k, v) for k, v in hdict.items() if v != 0)

        # if hdict:
        #     h = [0] * (1 + max(hdict))
        # else:
        #     h = []

        # for i, v in hdict.items():
        #     h[i] = v

        # j = dict((k, v) for k, v in j.items() if v != 0)

        # for i,hh in enumerate(h):
        #     inst = provider.createInstruction('dwqmi', [i,i], [hh])
        #     program.addInstruction(inst)
        # for (i,k),w in j.items():
        #     inst = provider.createInstruction('dwqmi', [i,k], [w])
        #     program.addInstruction(inst)

        ir = provider.createIR()
        ir.addComposite(program)
        return ir



