/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
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
