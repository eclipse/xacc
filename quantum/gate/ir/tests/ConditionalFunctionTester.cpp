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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ConditionalFunctionTester

#include <boost/test/included/unit_test.hpp>
#include "ConditionalFunction.hpp"
#include "Hadamard.hpp"
#include "CNOT.hpp"
#include "InstructionIterator.hpp"

using namespace xacc::quantum;


BOOST_AUTO_TEST_CASE(checkFunctionMethods) {

	ConditionalFunction f(0);
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);

	f.addInstruction(h);
	f.addInstruction(cn1);
	f.addInstruction(cn2);
	f.addInstruction(h2);
	BOOST_VERIFY(f.nInstructions() == 4);

	for (auto i : f.getInstructions()) {
		BOOST_VERIFY(!i->isEnabled());
	}

	f.evaluate(0);
	for (auto i : f.getInstructions()) {
		BOOST_VERIFY(!i->isEnabled());
	}

	f.evaluate(1);
	for (auto i : f.getInstructions()) {
		BOOST_VERIFY(i->isEnabled());
	}

}

