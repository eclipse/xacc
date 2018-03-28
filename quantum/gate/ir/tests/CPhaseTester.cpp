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
#define BOOST_TEST_MODULE CPhaseTester

#include <boost/test/included/unit_test.hpp>
#include "CPhase.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	CPhase cp(0, 1, 3.14);
	BOOST_VERIFY(boost::get<double>(cp.getParameter(0)) == 3.14);
	BOOST_VERIFY(cp.toString("qreg") == "CPhase(3.14) qreg0,qreg1");
	BOOST_VERIFY(cp.bits().size() == 2);
	BOOST_VERIFY(cp.bits()[0] == 0);
	BOOST_VERIFY(cp.bits()[1] == 1);
	BOOST_VERIFY(cp.name() == "CPhase");

	CPhase cp2(44, 45, 1.71234);

	BOOST_VERIFY(boost::get<double>(cp2.getParameter(0)) == 1.71234);
	BOOST_VERIFY(cp2.toString("qreg") == "CPhase(1.71234) qreg44,qreg45");
	BOOST_VERIFY(cp2.bits().size() == 2);
	BOOST_VERIFY(cp2.bits()[0] == 44);
	BOOST_VERIFY(cp2.bits()[1] == 45);
	BOOST_VERIFY(cp2.name() == "CPhase");

}

