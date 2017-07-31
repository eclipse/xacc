
#ifndef QUANTUM_AQC_COMPILER_PARAMETERSETTER_HPP_
#define QUANTUM_AQC_COMPILER_PARAMETERSETTER_HPP_

#include "DWGraph.hpp"
#include "DWQMI.hpp"
#include "Embedding.hpp"
#include "Identifiable.hpp"

namespace xacc {
namespace quantum {

class __attribute__((visibility("default"))) ParameterSetter : public Identifiable {

public:

	virtual std::list<std::shared_ptr<DWQMI>> setParameters(
			std::shared_ptr<DWGraph> problemGraph,
			std::shared_ptr<AcceleratorGraph> hardwareGraph,
			Embedding embedding) = 0;

	virtual ~ParameterSetter() {}
};


}
}

#endif
