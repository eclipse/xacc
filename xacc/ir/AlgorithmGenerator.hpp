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
#ifndef IR_ALGORITHMS_ALGORITHMGENERATOR_HPP_
#define IR_ALGORITHMS_ALGORITHMGENERATOR_HPP_

#include "Registry.hpp"
#include "Function.hpp"

namespace xacc {

/**
 * The AlgorithmGenerator interface provides a mechanism for
 * generating algorithms modeled as an XACC Function instance.
 *
 * @author Alex McCaskey
 */
class AlgorithmGenerator {

public:

	/**
	 * Implementations of this method generate a Function IR
	 * instance corresponding to the implementation's modeled
	 * algorithm. The algorithm is specified to operate over the
	 * provided bits.
	 *
	 * @param bits The bits this algorithm operates on
	 * @return function The algorithm represented as an IR Function
	 */
	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> bits) = 0;

	/**
	 * The destructor
	 */
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
