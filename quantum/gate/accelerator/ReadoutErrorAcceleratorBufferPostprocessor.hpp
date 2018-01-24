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

class ReadoutErrorAcceleratorBufferPostprocessor: public AcceleratorBufferPostprocessor {

protected:

	std::map<std::string, std::vector<int>> sites;
	std::vector<std::string> allTerms;
	IR& ir;

	double exptZ(double E_Z, double p01, double p10) {
		auto p_p = p01 + p10;
		auto p_m = p01 - p10;
		return (E_Z - p_m) / (1.0 - p_p);
	}

	double exptZZ(double E_ZZ, double E_ZI, double E_IZ, double ap01,
			double ap10, double bp01, double bp10, bool averaged = false);

	std::map<std::string, double> fix_assignments(
			std::map<std::string, double> oldExpects,
			std::map<std::string, std::vector<int>> sites,
			std::map<int, std::pair<double, double>> errorRates);

public:
	ReadoutErrorAcceleratorBufferPostprocessor(IR& i,
			std::map<std::string, std::vector<int>> sitesMap,
			std::vector<std::string> orderedTerms) :
			ir(i), sites(sitesMap), allTerms(orderedTerms) {
	}

	virtual std::vector<std::shared_ptr<AcceleratorBuffer>> process(
			std::vector<std::shared_ptr<AcceleratorBuffer>> buffers);
};
}
}

#endif
