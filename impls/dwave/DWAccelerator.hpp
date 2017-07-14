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
#include "AsioNetworkingTool.hpp"
#include "RuntimeOptions.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "DWKernel.hpp"
#include "DWQMI.hpp"


#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace xacc;

namespace xacc {
namespace quantum {

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
 *
 */
class DWAccelerator : virtual public Accelerator {
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
	virtual std::vector<xacc::IRTransformation> getIRTransformations() {
		std::vector<xacc::IRTransformation> v;
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
				value<std::string>(), "The name of the solver to run on.");
		return desc;
	}

	/**
	 * Register this Accelerator with the framework.
	 */
	static void registerAccelerator() {
		DWAccelerator acc;
		xacc::RegisterAccelerator<xacc::quantum::DWAccelerator> DWTEMP(
				"dwave", acc.getOptions());
	}

	DWAccelerator() {
	}

	virtual void initialize(); /* {
		auto options = RuntimeOptions::instance();
		searchAPIKey(apiKey, url);
		auto tempURL = url;
		boost::replace_all(tempURL, "https://", "");
		boost::replace_all(tempURL, "/sapi", "");

		// Set up the extra HTTP headers we are going to need
		headers.insert(std::make_pair("X-Auth-Token", apiKey));
		headers.insert(std::make_pair("Content-type", "application/x-www-form-urlencoded"));
		headers.insert(std::make_pair("Accept", "**"));

		// Get the Remote URL Solver data...
		auto getSolverClient = fire::util::AsioNetworkingTool<SimpleWeb::HTTPS>(tempURL, false);
		auto r = getSolverClient.get("/sapi/solvers/remote", headers);

		std::stringstream ss;
		ss << r.content.rdbuf();
		auto message = ss.str();

		std::cout << "MESSAGE:\n" << message << "\n";
		Document document;
		document.Parse(message.c_str());

		if (document.IsArray()) {
			for (auto i = 0; i < document.Size(); i++) {
				DWSolver solver;
				solver.name = document[i]["id"].GetString();
				boost::trim(solver.name);
				solver.description = document[i]["description"].GetString();
				if (document[i]["properties"].FindMember("j_range") != document[i]["properties"].MemberEnd()) {
					solver.jRangeMin = document[i]["properties"]["j_range"][0].GetDouble();
					solver.jRangeMax = document[i]["properties"]["j_range"][1].GetDouble();
					solver.hRangeMin = document[i]["properties"]["h_range"][0].GetDouble();
					solver.hRangeMax = document[i]["properties"]["h_range"][1].GetDouble();

				}
				solver.nQubits = document[i]["properties"]["num_qubits"].GetInt();

				// Get the connectivity
				auto couplers = document[i]["properties"]["parameters"]["couplers"].GetArray();
				std::cout << "HELLO WORLD: " << couplers.Size() << "\n";
				std::cout << "INSERTING: " << solver.name << "\n";
				availableSolvers.insert(std::make_pair(solver.name, solver));
			}
		}
	}*/

	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(
				const std::string& varId) {
		auto options = RuntimeOptions::instance();
		std::string solverName = "DW_2000Q_VFYC";
		if (options->exists("dwave-solver")) {
			solverName = (*options)["dwave-solver"];
		}
		if (!availableSolvers.count(solverName)) {
			XACCError(solverName + " is not available for creating a buffer.");
		}
		auto solver = availableSolvers[solverName];
		auto buffer = std::make_shared<AcceleratorBuffer>(varId, solver.nQubits);
		storeBuffer(varId, buffer);
		return buffer;
	}

	/**
	 * The destructor
	 */
	virtual ~DWAccelerator() {}

protected:

	std::string apiKey;
	std::string url;
	std::map<std::string, std::string> headers;
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

// Create an alias to search for.
RegisterAccelerator(xacc::quantum::DWAccelerator)

}
}




#endif
