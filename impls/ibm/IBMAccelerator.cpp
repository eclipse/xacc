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
#include "SimulatedQubits.hpp"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <codecvt>
#include <memory>
#include "GateQIR.hpp"
#include "XACC.hpp"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include "IBMAccelerator.hpp"
#include "IBMIRTransformation.hpp"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

using namespace rapidjson;

namespace xacc {
namespace quantum {

std::shared_ptr<AcceleratorBuffer> IBMAccelerator::createBuffer(
			const std::string& varId) {
	if (!isValidBufferSize(30)) {
		XACCError("Invalid buffer size.");
	}

	std::shared_ptr<AcceleratorBuffer> buffer;
	if ((*RuntimeOptions::instance())["IBM-type"] == "wavefunction") {
		buffer = std::make_shared<SimulatedQubits>(varId, 30);
	} else {
		buffer = std::make_shared<AcceleratorBuffer>(varId, 30);
	}

	storeBuffer(varId, buffer);
	return buffer;

}

std::shared_ptr<AcceleratorBuffer> IBMAccelerator::createBuffer(
		const std::string& varId, const int size) {
	if (!isValidBufferSize(size)) {
		XACCError("Invalid buffer size.");
	}

	std::shared_ptr<AcceleratorBuffer> buffer;
	if ((*RuntimeOptions::instance())["IBM-type"] == "wavefunction") {
		buffer = std::make_shared<SimulatedQubits>(varId, size);
	} else {
		buffer = std::make_shared<AcceleratorBuffer>(varId, size);
	}

	storeBuffer(varId, buffer);
	return buffer;
}

bool IBMAccelerator::isValidBufferSize(const int NBits) {
	return NBits > 0 && NBits < 31;
}

std::vector<std::shared_ptr<IRTransformation>> IBMAccelerator::getIRTransformations() {

	std::vector<std::shared_ptr<IRTransformation>> transformations;

	std::string backendName = "ibmqx_qasm_simulator";

	if (xacc::optionExists("ibm-backend")) {
		backendName = xacc::getOption("ibm-backend");
	}

	if (!availableBackends.count(backendName)) {
		XACCError(backendName + " is not available.");
	}

	auto backend = availableBackends[backendName];

	if (!backend.couplers.empty()) {
		auto transform = std::make_shared<IBMIRTransformation>(
				backend.couplers);
		transformations.push_back(transform);
	}
	return transformations;
}

void IBMAccelerator::initialize() {
	std::string jsonStr = "", apiKey = "";
	auto options = RuntimeOptions::instance();
	searchAPIKey(apiKey, url);

	std::string tokenParam = "apiToken=" + apiKey;
	http::uri uri = http::uri(url);
	http_client client(http::uri_builder(uri).append_path(U("/api/users/loginWithToken")).to_uri());

	http_request request(methods::POST);
	request.headers().add("Content-Type", "application/x-www-form-urlencoded");
	request.headers().add("Connection", "keep-alive");
	request.headers().add("Content-Length", std::to_string(tokenParam.length()));
	request.set_body(tokenParam);

	auto resp = client.request(request);
	std::stringstream xx;
	xx << resp.get().extract_string().get();

	Document d;
	d.Parse(xx.str());
	currentApiToken = d["id"].GetString();

	http_request requestBackends(methods::GET);
	http_client getBackendsClient(
			http::uri_builder(uri).append_path(
					U("/api/Backends?access_token=" + currentApiToken)).to_uri());
	auto getBackendsResponse = getBackendsClient.request(requestBackends);

	// get the result as a string
	std::stringstream z;
	z << getBackendsResponse.get().extract_json().get();
	std::cout << "Backends:\n" << z.str() << "\n";

	d.Parse(z.str());

	for (auto& b : d.GetArray()) {

		IBMBackend backend;
		backend.nQubits = b["nQubits"].GetInt();
		backend.name = b["name"].GetString();
		backend.description = b["description"].GetString();

		auto isSimulator = b["simulator"].GetBool();
		if (!isSimulator) {
			auto couplers = b["couplingMap"].GetArray();
			for (int j = 0; j < couplers.Size(); j++) {
				backend.couplers.push_back(
						std::make_pair(couplers[j][0].GetInt(),
								couplers[j][1].GetInt()));
			}
		}
		availableBackends.insert(std::make_pair(backend.name, backend));

	}
}


void IBMAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
		const std::shared_ptr<xacc::Function> kernel) {

	// Get the runtime options map, and initialize
	// some basic variables we are going to need
	auto options = RuntimeOptions::instance();
	std::string backend = "ibmqx_qasm_simulator";
	std::string jsonStr = "";
	std::string shots = "1024";
	std::map<std::string, std::string> headers;

	// Create the Instruction Visitor that is going
	// to map our IR to Quil.
	auto visitor = std::make_shared<OpenQasmVisitor>(buffer->size());

	// Our QIR is really a tree structure
	// so create a pre-order tree traversal
	// InstructionIterator to walk it
	InstructionIterator it(kernel);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();
		if (nextInst->isEnabled()) nextInst->accept(visitor);
	}

	Document d;
	auto qasmStr = visitor->getOpenQasmString();
	boost::replace_all(qasmStr, "\n", "\\n");
	jsonStr = "{\"qasms\": [{\"qasm\": \"" + qasmStr + "\"}], \"shots\": "+shots+", \"maxCredits\": 3, \"backend\": {\"name\": \""+backend+"\"}}";

	// Set up the extra HTTP headers we are going to need
	headers.insert(std::make_pair("Content-type", "application/json"));
	headers.insert(std::make_pair("Connection", "keep-alive"));
	headers.insert(std::make_pair("Accept-Encoding", "gzip, deflate"));
	headers.insert(std::make_pair("Accept", "*/*"));

	// Create the URI, HTTP Client and Post and Get request
	// add our headers to it - this contains the API key

	std::cout << "QASM:\n" << qasmStr << "\n";
	std::stringstream relSS;
	relSS << "/api/Jobs?access_token=" << currentApiToken;
	http::uri uri = http::uri(url);
	http_client postClient(
			http::uri_builder(uri).append_path(U(relSS.str())).to_uri());
	http_request postRequest(methods::POST), getRequest(methods::GET);
	for (auto& kv : headers) {
		postRequest.headers().add(kv.first, kv.second);
		getRequest.headers().add(kv.first, kv.second);
	}
	postRequest.set_body(jsonStr);

	// Post the problem, get the response as json
	auto postResponse = postClient.request(postRequest);
	auto respJson = postResponse.get().extract_string().get();

	// Map that response to a string
	std::stringstream ss;
	ss << respJson;

	std::cout << "RESPONSE:\n\n" << ss.str() << "\n\n";
	// Parse the json string
	d.Parse(ss.str());

	if (boost::contains(ss.str(), "error")) {
		auto message = std::string(d["error"]["message"].GetString());
		if (boost::contains(message, "qasm is wrong")) {
			boost::replace_all(qasmStr, "\\n", "\n");
			message += "\n\n" + qasmStr;
		}
		XACCError("IBM Error: " + message);
	}

	// Get the JobID
	std::string jobId = std::string(d["id"].GetString());

	// Create a client to execute HTTP Get requests
	http_client getClient(
			http::uri_builder(uri).append_path(
					U("/api/Jobs/" + jobId + "?access_token=" + currentApiToken)).to_uri());

	// Loop until the job is complete,
	// get the JSON response
	std::string msg;
	bool jobCompleted = false;
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

	std::cout << "LAST MSG:\n" << msg << "\n";

	d.Parse(msg);
	const Value& counts = d["qasms"][0]["result"]["data"]["counts"];
	for (Value::ConstMemberIterator itr = counts.MemberBegin();
			itr != counts.MemberEnd(); ++itr) {
		std::cout << "VAL: " << itr->value.GetInt() << "\n";

		// NOTE THESE BITS ARE LEFT MOST IS MOST SIGNIFICANT,
		// LEFT MOST IS (N-1)th Qubit, RIGHT MOST IS 0th qubit
		std::string bitStr = itr->name.GetString();
		std::vector<std::string> bits;
		boost::split(bits, bitStr, boost::is_any_of(" "));
		boost::dynamic_bitset<> outcome(buffer->size());
		for (int i = 0; i < bits.size(); i++) {
			outcome[i] = std::stoi(bits[i]);
		}

		std::cout << "OUTCOME: " << outcome << "\n";

		int nOccurrences = itr->value.GetInt();
		for (int i = 0; i < nOccurrences; i++) {
			buffer->appendMeasurement(outcome);
		}
	}

}

std::shared_ptr<AcceleratorGraph> IBMAccelerator::getAcceleratorConnectivity() {
	std::string backendName = "ibmqx_qasm_simulator";

	if (xacc::optionExists("ibm-backend")) {
		backendName = xacc::getOption("ibm-backend");
	}

	if (!availableBackends.count(backendName)) {
		XACCError(backendName + " is not available.");
	}

	auto backend = availableBackends[backendName];
	auto graph = std::make_shared<AcceleratorGraph>(backend.nQubits);

	if (!backend.couplers.empty()) {
		for (auto es : backend.couplers) {
			graph->addEdge(es.first, es.second);
		}
	} else {
		for (int i = 0; i < backend.nQubits; i++) {
			for (int j = 0; j < backend.nQubits; j++) {
				if (i < j) {
					graph->addEdge(i, j);
				}
			}
		}
	}

	return graph;
}

void IBMAccelerator::searchAPIKey(std::string& key, std::string& url) {

	// Search for the API Key in $HOME/.dwave_config,
	// $DWAVE_CONFIG, or in the command line argument --dwave-api-key
	auto options = RuntimeOptions::instance();
	boost::filesystem::path dwaveConfig(
			std::string(getenv("HOME")) + "/.ibm_config");

	if (boost::filesystem::exists(dwaveConfig)) {
		findApiKeyInFile(key, url, dwaveConfig);
	} else if (const char * nonStandardPath = getenv("IBM_CONFIG")) {
		boost::filesystem::path nonStandardDwaveConfig(
						nonStandardPath);
		findApiKeyInFile(key, url, nonStandardDwaveConfig);
	} else {

		// Ensure that the user has provided an api-key
		if (!options->exists("ibm-api-key")) {
			XACCError("Cannot execute kernel on IBM chip without API Key.");
		}

		// Set the API Key
		key = (*options)["ibm-api-key"];

		if (options->exists("ibm-api-url")) {
			url = (*options)["ibm-api-url"];
		}
	}

	// If its still empty, then we have a problem
	if (key.empty()) {
		XACCError("Error. The API Key is empty. Please place it "
				"in your $HOME/.ibm_config file, $IBM_CONFIG env var, "
				"or provide --ibm-api-key argument.");
	}
}

void IBMAccelerator::findApiKeyInFile(std::string& apiKey, std::string& url,
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
}
}
