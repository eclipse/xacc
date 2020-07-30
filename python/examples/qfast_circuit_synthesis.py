import xacc,sys, numpy as np
# Example using XACC QFAST circuit synthesis
xacc.set_verbose(True)

# CCNOT matrix:
# This takes a 2-D numpy array.
ccnotMat = np.identity(8, dtype = np.cdouble)
ccnotMat[6][6] = 0.0
ccnotMat[7][7] = 0.0
ccnotMat[6][7] = 1.0
ccnotMat[7][6] = 1.0

# Get the MLPack Optimizer, default is Adam
optimizer = xacc.getOptimizer('mlpack')
composite = xacc.createCompositeInstruction('QFAST', { 
    'unitary' : ccnotMat,
    'optimizer': optimizer 
})

print(composite)

