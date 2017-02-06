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

/**
 * Utility structs to help determine if
 * we have been given valid Vertices.
 */
template<typename T, typename = void>
struct is_valid_bitstype: std::false_type {
};
template<typename T>
struct is_valid_bitstype<T, decltype(std::declval<T>().N, void())> : std::true_type {
};

/**
 * The types of Accelerators that XACC interacts with
 */
enum AcceleratorType { qpu_gate, qpu_aqc, npu };

/**
 * The AcceleratorBits class provides a common
 * base class for allocating accelerator-specific
 * bit resources (for example, qubits). It takes an
 * integer template parameter at construction that indicates
 * the number of bits this AcceleratorBits models.
 *
 * Derived Accelerators should define a subclass of this
 * class that models the hardware.
 *
 * @author Alex McCaskey
 */
template<const int Number>
class AcceleratorBits {
public:
	/**
	 * Reference to the number of bits
	 */
	static constexpr int N = Number;

	/**
	 * Return the current state of the bits
	 * @return
	 */
	virtual std::bitset<(size_t) Number> measure() {
		return bits;
	}

	template<typename... SubBits>
	auto allocateSubset(
			SubBits ... subset) ->
			decltype(std::shared_ptr<AcceleratorBits<sizeof...(SubBits)>>()) {
	}
	virtual ~AcceleratorBits() {}

protected:

	/**
	 *  The bits themselves
	 */
	std::bitset<(size_t)Number> bits;

	std::vector<int> activeBits;

};

class IAccelerator : public qci::common::QCIObject {
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
	virtual void execute(const std::shared_ptr<IR> ir) = 0;

	/**
	 * Return the number of bits that the user most recently
	 * requested.
	 *
	 * @return nBits The number of requested bits
	 */
	virtual int getAllocationSize() = 0;

	/**
	 * Return the variable name provided upon bit allocation
	 * (for example - qreg for gate model quantum bits in (qbit qreg[2];))
	 *
	 * @return varName The name of the bits allocated.
	 */
	virtual const std::string getVariableName() = 0;
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
 */
template<typename TotalBits>
class Accelerator : public IAccelerator {

	static_assert(is_valid_bitstype<TotalBits>::value, "Derived BitsType parameter must contain N int member for number of bits.");
	static_assert(std::is_base_of<AcceleratorBits<TotalBits::N>, TotalBits>::value, "");

public:

	/**
	 * Allocate bit resources (if needed).
	 *
	 * @return bits The AcceleratorBits derived type
	 */
	std::shared_ptr<TotalBits> allocate(const std::string& variableNameId) {
		if (!canAllocate(TotalBits::N)) {
			QCIError("Error in allocated requested bits");
		}
		bits = std::make_shared<TotalBits>();
		NBitsAllocated = TotalBits::N;
		bitVarId = variableNameId;
		return bits;
	}

	/**
	 * Allocate some subset of Accelerator bit resources.
	 *
	 * @param variableNameId
	 * @param bitList
	 * @return
	 */
	template<typename... SubBits>
	auto allocate(const std::string& variableNameId,
			SubBits... bitList) -> decltype(std::declval<TotalBits>().allocateSubset(bitList...)) {
		// FIXME CHECK THAT THEY PASSED IN LIST OF INTS
		if (!canAllocate(sizeof...(SubBits))) {
			QCIError("Error in allocated requested bits");
		}

		auto subsetBits = bits->allocateSubset(bitList...);
		NBitsAllocated = (int) sizeof...(SubBits);
		bitVarId = variableNameId;

		return subsetBits;
	}

	/**
	 * Return the number of bits that the user most recently
	 * requested.
	 *
	 * @return nBits The number of requested bits
	 */
	virtual int getAllocationSize() {
		return NBitsAllocated;
	}

	/**
	 * Return the variable name provided upon bit allocation
	 * (for example - qreg for gate model quantum bits in (qbit qreg[2];))
	 *
	 * @return varName The name of the bits allocated.
	 */
	virtual const std::string getVariableName() {
		return bitVarId;
	}

	/**
	 * Destructor
	 */
	virtual ~Accelerator() {}

protected:

	/**
	 * The number of bits allocated upon the most
	 * recent user request for bit resources.
	 */
	int NBitsAllocated = 0;

	/**
	 * The variable name of the bits
	 */
	std::string bitVarId;

	/**
	 *
	 */
	std::shared_ptr<TotalBits> bits;

	/**
	 * Return true if this Accelerator can allocate
	 * the provided number of bits.
	 * @param NBits The number of bits to allocate
	 * @return canAllocate True if can allocate, false if not.
	 */
	virtual bool canAllocate(const int NBits) = 0;


};
}
#endif
