import xacc

# This example demonstrates how one might create 
# a new XACC InstructionVisitor in Python, 
# this demo shows a Visitor that counts the 
# number of each gate seen in the IR Tree.

H = xacc.getObservable('pauli', '')
for i,j in [[0,1], [1,2], [2,0]]:
    H += 0.5 * xacc.getObservable('pauli', '1.0 I - Z{} Z{}'.format(i,j))
qaoa_ansatz = xacc.createComposite('qaoa')
qaoa_ansatz.expand({'nbQubits':H.nBits(), 
                    'nbSteps':1, 
                    'cost-ham':H, 
                    'parameter-scheme':'Standard'})
print(qaoa_ansatz.toString())

class CountGatesOfTypePyVisitor(xacc.quantum.AllGateVisitor):
    def __init__(self):
        xacc.quantum.AllGateVisitor.__init__(self)
        self.count_dict = {}
            
    def visit(self, node):

        if node.name() in self.count_dict:
            self.count_dict[node.name()] += 1
        else:
            self.count_dict[node.name()] = 1

my_visitor = CountGatesOfTypePyVisitor()

inst_iter = xacc.InstructionIterator(qaoa_ansatz)
while inst_iter.hasNext():
    inst = inst_iter.next()
    if not inst.isComposite():
        inst.accept(my_visitor)

print(my_visitor.count_dict) 