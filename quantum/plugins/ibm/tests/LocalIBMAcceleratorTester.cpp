/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "CommonGates.hpp"
#include "Circuit.hpp"

using namespace xacc::quantum;

TEST(LocalIBMAcceleratorTester,checkSimple) {
    xacc::Initialize();

	auto acc = xacc::getAccelerator("local-ibm", {std::make_pair("shots",8192),std::make_pair("cx-p-depol",.15)});
	auto buffer = xacc::qalloc(2);

	auto f = std::make_shared<Circuit>("foo");

	auto h = std::make_shared<Hadamard>(0);
	auto cn1 = std::make_shared<CNOT>(0,1);
	auto m0 = std::make_shared<Measure>(0);
	auto m1 = std::make_shared<Measure>(1);

	f->addInstructions({h,cn1,m0,m1});

	acc->execute(buffer, f);

    buffer->print();
	xacc::Finalize();
}

int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
