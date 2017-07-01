#include "KernelReplacementPreprocessor.hpp"
#include <boost/algorithm/string.hpp>
#include "AlgorithmGenerator.hpp"
#include <numeric>

namespace xacc {
namespace quantum {

KernelReplacementPreprocessor::KernelReplacementPreprocessor() {}

const std::string KernelReplacementPreprocessor::process(const std::string& source,
			std::shared_ptr<Compiler> compiler,
			std::shared_ptr<Accelerator> accelerator) {

	XACCInfo("Running Kernel Replacement Preprocessor.\n");

	// Find any occurrences of xacc::FUNCTION
	// Get the first occurrence
	std::string search("xacc::"), newSource = source;
	auto pos = source.find(search);
	std::vector<std::size_t> allPositions;

	// Repeat till end is reached
	while (pos != std::string::npos) {
		// Add position to the vector
		allPositions.push_back(pos);

		// Get the next occurrence from the current position
		pos = source.find(search, pos + search.size());
	}

	for (auto position : allPositions) {

		auto funcString = source.substr(position, source.find("\n", position)-position);
		boost::trim(funcString);

		std::vector<std::string> split;
		boost::split(split, funcString, boost::is_any_of("::"));

		auto funcName = split[2].substr(0, split[2].find("("));

		std::string bufName = "";
		auto parenLocation = split[2].find("(")+1;
		if (boost::contains(split[2], ",")) {
			bufName = split[2].substr(parenLocation, split[2].find(",")-parenLocation);
		} else {
			bufName = split[2].substr(parenLocation, split[2].find(")")-parenLocation);
		}

		auto buffer = accelerator->getBuffer(bufName);
		auto bufferSize = buffer->size();
		std::vector<int> qubits(bufferSize);
		std::iota(qubits.begin(), qubits.end(), 0);

		auto algoGen = AlgorithmGeneratorRegistry::instance()->create(funcName);

		auto algoKernel = algoGen->generateAlgorithm(qubits);

		auto translation = compiler->translate(bufName, algoKernel);

		boost::replace_all(translation, "\n", "\n   ");
		boost::replace_all(newSource, funcString, translation);
	}

	return newSource;
}

xacc::RegisterPreprocessor<xacc::quantum::KernelReplacementPreprocessor> KernelReplacement(
				"kernel-replacement");

}
}
