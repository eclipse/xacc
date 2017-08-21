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
#include <boost/filesystem.hpp>
#include <fstream>
#include <memory>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include "DWAccelerator.hpp"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

namespace xacc {
namespace quantum {

std::shared_ptr<AcceleratorBuffer> DWAccelerator::createBuffer(
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
	auto buffer = std::make_shared<AQCAcceleratorBuffer>(varId, solver.nQubits);
	storeBuffer(varId, buffer);
	return buffer;
}

std::shared_ptr<AcceleratorBuffer> DWAccelerator::createBuffer(
		const std::string& varId, const int size) {
	if (!isValidBufferSize(size)) {
		XACCError("Invalid buffer size.");
	}

	auto buffer = std::make_shared<AQCAcceleratorBuffer>(varId, size);
	storeBuffer(varId, buffer);
	return buffer;
}

bool DWAccelerator::isValidBufferSize(const int NBits) {
	return NBits > 0;
}

void DWAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
		const std::shared_ptr<xacc::Function> kernel) {

	auto dwKernel = std::dynamic_pointer_cast<DWKernel>(kernel);
	if (!dwKernel) {
		XACCError("Invalid kernel.");
	}

	auto aqcBuffer = std::dynamic_pointer_cast<AQCAcceleratorBuffer>(buffer);
	if (!aqcBuffer) {
		XACCError("Invalid Accelerator Buffer.");
	}

	Document doc;
	bool jobCompleted = false;
	std::vector<std::string> splitLines;
	boost::split(splitLines, dwKernel->toString(""), boost::is_any_of("\n"));
	auto nQMILines = splitLines.size();
	auto options = RuntimeOptions::instance();
	std::string jsonStr = "",
			solverName = "DW_2000Q_VFYC", solveType = "ising", trials = "100",
			annealTime = "20";

	if (options->exists("dwave-solver")) {
		solverName = (*options)["dwave-solver"];
	}

	if (!availableSolvers.count(solverName)) {
		XACCError(solverName + " is not available.");
	}

	auto solver = availableSolvers[solverName];

	// Normalize the QMI data
	auto allWeightValues = dwKernel->getAllCouplers();
	auto minWeight = *std::min_element(allWeightValues.begin(),allWeightValues.end());
	auto maxWeight = *std::max_element(allWeightValues.begin(),allWeightValues.end());

	// Check if we should normalize Bias values
	if (minWeight < solver.jRangeMin || maxWeight > solver.jRangeMax) {
		for (auto inst : dwKernel->getInstructions()) {
			auto divisor =
					(std::fabs(minWeight) > std::fabs(maxWeight)) ?
							std::fabs(minWeight) : std::fabs(maxWeight);
			auto weight = boost::get<double>(inst->getParameter(0));
			auto newWeight = weight / divisor;
			InstructionParameter p(newWeight);
			inst->setParameter(0, p);
		}
	}

	if(options->exists("dwave-num-reads")) {
		trials = (*options)["dwave-num-reads"];
	}

	if(options->exists("dwave-anneal-time")) {
		annealTime = (*options)["dwave-anneal-time"];
	}

	jsonStr += "[{ \"solver\" : \"" + solverName + "\", \"type\" : \""
			+ solveType + "\", \"data\" : \"" + std::to_string(solver.nQubits)
			+ " " + std::to_string(nQMILines-1) + "\\n"
			+ dwKernel->toString("") + "\", \"params\": { \"num_reads\" : "
			+ trials + ", \"annealing_time\" : " + annealTime + "} }]";
	boost::replace_all(jsonStr, "\n", "\\n");

	// Create the URI, HTTP Client and Post and Get request
	// add our headers to it - this contains the API key
	http::uri uri = http::uri(url);
	http_client postClient(
			http::uri_builder(uri).append_path(U("/sapi/problems")).to_uri());
	http_request postRequest(methods::POST), getRequest(methods::GET);
	for (auto& kv : headers) {
		postRequest.headers().add(kv.first, kv.second);
		getRequest.headers().add(kv.first, kv.second);
	}
	postRequest.set_body(jsonStr);

	// Post the problem, get the response as json
	auto postResponse = postClient.request(postRequest);
	auto respJson = postResponse.get().extract_json().get();

	// Map that response to a string
	std::stringstream x;
	x << respJson;

	// Parse the json string
	doc.Parse(x.str());

	// Get the JobID
	std::string jobId = std::string(doc[0]["id"].GetString());

	// Create a client to execute HTTP Get requests
	http_client getClient(
			http::uri_builder(uri).append_path(U("/sapi/problems/" + jobId)).to_uri());

	// Loop until the job is complete,
	// get the JSON response
	std::string msg;
	while (!jobCompleted) {

		// Execute HTTP Get
		auto getResponse = getClient.request(getRequest);

		// get the result as a string
		std::stringstream z;
		z << getResponse.get().extract_json().get();
		msg = z.str();

		// Search the result for the status : COMPLETED indicator
		if (boost::contains(msg, "COMPLETED")) {
			jobCompleted = true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// We've completed, so let's get
	// teh results.
	doc.Parse(msg);
	if (doc["status"] == "COMPLETED") {
		std::vector<double> energies;
		std::vector<int> numOccurrences, active_vars;
		auto energyArray = doc["answer"]["energies"].GetArray();
		auto numOccArray = doc["answer"]["num_occurrences"].GetArray();
		for (int i = 0; i < energyArray.Size(); i++) {
			energies.push_back(energyArray[i].GetDouble());
			numOccurrences.push_back(numOccArray[i].GetInt());
		}

		auto solutionsStrEncoded = std::string(doc["answer"]["solutions"].GetString());
		auto decoded = base64_decode(solutionsStrEncoded);
		std::string bitStr = "";
		std::stringstream ss;
		for (std::size_t i = 0; i < decoded.size(); ++i) {
			ss << std::bitset<8>(decoded.c_str()[i]);
		}

		bitStr = ss.str();

		auto activeVarsSize = doc["answer"]["active_variables"].GetArray().Size();
		auto activeVars = doc["answer"]["active_variables"].GetArray();
		for (int i = 0; i < activeVarsSize; i++) {
			active_vars.push_back(activeVars[i].GetInt());
		}

		auto nBitsPerMeasurementPadded = ((activeVarsSize + 8 - 1) / 8) * 8;
		auto nPadBits = nBitsPerMeasurementPadded - activeVarsSize;
		int counter = 0;
		for (int i = 0; i < bitStr.size(); i += nBitsPerMeasurementPadded) {
			auto subBuffer = bitStr.substr(i, nBitsPerMeasurementPadded);
			boost::dynamic_bitset<> bset(subBuffer.substr(0, activeVarsSize));
			counter++;
			aqcBuffer->appendMeasurement(bset);
		}

		aqcBuffer->setEnergies(energies);
		aqcBuffer->setNumberOfOccurrences(numOccurrences);
		aqcBuffer->setActiveVariableIndices(active_vars);

		std::cout << "NExecs: " << aqcBuffer->getNumberOfExecutions() << "\n";
		std::cout << "Min Meas: " << aqcBuffer->getLowestEnergy() << ", " << aqcBuffer->getLowestEnergyMeasurement() << "\n";
		std::cout << "Max Prob Meas: " << aqcBuffer->getMostProbableEnergy() << ", " << aqcBuffer->getMostProbableMeasurement() << "\n";

	} else {
		XACCError("Error in executing D-Wave QPU.");
	}

	return;
}

void DWAccelerator::initialize() {
	auto options = RuntimeOptions::instance();
	searchAPIKey(apiKey, url);

	// Set up the extra HTTP headers we are going to need
	headers.insert(std::make_pair("X-Auth-Token", apiKey));
	headers.insert(std::make_pair("Content-type", "application/x-www-form-urlencoded"));
	headers.insert(std::make_pair("Accept", "*/*"));


	http::uri uri = http::uri(url);
	http_request getRequest(methods::GET);
	for (auto& kv : headers) {
		getRequest.headers().add(kv.first, kv.second);
	}

	// Create a client to execute HTTP Get requests
	http_client getClient(
			http::uri_builder(uri).append_path(U("/sapi/solvers/remote")).to_uri());

	// Execute HTTP Get
	auto getResponse = getClient.request(getRequest);

	// get the result as a string
	std::stringstream ss;
	ss << getResponse.get().extract_json().get();

	auto message = ss.str();

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
			auto couplers = document[i]["properties"]["couplers"].GetArray();
			for (int j = 0; j < couplers.Size(); j++) {
				solver.edges.push_back(std::make_pair(couplers[j][0].GetInt(), couplers[j][1].GetInt()));
			}
			availableSolvers.insert(std::make_pair(solver.name, solver));
		}
	}
}

void DWAccelerator::searchAPIKey(std::string& key, std::string& url) {

	// Search for the API Key in $HOME/.dwave_config,
	// $DWAVE_CONFIG, or in the command line argument --dwave-api-key
	auto options = RuntimeOptions::instance();
	boost::filesystem::path dwaveConfig(
			std::string(getenv("HOME")) + "/.dwave_config");

	if (boost::filesystem::exists(dwaveConfig)) {
		findApiKeyInFile(key, url, dwaveConfig);
	} else if (const char * nonStandardPath = getenv("DWAVE_CONFIG")) {
		boost::filesystem::path nonStandardDwaveConfig(
						nonStandardPath);
		findApiKeyInFile(key, url, nonStandardDwaveConfig);
	} else {

		// Ensure that the user has provided an api-key
		if (!options->exists("dwave-api-key")) {
			XACCError("Cannot execute kernel on DW chip without API Key.");
		}

		// Set the API Key
		key = (*options)["dwave-api-key"];

		if (options->exists("dwave-api-url")) {
			url = (*options)["dwave-api-url"];
		}
	}

	// If its still empty, then we have a problem
	if (key.empty()) {
		XACCError("Error. The API Key is empty. Please place it "
				"in your $HOME/.dwave_config file, $DWAVE_CONFIG env var, "
				"or provide --dwave-api-key argument.");
	}
}

void DWAccelerator::findApiKeyInFile(std::string& apiKey, std::string& url,
		boost::filesystem::path &p) {
	std::ifstream stream(p.string());
	std::string contents(
			(std::istreambuf_iterator<char>(stream)),
			std::istreambuf_iterator<char>());

	std::vector<std::string> lines;
	boost::split(lines, contents, boost::is_any_of("\n"));
	for (auto l : lines) {
		if (boost::contains(l, "key")) {
			std::vector<std::string> split;
			boost::split(split, l, boost::is_any_of(":"));
			auto key = split[1];
			boost::trim(key);
			apiKey = key;
		} else if (boost::contains(l, "url")) {
			std::vector<std::string> split;
			boost::split(split, l, boost::is_any_of(":"));
			auto key = split[1] + ":" + split[2];
			boost::trim(key);
			url = key;
		}
	}
}

/**
 * Return the graph structure for this Accelerator.
 *
 * @return connectivityGraph The graph structure of this Accelerator
 */
std::shared_ptr<AcceleratorGraph> DWAccelerator::getAcceleratorConnectivity() {
	auto options = RuntimeOptions::instance();
	std::string solverName = "DW_2000Q_VFYC";

	if (options->exists("dwave-solver")) {
		solverName = (*options)["dwave-solver"];
	}

	if (!availableSolvers.count(solverName)) {
		XACCError(solverName + " is not available.");
	}

	auto solver = availableSolvers[solverName];

	auto graph = std::make_shared<AcceleratorGraph>(solver.nQubits);

	for (auto es : solver.edges) {
		graph->addEdge(es.first, es.second);
	}

	return graph;

}
}
}
