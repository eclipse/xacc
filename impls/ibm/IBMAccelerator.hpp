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
#ifndef QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_HPP_

#include "Accelerator.hpp"
#include "InstructionIterator.hpp"
#include "RuntimeOptions.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "OpenQasmVisitor.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {



/**
 * Wrapper for information related to the remote
 * D-Wave solver.
 */
struct IBMBackend {
	std::string name;
	std::string description;
	int nQubits;
	std::vector<std::pair<int,int>> edges;
};

/**
 * The IBMAccelerator is a QPUGate Accelerator that
 * provides an execute implementation that maps XACC IR
 * to an equivalent Quil string, and executes it on the
 * IBM superconducting quantum chip at api.IBM.com/qvm
 * through Fire's HTTP Client utilities.
 *
 */
class IBMAccelerator : virtual public Accelerator {
public:

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string and of the given number of bits.
	 * The string id serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId
	 * @param size
	 * @return
	 */
	std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId,
			const int size);

	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(
				const std::string& varId);

	virtual void initialize();

	virtual std::shared_ptr<AcceleratorGraph> getAcceleratorConnectivity();

	/**
	 * Return true if this Accelerator can allocated
	 * NBits number of bits.
	 * @param NBits
	 * @return
	 */
	virtual bool isValidBufferSize(const int NBits);

	/**
	 * Execute the kernel on the provided AcceleratorBuffer through a
	 * HTTP Post of Quil instructions to the IBM QPU at api.IBM.com/qvm
	 *
	 * @param ir
	 */
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
			const std::shared_ptr<xacc::Function> kernel);

	/**
	 * This Accelerator models QPU Gate accelerators.
	 * @return
	 */
	virtual AcceleratorType getType() {
		return AcceleratorType::qpu_gate;
	}

	/**
	 * We have no need to transform the IR for this Accelerator,
	 * so return an empty list, for now.
	 * @return
	 */
	virtual std::vector<std::shared_ptr<IRTransformation>> getIRTransformations() {
		std::vector<std::shared_ptr<IRTransformation>> v;
		return v;
	}

	/**
	 * Return all relevant IBMAccelerator runtime options.
	 * Users can set the api-key, execution type, and number of triels
	 * from the command line with these options.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		auto desc = std::make_shared<options_description>(
				"IBM Accelerator Options");
		desc->add_options()("ibm-api-key", value<std::string>(),
				"Provide the IBM API key. This is used if $HOME/.ibm_config is not found")("ibm-backend",
				value<std::string>(),
				"Provide the backend name.")
				("ibm-shots", value<std::string>(), "Provide the number of shots to execute.")
				("ibm-list-backends", "List the available backends at the IBM Quantum Experience URL.");

		return desc;
	}

	virtual bool handleOptions(variables_map& map) {
		if (map.count("ibm-list-backends")) {
			initialize();
			for (auto s : availableBackends) {
				XACCInfo("Available IBM Backend: " + std::string(s.first));
			}
			return true;
		}
		return false;
	}

	IBMAccelerator() {
	}

	virtual const std::string name() const {
		return "ibm";
	}

	virtual const std::string description() const {
		return "The IBM Accelerator interacts...";
	}

	/**
	 * The destructor
	 */
	virtual ~IBMAccelerator() {}

private:

	/**
	 * Private utility to search for the IBM
	 * API key in $HOME/.pyquil_config, $PYQUIL_CONFIG,
	 * or --api-key command line arg
	 */
	void searchAPIKey(std::string& key, std::string& url);

	/**
	 * Private utility to search for key in the config
	 * file.
	 */
	void findApiKeyInFile(std::string& key, std::string& url, boost::filesystem::path &p);

	std::string currentApiToken;

	std::string url;

	std::map<std::string, IBMBackend> availableBackends;

};

}
}




#endif
