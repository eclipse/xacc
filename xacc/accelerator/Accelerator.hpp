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
#include "IRTransformation.hpp"
#include <array>
#include <bitset>

namespace xacc {

template<const int Number>
class AcceleratorBits {
public:
	static constexpr int N = Number;
	std::bitset<(size_t)Number> bits;
	std::bitset<(size_t)Number> toBits() {
		return bits;
	}

};

/**
 *
 */
class Accelerator {

public:
	enum AcceleratorType { qpu_gate, qpu_aqc, npu };

	virtual AcceleratorType getType() = 0;

	virtual std::vector<IRTransformation> getIRTransformations() = 0;

	virtual void execute(const std::shared_ptr<IR> ir) = 0;

	/**
	 *
	 * @return
	 */
	template<typename BitsType>
	BitsType allocate(const std::string& variableNameId) {
		static_assert(std::is_base_of<AcceleratorBits<BitsType::N>, BitsType>::value, "");
		if (!canAllocate(BitsType::N)) {
			QCIError("Error in allocated requested bits");
		}
		BitsType bits;
		NBitsAllocated = BitsType::N;
		bitVarId = variableNameId;
		return bits;
	}

	virtual int getAllocationSize() {
		return NBitsAllocated;
	}

	virtual const std::string getVariableName() {
		return bitVarId;
	}

	virtual ~Accelerator() {}

protected:

	int NBitsAllocated = 0;

	std::string bitVarId;

	virtual bool canAllocate(const int NBits) = 0;
};
}
#endif
