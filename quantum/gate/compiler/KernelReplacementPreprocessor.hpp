/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef XACC_COMPILER_KERNELREPLACEMENTPREPROCESSOR_HPP_
#define XACC_COMPILER_KERNELREPLACEMENTPREPROCESSOR_HPP_

#include "Preprocessor.hpp"
#include "Utils.hpp"

namespace xacc {

namespace quantum {

/**
 * The KernelReplacementPreprocessor is a preprocessor
 * for gate model quantum computing that analyzing
 * quantum kernel source code and looks for occurrences
 * of 'xacc::FUNCTION' strings (for example, xacc::QFT(qreg)).
 *
 * Once one of these occurrences is found, this preprocessor
 * queries the AlgorithmGenerator Registry for any available
 * AlgorithmGenerators with that name. If such an AlgorithmGenerator
 * exists, it generates the XACC IR instance for that algorithm,
 * translates the IR to the Compiler's source code representation,
 * and replaces the 'xacc::FUNCTION' occurrence with the
 * new source code.
 */
class KernelReplacementPreprocessor : public xacc::Preprocessor {

public:

	KernelReplacementPreprocessor();

	/**
	 * This method replaces xacc::FUNCTION references with actual
	 * Compiler-specific source code.
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

	virtual const std::string name() const {
		return "kernel-replacement";
	}

	virtual const std::string description() const {
		return "The Kernel Replacement Preprocessor is a preprocessor for gate model "
				"kernel code that searches for occurrences of xacc::FUNCTION and "
				"replaces it with the IR for that function.";
	}


};

}

}
#endif
