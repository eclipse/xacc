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
#include "IRTransformation.hpp"
#include "XACCError.hpp"

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


class RuntimeType {
public:

};

template<typename T>
class ConcreteRuntimeType :  public RuntimeType {
public:
	T value;
};

/**
 *
 * @author Alex McCaskey
 */
class AcceleratorBuffer {
public:
	AcceleratorBuffer(const std::string& str) :
			bufferId(str) {
	}
	AcceleratorBuffer(const std::string& str, const int N) :
			bufferId(str), bufferSize(N) {
	}
	template<typename ... Indices>
	AcceleratorBuffer(const std::string& str, int firstIndex,
			Indices ... indices) :
			bufferId(str), bufferSize(1 + sizeof...(indices)) {
	}
	int size() {
		return bufferSize;
	}
	std::string name() {
		return bufferId;
	}

protected:

	int bufferSize = 0;
	std::string bufferId;
};

class IAccelerator {
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
	virtual void execute(const std::string& bufferId, const std::shared_ptr<IR> ir) = 0;

	/**
	 * Return the number of bits that the user most recently
	 * requested.
	 *
	 * @return nBits The number of requested bits
	 */
	virtual int getBufferSize(const std::string& id) = 0;

	virtual int getBufferSize() = 0;

	template<typename T>
	void setRuntimeParameter(const std::string& name, T param) {
		auto type = std::make_shared<ConcreteRuntimeType<T>>();
		type->value = param;
		runtimeParameters.insert(std::make_pair(name, type));
	}

	template<typename T>
	T getRuntimeParameter(const std::string& name) {
		auto rp = runtimeParameters[name];
		auto cp = std::static_pointer_cast<ConcreteRuntimeType<T>>(rp);
		return cp->value;
	}

	virtual ~IAccelerator() {}
protected:

	virtual bool isValidBufferSize(const int NBits) {return true;}

	std::map<std::string, std::shared_ptr<RuntimeType>> runtimeParameters;

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
template<typename BitsType>
class Accelerator : public IAccelerator {

	static_assert(std::is_base_of<AcceleratorBuffer, BitsType>::value, "Accelerators "
			"can only be instantiated with a valid AcceleratorBuffer type as "
			"the template parameter.");

	using BitsTypePtr = std::shared_ptr<BitsType>;

public:

	Accelerator() {
		totalSystemBuffer = std::make_shared<BitsType>("default");
		allocatedBuffers.insert(std::make_pair("default", totalSystemBuffer));
	}

	BitsTypePtr createBuffer(const std::string& varId) {
		if (isValidBufferVarId(varId)) {
			auto buffer = std::make_shared<BitsType>(varId);
			allocatedBuffers.insert(std::make_pair(varId, buffer));
			return buffer;
		} else {
			XACCError("Invalid buffer variable name.");
		}
	}

	BitsTypePtr createBuffer(const std::string& varId, const int size) {
		if (!isValidBufferVarId(varId)) {
			XACCError("Invalid buffer variable name.");
		}
		if (!isValidBufferSize(size)) {
			XACCError("Invalid buffer size.");
		}
		auto buffer = std::make_shared<BitsType>(varId, size);
		allocatedBuffers.insert(std::make_pair(varId, buffer));
		return buffer;
	}

	template<typename... Indices>
	BitsTypePtr createBuffer(const std::string& varId, int firstIndex,
			Indices ... indices) {
		if (!isValidBufferVarId(varId)) {
			XACCError("Invalid buffer variable name.");
		}
		if (!isValidBufferSize(sizeof...(indices) + 1)) {
			XACCError("Invalid buffer size.");
		}
		if (!validIndices(indices...)) {
			XACCError("Invalid buffer indices.");
		}
		auto buffer = std::make_shared<BitsType>(varId, firstIndex, indices...);
		allocatedBuffers.insert(std::make_pair(varId, buffer));
		return buffer;
	}

	/**
	 * Return the number of bits that the user most recently
	 * requested.
	 *
	 * @return nBits The number of requested bits
	 */
	virtual int getBufferSize(const std::string& id) {
		return allocatedBuffers[id]->size();
	}

	virtual int getBufferSize() {
		return allocatedBuffers["default"]->size();
	}

	BitsTypePtr getExistingBuffer(const std::string& varId) {
		return allocatedBuffers[varId];
	}


	/**
	 * Destructor
	 */
	virtual ~Accelerator() {}

protected:

	std::map<std::string, BitsTypePtr> allocatedBuffers;

	BitsTypePtr totalSystemBuffer;

	bool isValidBufferVarId(const std::string& str) {
		return allocatedBuffers.find(str) == std::end(allocatedBuffers);
	}

	template<typename... Indices>
	bool validIndices(int firstIndex, Indices... indices) {
		return false;
	}

};

}
#endif
