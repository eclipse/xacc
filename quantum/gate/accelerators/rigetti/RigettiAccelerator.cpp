/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
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
#include "RigettiAccelerator.hpp"
#include <boost/filesystem.hpp>

namespace xacc {
namespace quantum {

std::shared_ptr<AcceleratorBuffer> RigettiAccelerator::createBuffer(
		const std::string& varId) {
	auto buffer = std::make_shared<AcceleratorBuffer>(varId);
	storeBuffer(varId, buffer);
	return buffer;
}

std::shared_ptr<AcceleratorBuffer> RigettiAccelerator::createBuffer(
		const std::string& varId, const int size) {
	if (!isValidBufferSize(size)) {
		XACCError("Invalid buffer size.");
	}
	auto buffer = std::make_shared<AcceleratorBuffer>(varId, size);
	storeBuffer(varId, buffer);
	return buffer;
}

bool RigettiAccelerator::isValidBufferSize(const int NBits) {
	return NBits > 0;
}

void RigettiAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
		const std::shared_ptr<xacc::Function> kernel) {

	// Get the runtime options map, and initialize
	// some basic variables we are going to need
	auto options = RuntimeOptions::instance();
	std::string type = "multishot";
	std::string jsonStr = "", apiKey = "";
	std::string trials = "10";
	std::map<std::string, std::string> headers;

	// Create the Instruction Visitor that is going
	// to map our IR to Quil.
	auto visitor = std::make_shared<QuilVisitor>();

	// Our QIR is really a tree structure
	// so create a pre-order tree traversal
	// InstructionIterator to walk it
	InstructionIterator it(kernel);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();
		if (nextInst->isEnabled()) nextInst->accept(visitor);
	}

	// Search for the API key
	searchAPIKey(apiKey);

	// Set the execution type if the user provided it
	if (options->exists("type")) {
		type = (*options)["type"];
	}

	// Set the trials number if user provided it
	if (options->exists("trials")) {
		trials = (*options)["trials"];
	}

	// Construct the JSON payload string
	if (type == "ping") {
		jsonStr += "{ \"type\" : \"ping\" }";
	} else if (type == "version") {
		jsonStr += "{ \"type\" : \"version\" }";
	} else {

		// Get the quil instructions
		auto quilStr = visitor->getQuilString();
		boost::replace_all(quilStr, "\n", "\\n");

		// Create the Json String
		jsonStr += "{ \"type\" : \"" + type + "\", \"addresses\" : "
				+ visitor->getClassicalAddresses()
				+ ", \"quil-instructions\" : \"" + quilStr
				+ (type == "wavefunction" ? "" : "\", \"trials\" : " + trials)
				+ " }";

	}

	// Set up the extra HTTP headers we are going to need
	headers.insert(std::make_pair("Content-type", "application/json"));
	headers.insert(std::make_pair("Accept", "application/octet-stream"));
	headers.insert(std::make_pair("x-api-key", apiKey));

	XACCInfo("Rigetti Json Payload = " + jsonStr);

	// Execute the HTTP Post!
	bool success = httpClient->post("/qvm", jsonStr, headers);

	// Check that it succeeded
	if (!success) {
		XACCError("Error in HTTPS Post.\n" + httpClient->getLastStatusCode()
				+ "\n" + httpClient->getLastRequestMessage());
	}

	XACCInfo("\n\tSuccessful HTTP Post to Rigetti.\n\t"
			+ httpClient->getLastRequestMessage());

	// Process results... to come
}

void RigettiAccelerator::searchAPIKey(std::string& key) {

	// Search for the API Key in $HOME/.pyquil_config,
	// $PYQUIL_CONFIG, or in the command line argument --api-key
	auto options = RuntimeOptions::instance();
	boost::filesystem::path pyquilConfig(
			std::string(getenv("HOME")) + "/.pyquil_config");
	if (boost::filesystem::exists(pyquilConfig)) {
		findApiKeyInFile(key, pyquilConfig);
	} else if (const char * nonStandardPath = getenv("PYQUIL_CONFIG")) {
		boost::filesystem::path nonStandardPyquilConfig(
						nonStandardPath);
		findApiKeyInFile(key, nonStandardPyquilConfig);
	} else {

		// Ensure that the user has provided an api-key
		if (!options->exists("api-key")) {
			XACCError("Cannot execute kernel on Rigetti chip without API Key.");
		}

		// Set the API Key
		key = (*options)["api-key"];
	}

	// If its still empty, then we have a problem
	if (key.empty()) {
		XACCError("Error. The API Key is empty. Please place it "
				"in your $HOME/.pyquil_config file, $PYQUIL_CONFIG env var, "
				"or provide --api-key argument.");
	}
}

void RigettiAccelerator::findApiKeyInFile(std::string& apiKey,
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
		}
	}
}
}
}

