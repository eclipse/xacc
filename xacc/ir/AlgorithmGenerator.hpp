#ifndef IR_ALGORITHMS_ALGORITHMGENERATOR_HPP_
#define IR_ALGORITHMS_ALGORITHMGENERATOR_HPP_

#include "Registry.hpp"
#include "Function.hpp"

namespace xacc {

class AlgorithmGenerator {

public:

	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> qubits) = 0;

	virtual ~AlgorithmGenerator() {}
};


using AlgorithmGeneratorRegistry = Registry<AlgorithmGenerator>;

/**
 * RegisterAlgorithmGenerator is a convenience class for
 * registering custom derived AlgorithmGenerator classes.
 *
 * Creators of AlgorithmGenerator subclasses create an instance
 * of this class with their AlgorithmGenerator subclass as the template
 * parameter to register their AlgorithmGenerator with XACC. This instance
 * must be created in the CPP implementation file for the AlgorithmGenerator
 * and at global scope.
 */
template<typename T>
class RegisterAlgorithmGenerator {
public:
	RegisterAlgorithmGenerator(const std::string& name) {
		AlgorithmGeneratorRegistry::instance()->add(name,
				(std::function<std::shared_ptr<AlgorithmGenerator>()>) ([]() {
					return std::make_shared<T>();
				}));
	}
};
}
#endif
