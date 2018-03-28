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
#define BOOST_TEST_MODULE RyTester

#include <boost/test/included/unit_test.hpp>
#include "Ry.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Ry ry(0, 3.14);
	BOOST_VERIFY(boost::get<double>(ry.getParameter(0)) == 3.14);
	BOOST_VERIFY(ry.toString("qreg") == "Ry(3.14) qreg0");
	BOOST_VERIFY(ry.bits().size() == 1);
	BOOST_VERIFY(ry.bits()[0] == 0);
	BOOST_VERIFY(ry.name() == "Ry");

	Ry ry2(44, 1.71234);

	BOOST_VERIFY(boost::get<double>(ry2.getParameter(0)) == 1.71234);
	BOOST_VERIFY(ry2.toString("qreg") == "Ry(1.71234) qreg44");
	BOOST_VERIFY(ry2.bits().size() == 1);
	BOOST_VERIFY(ry2.bits()[0] == 44);
	BOOST_VERIFY(ry2.name() == "Ry");


}

