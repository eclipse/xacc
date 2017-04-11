/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
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
#ifndef XACC_ACCELERATOR_HPP_
#define XACC_ACCELERATOR_HPP_

#include <string>
#include <vector>
#include <array>
#include <bitset>
#include "spdlog/spdlog.h"
#include "IRTransformation.hpp"
#include "XACCError.hpp"
#include "AcceleratorBuffer.hpp"
#include "Registry.hpp"

namespace xacc {

/**
 * The types of Accelerators that XACC interacts with
 */
enum AcceleratorType {
	qpu_gate, qpu_aqc, npu
};

/**
 * The Accelerator class provides a high-level abstraction
 * for XACC's interaction with attached post-exascale
 * accelerators (quantum and neuromorphic processing units).
 *
 * Derived Accelerators must provide a valid execute implementation
 * that takes XACC IR and executes it on the attached hardware or
 * simulator.
 *
 * Derived Accelerators must provide a list of IRTransformation
 * instances that transform XACC IR to be amenable to execution
 * on the hardware.
 *
 * STORE ALLOCATED BUFFERS
 */
class Accelerator {

public:

	/**
	 * Return the type of this Accelerator.
	 *
	 * @return type The Accelerator type - Gate or AQC QPU, or NPU
	 */
	virtual AcceleratorType getType() = 0;

	/**
	 * Return any IR Transformations that must be applied to ensure
	 * the compiled IR is amenable to execution on this Accelerator.
	 * @return
	 */
	virtual std::vector<IRTransformation> getIRTransformations() = 0;

	/**
	 * Execute the provided XACC IR on this attached Accelerator.
	 *
	 * @param ir
	 */
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
			const std::shared_ptr<IR> ir) = 0;

	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId) = 0;

	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId, const int size) = 0;

	virtual std::shared_ptr<AcceleratorBuffer> getBuffer(const std::string& varid) {
		if (isValidBuffer(varid)) {
			return allocatedBuffers[varid];
		} else {
			XACCError("Could not find AcceleratorBuffer with id " + varid);
		}
	}

	virtual bool isValidBufferSize(const int NBits) = 0;

	/**
	 * Destructor
	 */
	virtual ~Accelerator() {
	}

protected:

	void storeBuffer(const std::string& id, std::shared_ptr<AcceleratorBuffer> b) {
		allocatedBuffers.insert(std::make_pair(id, b));
	}

private:

	std::map<std::string, std::shared_ptr<AcceleratorBuffer>> allocatedBuffers;

	bool isValidBuffer(const std::string& str) {
		return allocatedBuffers.find(str) != allocatedBuffers.end();
	}

};

using AcceleratorRegistry = Registry<Accelerator>;
template<typename T>
class RegisterAccelerator {
public:
	RegisterAccelerator(const std::string& name) {
		AcceleratorRegistry::instance()->add(name,
				(std::function<std::shared_ptr<xacc::Accelerator>()>) ([]() {
					return std::make_shared<T>();
				}));
	}
};
}
#endif
