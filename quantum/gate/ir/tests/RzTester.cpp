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
#define BOOST_TEST_MODULE RzTester

#include <boost/test/included/unit_test.hpp>
#include "Rz.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Rz rz(0, 3.14);
	BOOST_VERIFY(boost::get<double>(rz.getParameter(0)) == 3.14);
	BOOST_VERIFY(rz.toString("qreg") == "Rz(3.14) qreg0");
	BOOST_VERIFY(rz.bits().size() == 1);
	BOOST_VERIFY(rz.bits()[0] == 0);
	BOOST_VERIFY(rz.getName() == "Rz");

	Rz rz2(44, 1.71234);

	BOOST_VERIFY(boost::get<double>(rz2.getParameter(0)) == 1.71234);
	BOOST_VERIFY(rz2.toString("qreg") == "Rz(1.71234) qreg44");
	BOOST_VERIFY(rz2.bits().size() == 1);
	BOOST_VERIFY(rz2.bits()[0] == 44);
	BOOST_VERIFY(rz2.getName() == "Rz");


}

BOOST_AUTO_TEST_CASE(checkAutoRegistration) {

	xacc::InstructionParameter p = 3.1415;
	auto rz = GateInstructionRegistry::instance()->create("Rz", std::vector<int>{0});
	rz->setParameter(0, p);
	BOOST_VERIFY(rz->getName() == "Rz");
	BOOST_VERIFY(boost::get<double>(rz->getParameter(0)) == 3.1415);
}
