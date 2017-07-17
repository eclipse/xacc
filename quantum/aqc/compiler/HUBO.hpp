#ifndef QUANTUM_AQC_COMPILER_HUBO_HPP_
#define QUANTUM_AQC_COMPILER_HUBO_HPP_

#include "DWGraph.hpp"

namespace xacc {
namespace quantum {
class HUBO {

public:

	virtual std::shared_ptr<xacc::quantum::DWGraph> reduceToQubo(std::vector<InstructionParameter> parameters) = 0;
	virtual ~HUBO() {}
};

}

}

#endif
