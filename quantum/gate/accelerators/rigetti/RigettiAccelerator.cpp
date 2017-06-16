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
#include <fstream>
#include <iomanip>

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
				+ (type == "wavefunction" ? "\"" : "\", \"trials\" : " + trials)
				+ " }";

	}

	// Set up the extra HTTP headers we are going to need
	headers.insert(std::make_pair("Content-type", "application/json"));
	headers.insert(std::make_pair("Accept", "application/octet-stream"));
	headers.insert(std::make_pair("x-api-key", apiKey));

	XACCInfo("Rigetti Json Payload = " + jsonStr);

	// Execute the HTTP Post!
	auto postResponse = httpClient->post("/qvm", jsonStr, headers);

	// Check that it succeeded
	if (!postResponse.successful) {
		XACCError("Error in HTTPS Post.\n" + postResponse.status_code);
	}

	XACCInfo("Successful HTTP Post to Rigetti.");

	// Process results... to come

	if (type == "wavefunction") {
		std::istream& str = postResponse.content;

		char buf[postResponse.contentLength];
		str.read(buf, postResponse.contentLength);

//		unsigned char buf[] = {
//		  0x05, // classical measurement octets - 0x05 is 5 which is 1 0 1 // make sense for teleport
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Re component 0
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Im component 0
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Re 0x3f = 63 = 1 1 1 1 1 1, 0xf0 = 15 = 1 1 1 1
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		  0x0a // new line
//		};

		// What to do with this???
		auto roundToNextMultiple = [](int n, int m) -> int {
			return ((n + m - 1) / m) * m;
		};

		auto getBits = [](auto o) -> std::bitset<8> {
    	    if  (!(0 <= o <= 255)) XACCError("invalid byte");
    	    std::bitset<8> bits;
    	    for (int i = 0; i < 8; i++) {
    	        if (1 == o & 1) {
    	            bits[i] = 1;
    	        }
    	        o = o >> 1;
    	    }
    	    return bits;
		};

		int numOctets = postResponse.contentLength;
		int numAddresses = visitor->getNumberOfAddresses();
		int numMemoryOctets = roundToNextMultiple(numAddresses, 8) / 8;
		int numWFOctets = numOctets - numMemoryOctets;

		assert(numWFOctets / 16 == std::pow(2, buffer->size()));

		std::cout << "NUM MEM OCT: " << numMemoryOctets << ", " << numAddresses << ", " << numWFOctets << "\n";
		std::vector<int> temp;
		for (int i = 0; i < numMemoryOctets; i++) {
		    std::stringstream ss;
		    ss << buf[i];
		    unsigned long long n;
		    ss >> n;
		    std::bitset<8> bits(n);
//			auto bits = getBits(buf[i]);
			for (int j = 0; j < 8; j++) {
				temp.push_back(bits[j]);
			}
		}

		std::vector<int> mem(temp.begin(), temp.begin() + numAddresses);

		std::cout << "Classical Measurements: ";
		for (auto i : mem) {
			std::cout << i << " ";
		}
		std::cout << "\n";

		union Converter { uint64_t i; double d; };

		auto getDouble = [&] (std::bitset<64> const& bs) -> double{
		    Converter c;
		    c.i = bs.to_ullong();
		    return c.d;
		};

		int p = numMemoryOctets;
		while (p < numOctets) {

			// Get subset of the binary buffer
			char re_be[8], im_be[8];
			std::memcpy(re_be, &buf[p], sizeof(double));
			std::memcpy(im_be, &buf[p+8], sizeof(double));

			double re = *reinterpret_cast<double*>(re_be);
			double im = *reinterpret_cast<double*>(im_be);

			std::cout << p << ": " << "(" << re << ", " << im << ")\n";

			p+=16;

		}

		/*
		 * def recover_complexes(coef_string):
            num_octets = len(coef_string)
            num_addresses = len(classical_addresses)
            num_memory_octets = _round_to_next_multiple(num_addresses, 8) // 8
            num_wavefunction_octets = num_octets - num_memory_octets

            # Parse the classical memory
            mem = []
            for i in range(num_memory_octets):
                # Python 3 oddity: indexing coef_string with a single
                # index returns an int. If you request a slice it keeps
                # it as a bytestring.
                octet = struct.unpack('B', coef_string[i:i+1])[0]
                mem.extend(_octet_bits(octet))

            mem = mem[0:num_addresses]

            # Parse the wavefunction
            wf = np.zeros(num_wavefunction_octets // OCTETS_PER_COMPLEX_DOUBLE, dtype=np.cfloat)
            for i, p in enumerate(range(num_memory_octets, num_octets, OCTETS_PER_COMPLEX_DOUBLE)):
                re_be = coef_string[p: p + OCTETS_PER_DOUBLE_FLOAT]
                im_be = coef_string[p + OCTETS_PER_DOUBLE_FLOAT: p + OCTETS_PER_COMPLEX_DOUBLE]
                re = struct.unpack('>d', re_be)[0]
                im = struct.unpack('>d', im_be)[0]
                wf[i] = complex(re, im)
		 */

	} else if (type == "multishot") {

		std::stringstream ss;
		ss << postResponse.content.rdbuf();

		XACCInfo("Rigetti QVM Response:\n\t" + ss.str());

	} else {
		// do nothing
	}
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

