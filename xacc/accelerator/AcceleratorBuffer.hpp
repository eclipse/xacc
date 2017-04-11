/*
 * AcceleratorBuffer.hpp
 *
 *  Created on: Apr 11, 2017
 *      Author: aqw
 */

#ifndef XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_
#define XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_

#include <string>
#include "spdlog/spdlog.h"

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

	virtual void print() {
	}
	virtual void print(std::ostream& stream) {
	}

	virtual ~AcceleratorBuffer() {}

protected:

	int bufferSize = 0;
	std::string bufferId;
};


#endif /* XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_ */
