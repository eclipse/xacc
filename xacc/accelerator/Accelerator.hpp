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
#ifndef XACC_ACCELERATOR_HPP_
#define XACC_ACCELERATOR_HPP_

#include <memory>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include "AcceleratorBuffer.hpp"
#include "IRTransformation.hpp"
#include "Function.hpp"
#include "OptionsProvider.hpp"
#include "Graph.hpp"
#include "Identifiable.hpp"
#include "Utils.hpp"

namespace xacc {

using AcceleratorGraph = Graph<XACCVertex<>>;

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
 * Derived Accelerators must provide implementations of createBuffer
 * that provide a valid AcceleratorBuffer instance modeling the
 * hardware memory or bits being computed on. Upon creating an
 * AcceleratorBuffer, derived Accelerator implementations must
 * call the protected storeBuffer method to store the AcceleratorBuffer
 * for future reference by Compilers and clients of Accelerator.
 *
 * @author Alex McCaskey
 */
class __attribute__((visibility("default"))) Accelerator: public OptionsProvider,
		public Identifiable {

public:

	/**
	 * Initialize this Accelerator. This method is called
	 * by the XACC framework after an Accelerator has been
	 * requested and created. Perform any work you need
	 * done before execution here.
	 *
	 */
	virtual void initialize() = 0;

	/**
	 * Return the type of this Accelerator.
	 *
	 * @return type The Accelerator type - Gate or AQC QPU, or NPU
	 */
	virtual AcceleratorType getType() = 0;

	/**
	 * Return any IR Transformations that must be applied to ensure
	 * the compiled IR is amenable to execution on this Accelerator.
	 *
	 * @return transformations The IR transformations this Accelerator exposes
	 */
	virtual std::vector<std::shared_ptr<IRTransformation>> getIRTransformations() = 0;

	/**
	 * Execute the provided XACC IR Function on the provided AcceleratorBuffer.
	 *
	 * @param buffer The buffer of bits this Accelerator should operate on.
	 * @param function The kernel to execute.
	 */
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
				const std::shared_ptr<Function> function) = 0;

	/**
	 * Execute a set of kernels with one remote call. Return
	 * a list of AcceleratorBuffers that provide a new view
	 * of the given one AcceleratorBuffer. The ith AcceleratorBuffer
	 * contains the results of the ith kernel execution.
	 *
	 * @param buffer The AcceleratorBuffer to execute on
	 * @param functions The list of IR Functions to execute
	 * @return tempBuffers The list of new AcceleratorBuffers
	 */
	virtual std::vector<std::shared_ptr<AcceleratorBuffer>> execute(
			std::shared_ptr<AcceleratorBuffer> buffer,
			const std::vector<std::shared_ptr<Function>> functions) = 0;

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string. This method returns all available
	 * qubits for this Accelerator. The string id serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId The variable name of the created buffer
	 * @return buffer The buffer instance created.
	 */
	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(
				const std::string& varId) = 0;

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string and of the given number of bits.
	 * The string id serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId The variable name of the created buffer
	 * @param size The number of bits in the created buffer
	 * @return buffer The buffer instance created.
	 */
	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(
			const std::string& varId, const int size) = 0;

	/**
	 * Return the stored AcceleratorBuffer with the provided
	 * string id.
	 *
	 * @param varid The variable name of the created buffer
	 * @return buffer The buffer with given varid.
	 */
	virtual std::shared_ptr<AcceleratorBuffer> getBuffer(
			const std::string& varid) {
		if (isValidBuffer(varid)) {
			return allocatedBuffers[varid];
		} else {
			auto tmpBuffer = createBuffer(varid, 100);
			XACCLogger::instance()->info("Could not find AcceleratorBuffer with id " + varid + ", creating one with 100 bits.");
			storeBuffer(varid, tmpBuffer);
			return tmpBuffer;
		}
	}

	/**
	 * Return all allocated AcceleratorBuffer variable names.
	 *
	 * @return varNames The buffer variable names
	 */
	virtual std::vector<std::string> getAllocatedBufferNames() {
		std::vector<std::string> names;
		for (auto b : allocatedBuffers) {
			names.push_back(b.first);
		}

		return names;
	}

	/**
	 * Return the graph structure for this Accelerator.
	 *
	 * @return connectivityGraph The graph structure of this Accelerator
	 */
	virtual std::shared_ptr<AcceleratorGraph> getAcceleratorConnectivity() {
		return std::make_shared<AcceleratorGraph>();
	}

	/**
	 * Return an empty options_description, this is for
	 * subclasses to implement.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		return std::make_shared<options_description>();
	}

	/**
	 * Given user-input command line options, perform
	 * some operation. Returns true if runtime should exit,
	 * false otherwise.
	 *
	 * @param map The mapping of options to values
	 * @return exit True if exit, false otherwise
	 */
	virtual bool handleOptions(variables_map& map) {
		return false;
	}

	virtual bool isRemote() {
		return false;
	}

	/**
	 * Destructor
	 */
	virtual ~Accelerator() {
	}

protected:

	/**
	 * Return true if this Accelerator can allocated
	 * NBits number of bits. This is meant to be implemented
	 * and used by subclasses.
	 *
	 * @param NBits The number of bits to allocate
	 * @return valid True if size is valid.
	 */
	virtual bool isValidBufferSize(const int NBits) = 0;

	/**
	 * This protected method is to be used by derived
	 * Accelerators to store any created AcceleratorBuffer.
	 *
	 * @param id The variable name of the buffer to store
	 * @param b The buffer to store
	 */
	void storeBuffer(const std::string& id,
			std::shared_ptr<AcceleratorBuffer> b) {
		allocatedBuffers.insert(std::make_pair(id, b));
	}

private:

	/**
	 * The mapping of string ids to created AcceleratorBuffers.
	 */
	std::map<std::string, std::shared_ptr<AcceleratorBuffer>> allocatedBuffers;

	/**
	 * Private utility method to indicate whether there exists
	 * an AcceleratorBuffer with the provided string id.
	 * @param str
	 * @return
	 */
	bool isValidBuffer(const std::string& str) {
		return allocatedBuffers.find(str) != allocatedBuffers.end();
	}

};

}
#endif
