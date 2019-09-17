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
#include <gtest/gtest.h>
#include "Instruction.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
using namespace xacc;


TEST(UCCSDTester,checkUCCSD) {

	xacc::Initialize();
 auto tmp = xacc::getService<Instruction>("uccsd");//std::make_shared<QFT>();
  auto uccsd = std::dynamic_pointer_cast<CompositeInstruction>(tmp);
	EXPECT_TRUE(uccsd->expand({std::make_pair("ne", 2), std::make_pair("nq", 4)}));

	std::cout << uccsd->toString() << "\n";
	xacc::Finalize();
}


int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
