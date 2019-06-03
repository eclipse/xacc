from pelix.ipopo.decorators import (ComponentFactory, Property, Requires,
                                    BindField, UnbindField, Provides, Validate,
                                    Invalidate, Instantiate)
import xacc
from xacc import BenchmarkAlgorithm

# XACC Plugin-Generator template for XACC BenchmarkAlgorithms
# This produces a BenchmarkAlgorithm plugin implementation for use within the XACC benchmarking software suite.
#
# To add services provided by other plugins as dependencies, decorate the class with:
# @Requires(_internal_field, service_type)


@ComponentFactory("{benchmark_algorithm_name}_benchmark_factory")
@Provides("benchmark_algorithm")
@Property("_name", "name", "{benchmark_algorithm_name}")
@Instantiate("{benchmark_algorithm_name}_benchmark")
class {benchmark_algorithm_name}(BenchmarkAlgorithm):



    def execute(self, inputParams):
        pass



    def analyze(self, inputParams):
        pass
