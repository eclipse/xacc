import xacc

class bell(xacc.IRGenerator):
    def __init__(self):
        xacc.IRGenerator.__init__(self)

    def name(self):
        return 'bell'

    def generate(self, params):
        xacc.qasm('''.compiler quil
        .function bell
        H 0
        CNOT 0 1
        MEASURE 0 [0]
        MEASURE 1 [1]
        ''')
        return xacc.getCompiled('bell')

irg = bell()
xacc.contributeService('bell', irg)

qpu = xacc.getAccelerator('local-ibm') #my_acc()
buffer = xacc.qalloc(2)

irg2 = xacc.getIRGenerator('bell')

@xacc.qpu(accelerator=qpu)
def function(buffer):
    bell()

function(buffer)

print(buffer)