#ifndef XACC_COMPILER_KERNELREPLACEMENTPREPROCESSOR_HPP_
#define XACC_COMPILER_KERNELREPLACEMENTPREPROCESSOR_HPP_

#include "Preprocessor.hpp"
#include "Utils.hpp"

namespace xacc {

namespace quantum {

class KernelReplacementPreprocessor : public xacc::Preprocessor {

public:

	KernelReplacementPreprocessor();

	/**
	 * This method is to be implemented by subclasses to take in a
	 * kernel source string and process it in an isomorphic manner, and
	 * returns the processed source code.
	 *
	 * @param src The unprocessed kernel source code
	 * @param compiler The compiler being used to compile the code
	 * @param accelerator The Accelerator this code will be run on
	 *
	 * @return processedSrc The processed kernel source code
	 */
	virtual const std::string process(const std::string& source,
			std::shared_ptr<Compiler> compiler,
			std::shared_ptr<Accelerator> accelerator);

	/**
	 * Return the name of this Preprocessor
	 * @return name Preprocessor name
	 */
	virtual const std::string getName() {
		return "kernel-replacement";
	}

	virtual ~KernelReplacementPreprocessor() {}

};

}

}
#endif
