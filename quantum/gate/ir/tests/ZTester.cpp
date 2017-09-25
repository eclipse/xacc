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
#define BOOST_TEST_MODULE ZTester

#include <boost/test/included/unit_test.hpp>
#include "Z.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(czeckCreation) {

	Z z(0);
	BOOST_VERIFY(z.toString("qreg") == "Z qreg0");
	BOOST_VERIFY(z.bits().size() == 1);
	BOOST_VERIFY(z.bits()[0] == 0);
	BOOST_VERIFY(z.getName() == "Z");

	Z z2(44);
	BOOST_VERIFY(z2.toString("qreg") == "Z qreg44");
	BOOST_VERIFY(z2.bits().size() == 1);
	BOOST_VERIFY(z2.bits()[0] == 44);
	BOOST_VERIFY(z2.getName() == "Z");


}

BOOST_AUTO_TEST_CASE(checkAutoRegistration) {

	auto z = GateInstructionRegistry::instance()->create("Z", std::vector<int>{0});
	BOOST_VERIFY(z->bits().size() == 1);
	BOOST_VERIFY(z->bits()[0] == 0);
	BOOST_VERIFY(z->getName() == "Z");
}
