/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#ifndef QUANTUM_GATE_ACCELERATOR_READOUTERRORACCELERATORBUFFERPOSTPROCESSOR_HPP_
#define QUANTUM_GATE_ACCELERATOR_READOUTERRORACCELERATORBUFFERPOSTPROCESSOR_HPP_

#include "IR.hpp"
#include "AcceleratorBufferPostprocessor.hpp"

namespace xacc {
namespace quantum {

class StaticExpectationValueBuffer : public AcceleratorBuffer {
protected:
	double expectationValue = 0.0;

public:

	StaticExpectationValueBuffer(const std::string& name, const int nBits,
			const double e) :
			AcceleratorBuffer(name, nBits), expectationValue(e) {
	}

	virtual const double getExpectationValueZ() {
		return expectationValue;
	}
};

class ReadoutErrorAcceleratorBufferPostprocessor : public AcceleratorBufferPostprocessor {

protected:

	std::map<std::string, int> measurements;
	std::map<std::string, int> extraKernels;
	IR& ir;

public:
	ReadoutErrorAcceleratorBufferPostprocessor(IR& i,
			std::map<std::string, int> extraKernelIndexMap,
			std::map<std::string, int> measurementIndexMap) : ir(i),
			extraKernels(extraKernelIndexMap), measurements(measurementIndexMap) {
	}

	virtual std::vector<std::shared_ptr<AcceleratorBuffer>> process(std::vector<std::shared_ptr<AcceleratorBuffer>> buffers);
};
}
}

#endif
