import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

import numpy as np

# Define a pulse optimization procedure
@ComponentFactory("py_pulse_optimizer_factory")
@Provides("optimizer")
@Property("_optimizer", "optimizer", "ml_optimizer")
@Property("_name", "name", "ml_optimizer")
@Instantiate("mlopt_instance")
class MlPulseOptimizer(xacc.Optimizer):
    def __init__(self):
        xacc.Optimizer.__init__(self)
        self.options = {}
        self.pulseOpts = None

    def name(self):
        return 'ml_optimizer'

    def setOptions(self, opts):
        self.pulseOpts = opts
        # These params are always present in the option map/dict
        # when using the IR transformation service.
        if 'dimension' in opts:
            self.dimension = opts['dimension']
        # Target unitary matrix
        if 'target-U' in opts:
            self.targetU = opts['target-U']
        # Static Hamiltonian
        if 'static-H' in opts:
            self.H0 = opts['static-H']
        # Control Hamiltonian (list)
        if 'control-H' in opts:
            self.Hops = opts['control-H']
        # Max time horizon
        if 'max-time' in opts:
            self.tMax = opts['max-time']
        # Pulse sample dt (number of samples over the time horizon)
        if 'dt' in opts:
            self.dt = opts['dt']
        # Note: if the method requires specific parameters,
        # we can require those params being specified in the IR transformation options
        # then propagate to here. 

    # This is main entry point that the high-level
    # IR transformation service will call.
    # This needs to return the pair (opt-val, pulses)
    # where opt-val is a floating point number (final value of the cost function);
    # pulses is a single array of all control pulses (one for each control-H term)
    # (appending one pulse array after another).
    def optimize(self):
        # TODO: we can now call any Python lib to
        # perform pulse optimization (marshalling the options/parameters if required)
        # For example, one can use Qutip pulse optimization:
        # Notes about data types: 
        # - targerU: flatten (row-by-row) U matrix into a 1-D array of complex numbers
        # - H0: string-type representation of the static Hamiltonian:
        # e.g.: 0.123 Z0Z1
        # - Hops: array of strings represent terms on the Hamiltonian which can be controlled.
        # Depending on the specific library we use for pulse optimization,
        # we may need to marshal these data types accordingly.
        print('Target U: ')
        print(self.targetU)
        print('Hops: ')
        print(self.Hops)
        nbSamples = (int)(self.tMax/self.dt)
        # DUMMY: just return a zero array
        pulse = np.zeros(nbSamples * len(self.Hops))
        # Return the final cost functional value and the array of pulse samples.
        return (0.0, pulse)