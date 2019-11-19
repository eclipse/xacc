import xacc

def rosen(x):
    xx = (1.-x[0])**2 + 100*(x[1]-x[0]**2)**2
    return xx

def rosen_with_grad(x):
    g = [-2*(1-x[0]) + 400.*(x[0]**3 - x[1]*x[0]), 200 * (x[1] - x[0]**2)]
    xx = (1.-x[0])**2 + 100*(x[1]-x[0]**2)**2
    return xx, g

optimizer = xacc.getOptimizer('mlpack',{'mlpack-optimizer':'l-bfgs'})

r,p = optimizer.optimize(rosen_with_grad,2)

print('Result = ', r,p)

# cobyla = xacc.getOptimizer('nlopt',{ 'nlopt-optimizer':'cobyla',  'nlopt-maxeval':500})
# print('Result = ', result)
