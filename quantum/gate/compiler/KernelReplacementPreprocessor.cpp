#include "KernelReplacementPreprocessor.hpp"

namespace xacc {
namespace quantum {

KernelReplacementPreprocessor::KernelReplacementPreprocessor() {}

const std::string KernelReplacementPreprocessor::process(const std::string& source,
			std::shared_ptr<Compiler> compiler,
			std::shared_ptr<Accelerator> accelerator) {

	std::cout << "Running Kernel Replacement Preprocessor.\n";
	return source;
}

}
}
