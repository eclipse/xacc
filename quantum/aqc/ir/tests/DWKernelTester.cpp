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
#define BOOST_TEST_MODULE DWKernelTester

#include <boost/test/included/unit_test.hpp>
#include "DWKernel.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkDWKernelConstruction) {

	auto qmi = std::make_shared<DWQMI>(0, 1, 2.2);
	auto qmi2 = std::make_shared<DWQMI>(0);
	auto qmi3 = std::make_shared<DWQMI>(22, 3.3);

	DWKernel kernel("foo");
	kernel.addInstruction(qmi);
	kernel.addInstruction(qmi2);
	kernel.addInstruction(qmi3);

	BOOST_VERIFY(kernel.nInstructions() == 3);
	BOOST_VERIFY(kernel.name() == "foo");
	BOOST_VERIFY(kernel.getInstruction(0) == qmi);
	BOOST_VERIFY(kernel.getInstruction(1) == qmi2);
	BOOST_VERIFY(kernel.getInstruction(2) == qmi3);

	const std::string expected = "0 1 2.2\n0 0 0\n22 22 3.3\n";

	BOOST_VERIFY(kernel.toString("") == expected);
}

