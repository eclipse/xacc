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
#define BOOST_TEST_MODULE YTester

#include <boost/test/included/unit_test.hpp>
#include "Y.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Y y(0);
	BOOST_VERIFY(y.toString("qreg") == "Y qreg0");
	BOOST_VERIFY(y.bits().size() == 1);
	BOOST_VERIFY(y.bits()[0] == 0);
	BOOST_VERIFY(y.name() == "Y");

	Y y2(44);
	BOOST_VERIFY(y2.toString("qreg") == "Y qreg44");
	BOOST_VERIFY(y2.bits().size() == 1);
	BOOST_VERIFY(y2.bits()[0] == 44);
	BOOST_VERIFY(y2.name() == "Y");

}
