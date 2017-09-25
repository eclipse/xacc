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
#define BOOST_TEST_MODULE MeasureTester

#include <boost/test/included/unit_test.hpp>
#include "Measure.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Measure meas(0, 1);
	BOOST_VERIFY(boost::get<int>(meas.getParameter(0)) == 1);
	BOOST_VERIFY(meas.toString("qreg") == "Measure qreg0");
	BOOST_VERIFY(meas.bits().size() == 1);
	BOOST_VERIFY(meas.bits()[0] == 0);
	BOOST_VERIFY(meas.getName() == "Measure");


}

BOOST_AUTO_TEST_CASE(checkAutoRegistration) {

	xacc::InstructionParameter p(1);
	auto meas = GateInstructionRegistry::instance()->create("Measure", std::vector<int>{0});
	meas->setParameter(0, p);
	BOOST_VERIFY(meas->getName() == "Measure");
	BOOST_VERIFY(boost::get<int>(meas->getParameter(0)) == 1);
}
