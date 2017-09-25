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
#include "CNOT.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	CNOT cnot(0, 1);
	BOOST_VERIFY(cnot.toString("qreg") == "CNOT qreg0,qreg1");
	BOOST_VERIFY(cnot.bits().size() == 2);
	BOOST_VERIFY(cnot.bits()[0] == 0);
	BOOST_VERIFY(cnot.bits()[1] == 1);
	BOOST_VERIFY(cnot.getName() == "CNOT");

	CNOT cnot2(44, 46);
	BOOST_VERIFY(cnot2.toString("qreg") == "CNOT qreg44,qreg46");
	BOOST_VERIFY(cnot2.bits().size() == 2);
	BOOST_VERIFY(cnot2.bits()[0] == 44);
	BOOST_VERIFY(cnot2.bits()[1] == 46);
	BOOST_VERIFY(cnot2.getName() == "CNOT");

}
BOOST_AUTO_TEST_CASE(checkAutoRegistration) {
	auto cnot = GateInstructionRegistry::instance()->create("CNOT",
			std::vector<int> { 0, 1 });
	BOOST_VERIFY(cnot->getName() == "CNOT");
	BOOST_VERIFY(cnot->bits().size() == 2);
	BOOST_VERIFY(cnot->bits()[0] == 0);
	BOOST_VERIFY(cnot->bits()[1] == 1);
}
