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
#define BOOST_TEST_MODULE SwapTester

#include <boost/test/included/unit_test.hpp>
#include "Swap.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Swap sw(0, 1);
	BOOST_VERIFY(sw.toString("qreg") == "Swap qreg0,qreg1");
	BOOST_VERIFY(sw.bits().size() == 2);
	BOOST_VERIFY(sw.bits()[0] == 0);
	BOOST_VERIFY(sw.bits()[1] == 1);
	BOOST_VERIFY(sw.name() == "Swap");

	Swap sw2(44, 45);

	BOOST_VERIFY(sw2.toString("qreg") == "Swap qreg44,qreg45");
	BOOST_VERIFY(sw2.bits().size() == 2);
	BOOST_VERIFY(sw2.bits()[0] == 44);
	BOOST_VERIFY(sw2.bits()[1] == 45);
	BOOST_VERIFY(sw2.name() == "Swap");

}

