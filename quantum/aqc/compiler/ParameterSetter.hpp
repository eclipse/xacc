
#ifndef QUANTUM_AQC_COMPILER_PARAMETERSETTER_HPP_
#define QUANTUM_AQC_COMPILER_PARAMETERSETTER_HPP_

#include "DWGraph.hpp"
#include "DWQMI.hpp"

namespace xacc {
namespace quantum {

class ParameterSetter {

public:

	virtual std::list<std::shared_ptr<DWQMI>> setParameters(
			std::shared_ptr<DWGraph> problemGraph,
			std::shared_ptr<AcceleratorGraph> hardwareGraph,
			std::map<int, std::list<int>> embedding) = 0;

	virtual ~ParameterSetter() {}
};

/**
 * ParameterSetter Registry is just an alias for a
 * Registry of ParameterSetters.
 */
using ParameterSetterRegistry = Registry<ParameterSetter>;

/**
 * RegisterParameterSetter is a convenience class for
 * registering custom derived ParameterSetter classes.
 *
 * Creators of ParameterSetter subclasses create an instance
 * of this class with their ParameterSetter subclass as the template
 * parameter to register their ParameterSetter with XACC. This instance
 * must be created in the CPP implementation file for the ParameterSetter
 * and at global scope.
 */
template<typename T>
class RegisterParameterSetter {
public:
	RegisterParameterSetter(const std::string& name) {
		ParameterSetterRegistry::CreatorFunctionPtr f = std::make_shared<
				ParameterSetterRegistry::CreatorFunction>([]() {
			return std::make_shared<T>();
		});
		ParameterSetterRegistry::instance()->add(name, f);
	}
};



}
}

#endif
