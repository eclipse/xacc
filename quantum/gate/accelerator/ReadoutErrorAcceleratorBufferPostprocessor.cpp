/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#include "ReadoutErrorAcceleratorBufferPostprocessor.hpp"
#include <boost/algorithm/string.hpp>
#include "XACC.hpp"

#include <boost/range/adaptor/sliced.hpp>

namespace xacc {
namespace quantum {

std::vector<std::shared_ptr<AcceleratorBuffer>> ReadoutErrorAcceleratorBufferPostprocessor::process(
		std::vector<std::shared_ptr<AcceleratorBuffer>> buffers) {

	// Goal here is to get all ap01, ap10, ..., and fix all expectation values
	int nQubits = ir.maxBit() + 1;
	std::string zeroStr = "";
	for (int i = 0; i < nQubits; i++) zeroStr += "0";

	int nKernels = ir.getKernels().size();
	int nRepititions = buffers.size() / nKernels;

	int nonIdentityKernels = 0;
	for (auto& k : ir.getKernels()) {
		if (k->nInstructions() > 0) {
			nonIdentityKernels++;
		}
	}

	if (buffers.size() % nonIdentityKernels != 0) {
		xacc::error("ReadoutError Postprocessor: Invalid number of buffers and kernels - " + std::to_string(buffers.size()) + ", " + std::to_string(nonIdentityKernels) );

	}

	std::vector<std::vector<std::shared_ptr<AcceleratorBuffer>>> bufvec;
	for (int i = 0; i < buffers.size(); i+=nonIdentityKernels) {
		std::vector<std::shared_ptr<AcceleratorBuffer>> sub(buffers.begin() + i, buffers.begin() + i + nonIdentityKernels);
		bufvec.push_back(sub);
	}

	std::vector<std::shared_ptr<AcceleratorBuffer>> fixedBuffers;

	for ( auto subList : bufvec) {
		std::vector<std::shared_ptr<Function>> nonIdentityKernels;
		for (int i = 0; i < nKernels; i++) {
			if (ir.getKernels()[i]->nInstructions() > 0) {
				nonIdentityKernels.push_back(ir.getKernels()[i]);
			}
		}

		auto nIk = nonIdentityKernels.size();

		std::map<int, std::pair<double,double>> errorRates;
		bool first = true;
		int counter = 0, qbitCount=0;
		std::vector<double> probs;
		for (int i = allTerms.size(); i < nIk; i++) {
			auto localBitStr = zeroStr;
			auto kernel = nonIdentityKernels[i];
			if (first) {
				// we have a p01 buffer
				auto bit = kernel->getInstruction(0)->bits()[0];
				localBitStr[nQubits - bit - 1] = '1';
				first = false;
			} else {
				// we have a p10 buffer
				first = true;
			}

			xacc::info(kernel->getName() + " - Computing measurement probability for bit string = " + localBitStr);

			probs.push_back(subList[i]->computeMeasurementProbability(localBitStr));
			counter++;

			if (counter == 2) {
				errorRates.insert(
						{ qbitCount, { std::isnan(probs[0]) ? 0.0 : probs[0],
								std::isnan(probs[1]) ? 0.0 : probs[1] } });
				counter = 0;
				qbitCount++;
				probs.clear();
			}
		}


		for (auto& kv : errorRates) {
			std::stringstream s, s2, s3;
			s << "Qubit " << kv.first << ": p01 = " << kv.second.first << ", p10 = " << kv.second.second;
			xacc::info(s.str());
		}


	// Return new AcceleratorBuffers subtype, StaticExpValAcceleratorBuffer that has static

		std::map<std::string, double> oldExpects;
		for (int i = 0; i < allTerms.size(); i++) {
			xacc::info("Raw Expectatations: " + allTerms[i] + " = " + std::to_string(subList[i]->getExpectationValueZ()));
			oldExpects.insert({allTerms[i], subList[i]->getExpectationValueZ()});
		}

		auto fixed = fix_assignments(oldExpects, sites, errorRates);

		// constant fixed expectation value from the calculation

		for (int i = 0; i < allTerms.size(); i++) {
			auto staticBuffer = std::make_shared<StaticExpectationValueBuffer>(subList[i]->name(), subList[i]->size(), fixed[allTerms[i]]);
			fixedBuffers.push_back(staticBuffer);
		}

	}
	return fixedBuffers;
}

std::map<std::string, double> ReadoutErrorAcceleratorBufferPostprocessor::fix_assignments(
		std::map<std::string, double> oldExpects,
		std::map<std::string, std::vector<int>> sites, // GIVES KEY X0X1 -> [0,1]
		std::map<int, std::pair<double, double>> errorRates) {

	std::map<std::string, double> newExpects;

	for (auto& kv : sites) {

		if (kv.first != "I") {
			if (kv.second.size() == 1) {
				double p01 = errorRates[kv.second[0]].first;
				double p10 = errorRates[kv.second[0]].second;
				newExpects.insert({kv.first, exptZ(oldExpects[kv.first], p01, p10)});
			} else if (kv.second.size() == 2) {

				std::stringstream s,t;
				s << kv.first[0] << kv.first[1];
				t << kv.first[2] << kv.first[3];
				auto k0 = s.str();
				auto k1 = t.str();

				double ap01 = errorRates[kv.second[0]].first;
				double ap10 = errorRates[kv.second[0]].second;
				double bp01 = errorRates[kv.second[1]].first;
				double bp10 = errorRates[kv.second[1]].second;

				std::stringstream s2;
				newExpects.insert({kv.first, exptZZ(oldExpects[kv.first], oldExpects[k0], oldExpects[k1], ap10, ap01, bp10, bp01)});

			} else {
				xacc::error("Correction for paulis with support on > 2 sites not implemented.");
			}
		}

	}

	return newExpects;
}

double ReadoutErrorAcceleratorBufferPostprocessor::exptZZ(double E_ZZ, double E_ZI, double E_IZ, double a01, double a10,
		double b01, double b10, bool averaged) {

    double a00 = 1 - a10;
    double a11 = 1 - a01;
    double b00 = 1 - b10;
    double b11 = 1 - b01;
    double E_II = 1;

	double E_ZZ_fixed = 0;
    if(averaged) {
        E_ZZ_fixed = (E_ZZ + (a01 - a10)*(- E_ZI - E_IZ + a01 - a10))/std::pow((-1 + a01 + a10),2);
    } else {
        E_ZZ_fixed = (-(2 - a00 * b01 - a11 * b01 - a00 * b10 - a11 * b10 +
        a01*a01 * (b00 * b01 + b10 * (2 * b01 + b11)) +
        a10*a10 * (b00 * b01 + b10 * (2 * b01 + b11)) +
        a01 * ((a00 + 2 * a10) * b01*b01 - 2 * b10 + a00 * b10*b10 + 2 * a10 * b10*b10 +
           b00 * (-1 + a11 * b01 + a00 * b10 + 2 * a10 * b10) - b11 +
           a11 * b10 * b11 + b01 * (-2 + 2 * a11 * b10 + a00 * b11 + 2 * a10 * b11)) +
         a10 * (a11 * b01*b01 - 2 * b10 + a11 * b10*b10 +
           b00 * (-1 + a00 * b01 + a11 * b10) - b11 + a00 * b10 * b11 +
           b01 * (-2 + 2 * a00 * b10 + a11 * b11))) * E_ZZ -
    a10 * b00 * E_ZI + a00 * b01 * E_ZI +
    a11 * b01 * E_ZI + a00 * a10 * b00 * b01 * E_ZI +
    a10*a10 * b00 * b01 * E_ZI - 2 * a00 * a11 * b01*b01 * E_ZI -
    a10 * a11 * b01*b01 * E_ZI - a00 * b10 * E_ZI -
    a11 * b10 * E_ZI + a10 * a11 * b00 * b10 * E_ZI +
    2 * a00 * a11 * b10*b10 * E_ZI + a10 * a11 * b10*b10 * E_ZI +
    a10 * b11 * E_ZI - a10 * a11 * b01 * b11 * E_ZI -
    a00 * a10 * b10 * b11 * E_ZI - a10*a10 * b10 * b11 * E_ZI -
    a10 * b00 * E_IZ - a00 * b01 * E_IZ +
    a11 * b01 * E_IZ + a00 * a10 * b00 * b01 * E_IZ +
    a10*a10 * b00 * b01 * E_IZ - a10 * a11 * b01*b01 * E_IZ -
    a00 * b10 * E_IZ + a11 * b10 * E_IZ -
    a10 * a11 * b00 * b10 * E_IZ +
    2 * a00 * a10 * b01 * b10 * E_IZ -
    a10 * a11 * b10*b10 * E_IZ - a10 * b11 * E_IZ +
    2 * a10*a10 * b00 * b11 * E_IZ -
    a10 * a11 * b01 * b11 * E_IZ +
    a00 * a10 * b10 * b11 * E_IZ + a10*a10 * b10 * b11 * E_IZ -
    a10 * b00 * E_II +
    a00 * b01 * E_II -
    a11 * b01 * E_II +
    a00 * a10 * b00 * b01 * E_II +
    a10*a10 * b00 * b01 * E_II +
    2 * a10 * a11 * b00 * b01 * E_II +
    a10 * a11 * b01*b01 * E_II -
    2 * a00 * a10 * a11 * b00 * b01*b01 * E_II -
    2 * a10*a10 * a11 * b00 * b01*b01 * E_II -
    a00 * b10 * E_II +
    a11 * b10 * E_II -
    a10 * a11 * b00 * b10 * E_II +
    2 * a00 * a10 * a11 * b00 * b01 * b10 * E_II +
    2 * a10*a10 * a11 * b00 * b01 * b10 * E_II -
    4 * a00 * a10 * a11 * b01*b01 * b10 * E_II -
    2 * a10*a10 * a11 * b01*b01 * b10 * E_II -
    a10 * a11 * b10*b10 * E_II +
    4 * a00 * a10 * a11 * b01 * b10*b10 * E_II +
    2 * a10*a10 * a11 * b01 * b10*b10 * E_II +
    a10 * b11 * E_II +
    a10 * a11 * b01 * b11 * E_II -
    2 * a10*a10 * a11 * b00 * b01 * b11 * E_II -
    a00 * a10 * b10 * b11 * E_II -
    a10*a10 * b10 * b11 * E_II -
    2 * a10 * a11 * b10 * b11 * E_II +
    2 * a10*a10 * a11 * b00 * b10 * b11 * E_II -
    2 * a00 * a10 * a11 * b01 * b10 * b11 * E_II -
    2 * a10*a10 * a11 * b01 * b10 * b11 * E_II +
    2 * a00 * a10 * a11 * b10*b10 * b11 * E_II +
    2 * a10*a10 * a11 * b10*b10 * b11 * E_II +
    a01*a01 * (-b10 * (2 * a00 * b01 * (-b01 + b10) * E_II +
           b11 * (E_ZI + E_IZ + (-1 - 2 * a00 * b01 -
                2 * a10 * b01 + 2 * a00 * b10 +
                2 * a10 * b10) * E_II)) +
       b00 * (2 * (a00 + a10) * b01*b01 * E_II -
          2 * b11 * (E_IZ + (a00 +
                2 * a10) * b10 * E_II) +
          b01 * (E_ZI - E_IZ + (-1 - 2 * a00 * b10 -
                2 * a10 * b10 + 2 * a00 * b11 +
                4 * a10 * b11) * E_II))) +
    a01 * (b11 * E_ZI - 2 * a10 * b01 * b11 * E_ZI -
       a11 * b10 * b11 * E_ZI - 2 * a11 * b01 * b10 * E_IZ +
       b11 * E_IZ - a11 * b10 * b11 * E_IZ +
       2 * a10 * a11 * b01*b01 * b10 * E_II -
       2 * a10 * a11 * b01 * b10*b10 * E_II -
       b11 * E_II +
       a11 * b10 * b11 * E_II -
       2 * a10*a10 * b01 * b10 * b11 * E_II +
       2 * a10 * a11 * b01 * b10 * b11 * E_II +
       2 * a10*a10 * b10*b10 * b11 * E_II -
       2 * a10 * a11 * b10*b10 * b11 * E_II +
       b00 * ((-1 + a11 * b01 + a00 * b10 +
             2 * a10 * b10) * E_ZI + (1 - a11 * b01 +
             a00 * b10) * E_IZ + (1 - 2 * a10*a10 * b01*b01 +
             2 * a10*a10 * b01 * b10 - 4 * a10*a10 * b01 * b11 + 4 * a10*a10 * b10 * b11 +
             a11 * (2 * a10 * b01*b01 - 2 * a10 * b10 * b11 +
                b01 * (-1 - 2 * a10 * b10 + 2 * a10 * b11)) +
             a00 * (-2 * (a10 - a11) * b01*b01 + b10 + 2 * a10 * b10 * b11 -
                2 * b01 * (1 + a11 * b10 +
                   a10 * (-b10 + b11)))) * E_II) +
        a00 * (-b01*b01 * (E_ZI - E_IZ + E_II +
             2 * a10 * b10 * E_II -
             4 * a11 * b10 * E_II) -
          b01 * (-2 * (a10 - 2 * a11) * b10*b10 * E_II +
             b11 * (E_ZI - E_IZ + E_II + 2 * a10 * b10 * E_II -
                2 * a11 * b10 * E_II)) +
          b10 * (2 * b11 * E_II +
             b10 * (E_ZI + E_IZ + E_II + 2 * a10 * b11 * E_II -
                2 * a11 * b11 * E_II)))))/(2 * ((-1 +
         a11 * (b01 + b10)) * (1 - a00 * (b01 + b10) +
         a10*a10 * (b00 + b10) * (b01 + b11) +
         a10 * (-b01 + b00 * (-1 + a00 * b01) - b10 + 2 * a00 * b01 * b10 - b11 +
            a00 * b10 * b11)) +
      a01*a01 * (b10 * (a00 * b01*b01 + (-1 + a00 * b10 + a10 * b10) * b11 +
            b01 * (-1 + a10 * b11 + a00 * (b10 + b11))) +
         b00 * ((a00 + a10) * b01*b01 + (-1 + a00 * b10 + 2 * a10 * b10) * b11 +
            b01 * (-1 + a10 * b10 + 2 * a10 * b11 + a00 * (b10 + b11)))) +
      a01 * (b01*b01 * (a10 * (-1 + a11 * b10) +
            a00 * (-1 + a10 * b10 + 2 * a11 * b10)) + (-1 + a00 * b10 +
            a10 * b10) * (-b11 + b10 * (-1 + a10 * b11 + a11 * b11)) +
         b01 * (1 - 2 * a10 * b11 + a10*a10 * b10 * b11 +
            a11 * b10 * (-2 + a10 * (b10 + b11)) +
            a00 * ((a10 + 2 * a11) * b10*b10 - b11 +
               b10 * (-2 + a10 * b11 + a11 * b11))) +
         b00 * (1 - a11 * b01 + a10*a10 * (b01 + b10) * (b01 + 2 * b11) +
            a00 * (b01 + b10) * (-1 + a11 * b01 + a10 * (b01 + b11)) +
            a10 * (a11 * b01*b01 - 2 * b10 - 2 * b11 + a11 * b10 * b11 +
               b01 * (-2 + a11 * (b10 + b11)))))));
    }
    return E_ZZ_fixed;

}

}
}



