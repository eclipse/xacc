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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE RigettiAcceleratorTester

#include <memory>
#include <boost/test/included/unit_test.hpp>
#include "RigettiAccelerator.hpp"

using namespace xacc::quantum;

class FakeHttpClient: public fire::util::INetworkingTool {

public:

	bool postOccured = false;

	virtual fire::util::HttpResponse get(const std::string& relativePath,
			const std::map<std::string, std::string>& header = std::map<
					std::string, std::string>()) {
		std::stringstream ss;
		ss << "HELLO\n";
		fire::util::HttpResponse r(ss);
		r.successful = true;

		return r;
	}

	/**
	 * Issue an HTTP Post command at the given relative path with
	 * the provided message. Clients can provide a map of header key values to modify the
	 * POST request.
	 *
	 * @param relativePath The path relative to the hostname/port provided to this NetworkingTool
	 * @param message The message to post
	 * @param header The map of additional HTTP POST header information
	 * @return success Boolean indicating if post was successful
	 *
	 */
	virtual fire::util::HttpResponse post(const std::string& relativePath,
			const std::string& message,
			const std::map<std::string, std::string>& header = std::map<
					std::string, std::string>()) {
		postOccured = true;
		std::stringstream ss;
		ss << "HELLO\n";
		fire::util::HttpResponse r(ss);
		r.successful = true;

		return r;
	}

};
BOOST_AUTO_TEST_CASE(checkKernelExecution) {

	auto options = RuntimeOptions::instance();
	options->insert(std::make_pair("rigetti-api-key", "fakekey"));
	options->insert(std::make_pair("rigetti-type", "faketype"));

	auto client = std::make_shared<FakeHttpClient>();
	RigettiAccelerator acc(client);

	auto qreg1 = acc.createBuffer("qreg", 3);
	auto f = std::make_shared<GateFunction>("foo");

	auto x = std::make_shared<X>(0);
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
	auto m0 = std::make_shared<Measure>(0, 0);
	auto m1 = std::make_shared<Measure>(1,1);

	auto cond1 = std::make_shared<ConditionalFunction>(0);
	auto z = std::make_shared<Z>(2);
	cond1->addInstruction(z);
	auto cond2 = std::make_shared<ConditionalFunction>(1);
	auto x2 = std::make_shared<X>(2);
	cond2->addInstruction(x2);

	f->addInstruction(x);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);
	f->addInstruction(m0);
	f->addInstruction(m1);
	f->addInstruction(cond1);
	f->addInstruction(cond2);

	acc.execute(qreg1, f);

	BOOST_VERIFY(client->postOccured);
}

