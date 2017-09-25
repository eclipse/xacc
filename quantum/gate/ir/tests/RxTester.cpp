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
#define BOOST_TEST_MODULE RxTester

#include <boost/test/included/unit_test.hpp>
#include "Rx.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Rx rx(0, 3.14);
	BOOST_VERIFY(boost::get<double>(rx.getParameter(0)) == 3.14);
	BOOST_VERIFY(rx.toString("qreg") == "Rx(3.14) qreg0");
	BOOST_VERIFY(rx.bits().size() == 1);
	BOOST_VERIFY(rx.bits()[0] == 0);
	BOOST_VERIFY(rx.getName() == "Rx");

	Rx rx2(44, 1.71234);

	BOOST_VERIFY(boost::get<double>(rx2.getParameter(0)) == 1.71234);
	BOOST_VERIFY(rx2.toString("qreg") == "Rx(1.71234) qreg44");
	BOOST_VERIFY(rx2.bits().size() == 1);
	BOOST_VERIFY(rx2.bits()[0] == 44);
	BOOST_VERIFY(rx2.getName() == "Rx");


}

BOOST_AUTO_TEST_CASE(checkAutoRegistration) {

	xacc::InstructionParameter p = 3.1415;
	auto rx = GateInstructionRegistry::instance()->create("Rx", std::vector<int>{0});
	rx->setParameter(0, p);
	BOOST_VERIFY(rx->getName() == "Rx");
	BOOST_VERIFY(boost::get<double>(rx->getParameter(0)) == 3.1415);
}
