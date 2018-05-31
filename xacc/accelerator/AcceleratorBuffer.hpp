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
#ifndef XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_
#define XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_

#include <boost/dynamic_bitset.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include "Utils.hpp"

namespace xacc {

/**
 * The AcceleratorBuffer models an allocated buffer of
 * bits that are operated on by a kernel. As such,
 * the AcceleratorBuffer's primary role is to store
 * Accelerator execution results.
 *
 * @author Alex McCaskey
 */
class AcceleratorBuffer {

public:

	/**
	 * The Constructor
	 */
	AcceleratorBuffer(const std::string& str, const int N) :
			bufferId(str), nBits(N) {
	}

	/**
	 * The Constructor, takes as input the name of this buffer,
	 * and the bit indices to model.
	 *
	 * @param str The name of the buffer
	 * @param firstIndex The first bit index
	 * @param indices The remaining bit indices
	 */
	template<typename ... Indices>
	AcceleratorBuffer(const std::string& str, int firstIndex,
			Indices ... indices) :
			bufferId(str), nBits(1 + sizeof...(indices)) {
	}

	/**
	 * The copy constructor
	 */
	AcceleratorBuffer(const AcceleratorBuffer& other) :
			nBits(other.nBits), bufferId(other.bufferId) {
	}

	/**
	 * Return the number of bits in this buffer.
	 *
	 * @return size The number of bits in this buffer
	 */
	const int size() const {
		return nBits;
	}

	/**
	 * Return this AcceleratorBuffer's name
	 *
	 * @return name The name of this AcceleratorBuffer
	 */
	const std::string name() const {
		return bufferId;
	}

	/**
	 * Reset the stored measured bit strings.
	 */
	virtual void resetBuffer() {
		measurements.clear();
		bitStringToCounts.clear();
	}

	virtual void appendMeasurement(const std::string& measurement) {
		measurements.push_back(boost::dynamic_bitset<>(measurement));
		std::stringstream ss;
		ss << measurement;
		bitStringToCounts[ss.str()]++;
	}

	/**
	 * Add a measurement result to this Buffer
	 *
	 * @param measurement The measurement result
	 */
	virtual void appendMeasurement(const boost::dynamic_bitset<>& measurement) {
		measurements.push_back(measurement);
		std::stringstream ss;
		ss << measurement;
		bitStringToCounts[ss.str()]++;
	}

	virtual void appendMeasurement(const boost::dynamic_bitset<>& measurement, const int count) {
		std::stringstream ss;
		ss << measurement;
		bitStringToCounts[ss.str()] = count;
		for (int i = 0; i < count; i++) measurements.push_back(measurement);
		return;
	}

	virtual double computeMeasurementProbability(const std::string& bitStr) {
		return (double) bitStringToCounts[bitStr] / (double)measurements.size();
	}

	/**
	 * Compute and return the expectation value with respect
	 * to the Pauli-Z operator. Here we provide a base implementation
	 * based on an ensemble of measurement results. Subclasses
	 * are free to implement this as they see fit, ie, for simulators
	 * use the wavefunction.
	 *
	 * @return expVal The expectation value
	 */
	virtual const double getExpectationValueZ() {
		double aver = 0.0;
		auto has_even_parity = [](unsigned int x) -> int {
		    unsigned int count = 0, i, b = 1;
		    for(i = 0; i < 32; i++){
		        if( x & (b << i) ){count++;}
		    }
		    if( (count % 2) ){return 0;}
		    return 1;
		};

		for (auto& kv : bitStringToCounts) {
			int i = std::stoi(kv.first, nullptr, 2);
			auto par = has_even_parity(i);
			auto p = computeMeasurementProbability(kv.first);
			if (!par) {
				p = -p;
			}
			aver += p;
		}
		return aver;
	}

	virtual void setExpectationValueZ(const double exp) {
		XACCLogger::instance()->error("AcceleratorBuffer.setExpectationValueZ not "
				"implemented. This method is intended for subclasses.");
	}

	/**
	 * Return a read-only view of this Buffer's measurement results
	 *
	 * @return results Measurement results
	 */
	virtual const std::vector<boost::dynamic_bitset<>> getMeasurements() {
		return measurements;
	}

	/**
	 * Return all measurements as bit strings.
	 *
	 * @return bitStrings List of bit strings.
	 */
	virtual const std::vector<std::string> getMeasurementStrings() {
		std::vector<std::string> strs;
		for (auto m : measurements) {
			std::stringstream ss;
			ss << m;
			strs.push_back(ss.str());
		}
		return strs;
	}

	virtual std::map<std::string, int> getMeasurementCounts() {
		return bitStringToCounts;
	}

	/**
	 * Print information about this AcceleratorBuffer to standard out.
	 *
	 */
	virtual void print() {
	}

	/**
	 * Print information about this AcceleratorBuffer to the
	 * given output stream.
	 *
	 * @param stream Stream to write the buffer to.
	 */
	virtual void print(std::ostream& stream) {
	}

	/**
	 * The Destructor
	 */
	virtual ~AcceleratorBuffer() {
	}

protected:

	/**
	 * Reference to the Accelerator measurement result bit strings
	 */
	std::vector<boost::dynamic_bitset<>> measurements;

	std::map<std::string, int> bitStringToCounts;

	/**
	 * The name of this AcceleratorBuffer
	 */
	std::string bufferId;

	/**
	 * The number of bits in this buffer.
	 */
	int nBits;

};

}

#endif
