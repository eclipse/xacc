import xacc


dwave = xacc.getAccelerator('dwave', {'shots':10})

@xacc.qpu(accelerator=dwave)
def test(q, bias, edge):
    dwqmi(q[0],q[0], bias)
    dwqmi(q[4],q[4], -bias)
    dwqmi(q[0],q[4], edge)

qq=xacc.qalloc(2)
qq.addExtraInfo('embedding', {0:[0], 1:[1]})
test(qq, 1.0, 0.5)

print(qq)

# def f(x):
#     q = xacc.qalloc()
#     test(q, x[0], x[1])
#     uniqueConfigs = q['unique-configurations']
#     return min(uniqueConfigs.values())

# opt = xacc.getOptimizer('nlopt')

# print(opt.optimize(f, 2))

