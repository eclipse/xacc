import xacc
def rosen(x):
    return (1.-x[0])**2 + 100*(x[1]-x[0]**2)**2

optimizer = xacc.getOptimizer('nlopt', {'nlopt-maxeval':10000})
f = xacc.OptFunction(rosen, 2)

result = optimizer.optimize(f)

print(result)