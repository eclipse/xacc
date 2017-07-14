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
class Preprocessor : public OptionsProvider {
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
	 * Return the name of this Preprocessor
	 *
	 * @return name The name of this preprocessor
	 */
	virtual const std::string getName() = 0;

	/**
	 * Return an empty options_description, this is for
	 * subclasses to implement.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		return std::make_shared<options_description>();
	}

	~Preprocessor() {}
};

/**
 * Preprocessor Registry is just an alias for a
 * Registry of Preprocessors.
 */
using PreprocessorRegistry = Registry<Preprocessor>;

/**
 * RegisterPreprocessor is a convenience class for
 * registering custom derived Preprocessor classes.
 *
 * Creators of Preprocessor subclasses create an instance
 * of this class with their Preprocessor subclass as the template
 * parameter to register their Preprocessor with XACC. This instance
 * must be created in the CPP implementation file for the Preprocessor
 * and at global scope.
 */
template<typename T>
class RegisterPreprocessor {
public:
	RegisterPreprocessor(const std::string& name) {
		PreprocessorRegistry::CreatorFunctionPtr f = std::make_shared<
				PreprocessorRegistry::CreatorFunction>([]() {
			return std::make_shared<T>();
		});
		PreprocessorRegistry::instance()->add(name, f);
	}
};

#define RegisterPreprocessor(TYPE) BOOST_DLL_ALIAS(TYPE::registerPreprocessor, registerPreprocessor)

}

#endif
