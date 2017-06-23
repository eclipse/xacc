
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
#define BOOST_TEST_MODULE SimpleAcceleratorTester

#include <memory>
#include <boost/test/included/unit_test.hpp>
#include "SimpleAccelerator.hpp"
#include "JsonVisitor.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkKernelExecution) {

	SimpleAccelerator acc;
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

}

template<typename ... RuntimeArgs>
std::function<void(RuntimeArgs...)> setParams(
		std::shared_ptr<Function> kernel) {

	// Create a lambda that executes the kernel on the Accelerator.
	return [=](RuntimeArgs... args) {

		if (sizeof...(RuntimeArgs) > 0) {
			// Store the runtime parameters in a tuple
			auto argsTuple = std::make_tuple(args...);

			// Loop through the tuple, and add InstructionParameters
			// to the parameters vector.
			std::vector<InstructionParameter> parameters;
			xacc::tuple_for_each(argsTuple, [&](auto value) {
				parameters.push_back(InstructionParameter(value));
			});

			// Evaluate all Variable Parameters
			kernel->evaluateVariableParameters(parameters);
		}

		return;
	};
}

BOOST_AUTO_TEST_CASE(checkExecuteKernelWithParameters) {

	SimpleAccelerator acc;
	auto qreg1 = acc.createBuffer("qreg", 3);

	InstructionParameter fParam("phi");
	InstructionParameter rzParam("phi");
	auto kernel = std::make_shared<GateFunction>("foo",
			std::vector<InstructionParameter>{fParam});

	auto rz = std::make_shared<Rz>(std::vector<int> { 2 });
	rz->setParameter(0, rzParam);

	kernel->addInstruction(rz);

	JsonVisitor visitor(kernel);
	std::cout << visitor.write() << "\n";
	BOOST_VERIFY(boost::get<std::string>(kernel->getInstruction(0)->getParameter(0)) == "phi");

	auto k = setParams<double>(kernel);
	k(3.1415);

	JsonVisitor visitor2(kernel);

	std::cout << "set new param:\n" << visitor2.write() << "\n";
	BOOST_VERIFY(boost::get<double>(kernel->getInstruction(0)->getParameter(0)) == 3.1415);

	for (double i = 0.0; i < 4; i+=1.0) {
		k(i);
		JsonVisitor visitor3(kernel);

		std::cout << "set new param " << i << ":\n" << visitor3.write() << "\n";
		BOOST_VERIFY(boost::get<double>(kernel->getInstruction(0)->getParameter(0)) == i);
	}



}
