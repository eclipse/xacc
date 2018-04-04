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
#define BOOST_TEST_MODULE XTester

#include <boost/test/included/unit_test.hpp>
#include "X.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	X x(0);
	BOOST_VERIFY(x.toString("qreg") == "X qreg0");
	BOOST_VERIFY(x.bits().size() == 1);
	BOOST_VERIFY(x.bits()[0] == 0);
	BOOST_VERIFY(x.name() == "X");

	X x2(44);
	BOOST_VERIFY(x2.toString("qreg") == "X qreg44");
	BOOST_VERIFY(x2.bits().size() == 1);
	BOOST_VERIFY(x2.bits()[0] == 44);
	BOOST_VERIFY(x2.name() == "X");

}
