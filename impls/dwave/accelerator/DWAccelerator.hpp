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
#ifndef QUANTUM_GATE_ACCELERATORS_DWACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_DWACCELERATOR_HPP_

#include "Accelerator.hpp"
#include "RuntimeOptions.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "DWKernel.hpp"
#include "DWQMI.hpp"
#include "AQCAcceleratorBuffer.hpp"


#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace xacc;

namespace xacc {
namespace quantum {

/**
 * Wrapper for information related to the remote
 * D-Wave solver.
 */
struct DWSolver {
	std::string name;
	std::string description;
	double jRangeMin;
	double jRangeMax;
	double hRangeMin;
	double hRangeMax;
	int nQubits;
	std::vector<std::pair<int,int>> edges;
};

/**
 * The DWAccelerator is an XACC Accelerator that
 * takes D-Wave IR and executes the quantum machine
 * instructions via remote HTTP invocations.
 */
class DWAccelerator : public Accelerator {
public:

	/**
	 * The constructor
	 */
	DWAccelerator() {}

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

	/**
	 * Return true if this Accelerator can allocated
	 * NBits number of bits.
	 * @param NBits
	 * @return
	 */
	virtual bool isValidBufferSize(const int NBits);

	/**
	 * Return the graph structure for this Accelerator.
	 *
	 * @return connectivityGraph The graph structure of this Accelerator
	 */
	virtual std::shared_ptr<AcceleratorGraph> getAcceleratorConnectivity();

	/**
	 * Execute the kernel on the provided AcceleratorBuffer through a
	 * HTTP Post of Quil instructions to the Rigetti QPU at api.rigetti.com/qvm
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
		return AcceleratorType::qpu_aqc;
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
	 * Return all relevant RigettiAccelerator runtime options.
	 * Users can set the api-key, execution type, and number of triels
	 * from the command line with these options.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		auto desc = std::make_shared<options_description>(
				"D-Wave Accelerator Options");
		desc->add_options()("dwave-api-key", value<std::string>(),
				"Provide the D-Wave API key. This is used if "
						"$HOME/.dwave_config is not found")("dwave-api-url",
				value<std::string>(), "The D-Wave SAPI URL, "
						"https://qubist.dwavesys.com/sapi "
						"used by default.")("dwave-solver",
				value<std::string>(), "The name of the solver to run on.")
				("dwave-num-reads", value<std::string>(), "The number of executions on the chip for the given problem.")
				("dwave-anneal-time", value<std::string>(), "The time to evolve the chip - an integer in microseconds.")
				("dwave-thermalization", value<std::string>(), "The thermalization...")
				("dwave-list-solvers", "List the available solvers at the Qubist URL.");
		return desc;
	}

	virtual bool handleOptions(variables_map& map) {
		if (map.count("dwave-list-solvers")) {
			initialize();

			for (auto s : availableSolvers) {
				XACCInfo("Available D-Wave Solver: " + std::string(s.first));
			}
			return true;
		}
		return false;
	}

	/**
	 * Initialize this DWAccelerator with information
	 * about the remote Qubist solvers
	 */
	virtual void initialize();

	/**
	 * Create and return an AQCAcceleratorBuffer of size
	 * dictated by the current solver being used.
	 *
	 * @param varId The name of this buffer
	 * @return buffer The AcceleratorBuffer
	 */
	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(
				const std::string& varId);

	virtual const std::string name() const {
		return "dwave";
	}

	virtual const std::string description() const {
		return "The D-Wave Accelerator executes Ising Hamiltonian parameters "
				"on a remote D-Wave QPU.";
	}

	/**
	 * The destructor
	 */
	virtual ~DWAccelerator() {}

protected:

	/**
	 * Reference to the D-Wave API Token
	 */
	std::string apiKey;

	/**
	 * Reference to the remote D-Wave Qubist URL
	 */
	std::string url;

	/**
	 * Reference to the HTTP Post/Get headers
	 */
	std::map<std::string, std::string> headers;

	/**
	 * Reference to the mapping of solver names
	 * to Solver Type.
	 */
	std::map<std::string, DWSolver> availableSolvers;

private:

	/**
	 * Private utility to search for the D-Wave
	 * API key in $HOME/.dwave_config, $DWAVE_CONFIG,
	 * or --dwave-api-key command line arg
	 */
	void searchAPIKey(std::string& key, std::string& url);

	/**
	 * Private utility to search for key in the config
	 * file.
	 */
	void findApiKeyInFile(std::string& key, std::string& url, boost::filesystem::path &p);

};

}
}




#endif
