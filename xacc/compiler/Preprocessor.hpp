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
#ifndef XACC_COMPILER_PREPROCESSOR_HPP_
#define XACC_COMPILER_PREPROCESSOR_HPP_

#include "Compiler.hpp"

namespace xacc {

/**
 * The Preprocessor interface provides a mechanism for
 * processing quantum kernel source code before
 * compilation takes place. Realizations of this interface
 * implement the process method which takes as input the
 * source code to process, and the compiler and accelerator
 * this code is targeted at.
 *
 */
class Preprocessor : public OptionsProvider, public Identifiable {
public:

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
			std::shared_ptr<Accelerator> accelerator) = 0;

	/**
	 * Return an empty options_description, this is for
	 * subclasses to implement.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		return std::make_shared<options_description>();
	}

	virtual bool handleOptions(variables_map& map) {
		return false;
	}

	~Preprocessor() {}
};

}

#endif
