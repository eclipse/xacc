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
#ifndef XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_
#define XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_

#include <string>
#include <iostream>
#include "XACCError.hpp"

enum class AcceleratorBitState {ZERO, ONE, UNKNOWN};
class AcceleratorBit {
public:
	AcceleratorBit() :state(AcceleratorBitState::UNKNOWN){}
	void update(int zeroOrOne) {
		std::cout << "HOWDY\n";
		state = (zeroOrOne == 0 ? AcceleratorBitState::ZERO : AcceleratorBitState::ONE);
	}
	AcceleratorBitState getState() {
		return state;
	}
protected:
	AcceleratorBitState state;
};

/**
 *
 * @author Alex McCaskey
 */
class AcceleratorBuffer {

public:
	AcceleratorBuffer(const std::string& str) :
			bufferId(str), bits(std::vector<AcceleratorBit> { }) {
	}
	AcceleratorBuffer(const std::string& str, const int N) :
			bufferId(str), bits(std::vector<AcceleratorBit>(N)) {
	}
	template<typename ... Indices>
	AcceleratorBuffer(const std::string& str, int firstIndex,
			Indices ... indices) :
			bufferId(str), bits(
					std::vector<AcceleratorBit>(1 + sizeof...(indices))) {
	}
	int size() {
		return bits.size();
	}

	std::string name() {
		return bufferId;
	}

	void resetBuffer() {
	}
	void updateBit(const int idx, int zeroOrOne) {
		bits[idx].update(zeroOrOne);
	}

	AcceleratorBitState getAcceleratorBitState(const int idx) {
		return bits[idx].getState();
	}

	virtual void print() {
	}
	virtual void print(std::ostream& stream) {
	}

	virtual ~AcceleratorBuffer() {}

protected:

	std::string bufferId;
	std::vector<AcceleratorBit> bits;
};


#endif /* XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_ */
