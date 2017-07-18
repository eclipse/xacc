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
#ifndef QUANTUM_AQC_IR_DWIR_HPP_
#define QUANTUM_AQC_IR_DWIR_HPP_

#include "IR.hpp"
#include "DWGraph.hpp"

namespace xacc {
namespace quantum {

class DWIR : public virtual IR {

public:
	/**
		 * Return a assembly-like string representation of this
		 * intermediate representation
		 * @return
		 */
		virtual std::string toAssemblyString(const std::string& kernelName, const std::string& accBufferVarName) {
			return "";
		}

		/**
		 * Persist this IR instance to the given
		 * output stream.
		 *
		 * @param outStream
		 */
		virtual void persist(std::ostream& outStream) {
			return;
		}

		/**
		 * Create this IR instance from the given input
		 * stream.
		 *
		 * @param inStream
		 */
		virtual void load(std::istream& inStream) {

		}

		virtual void addKernel(std::shared_ptr<Function> kernel) {
			kernels.push_back(kernel);

		}

		virtual bool kernelExists(const std::string& name) {
			return std::any_of(kernels.cbegin(), kernels.cend(),
					[=](std::shared_ptr<Function> i) {return i->getName() == name;});
		}

		virtual std::shared_ptr<Function> getKernel(const std::string& name) {
			for (auto f : kernels) {
				if (f->getName() == name) {
					return f;
				}
			}
			XACCError("Invalid kernel name.");
		}

		virtual std::vector<std::shared_ptr<Function>> getKernels() {
			return kernels;
		}

protected:

		/**
			 * Reference to this QIR's list of quantum functions
			 */
			std::vector<std::shared_ptr<Function>> kernels;
};

}
}

#endif /* QUANTUM_AQC_IR_DWIR_HPP_ */
