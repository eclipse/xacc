import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("chp_mc_accelerator_factory")
@Provides("accelerator")
@Property("_accelerator", "accelerator", "chp-mc")
@Property("_name", "name", "chp-mc")
@Instantiate("chp_mc_accelerator_instance")
class CHPMCAccelerator(xacc.Accelerator):
    def __init__(self):
        xacc.Accelerator.__init__(self)
        self.shots = 1024

    def initialize(self, options):
        if 'shots' in options:
            self.shots = options['shots']

    def name(self):
        return 'chp-mc'

    def execute(self, buffer, programs):

