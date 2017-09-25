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
#define BOOST_TEST_MODULE DWQMITester

#include <boost/test/included/unit_test.hpp>
#include "DWQMI.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkDWQMIConstruction) {

	DWQMI qmi(0, 1, 2.2);


	BOOST_VERIFY(qmi.bits()[0] == 0);
	BOOST_VERIFY(qmi.bits()[1] == 1);
	BOOST_VERIFY(boost::get<double>(qmi.getParameter(0)) == 2.2);
	BOOST_VERIFY(qmi.toString("") == "0 1 2.2");

	DWQMI qmi2(0);

	BOOST_VERIFY(qmi2.bits()[0] == 0);
	BOOST_VERIFY(qmi2.bits()[1] == 0);
	BOOST_VERIFY(boost::get<double>(qmi2.getParameter(0)) == 0.0);
	BOOST_VERIFY(qmi2.toString("") == "0 0 0");

	DWQMI qmi3(22, 3.3);

	BOOST_VERIFY(qmi3.bits()[0] == 22);
	BOOST_VERIFY(qmi3.bits()[1] == 22);
	BOOST_VERIFY(boost::get<double>(qmi3.getParameter(0)) == 3.3);
	BOOST_VERIFY(qmi3.toString("") == "22 22 3.3");

}

