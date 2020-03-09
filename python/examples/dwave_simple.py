import xacc


dwave = xacc.getAccelerator('dwave:DW_2000Q_5', {'shots':10})

@xacc.qpu(accelerator=dwave)
def test(q, bias, edge):
    dwqmi(q[0],q[0], bias)
    dwqmi(q[1],q[1], -bias)
    dwqmi(q[0],q[1], edge)

qq=xacc.qalloc(2)
qq.addExtraInfo('embedding', {0:[0], 1:[4]})
test(qq, 1.0, 0.5)

print(qq)


