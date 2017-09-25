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
#define BOOST_TEST_MODULE HadamardTester

#include <boost/test/included/unit_test.hpp>
#include "Hadamard.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Hadamard h(0);
	BOOST_VERIFY(h.toString("qreg") == "H qreg0");
	BOOST_VERIFY(h.bits().size() == 1);
	BOOST_VERIFY(h.bits()[0] == 0);
	BOOST_VERIFY(h.getName() == "H");

	Hadamard h2(44);
	BOOST_VERIFY(h2.toString("qreg") == "H qreg44");
	BOOST_VERIFY(h2.bits().size() == 1);
	BOOST_VERIFY(h2.bits()[0] == 44);
	BOOST_VERIFY(h2.getName() == "H");

}

BOOST_AUTO_TEST_CASE(checkAutoRegistration) {

	auto hadamard = GateInstructionRegistry::instance()->create("H",
			std::vector<int> { 0 });
	BOOST_VERIFY(hadamard->bits().size() == 1);
	BOOST_VERIFY(hadamard->bits()[0] == 0);
	BOOST_VERIFY(hadamard->getName() == "H");

}
