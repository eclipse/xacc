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
#include <gtest/gtest.h>
#include "Z.hpp"

using namespace xacc::quantum;

TEST(ZTester,checkCreation) {

	Z z(0);
	EXPECT_TRUE(z.toString("qreg") == "Z qreg0");
	EXPECT_TRUE(z.bits().size() == 1);
	EXPECT_TRUE(z.bits()[0] == 0);
	EXPECT_TRUE(z.name() == "Z");

	Z z2(44);
	EXPECT_TRUE(z2.toString("qreg") == "Z qreg44");
	EXPECT_TRUE(z2.bits().size() == 1);
	EXPECT_TRUE(z2.bits()[0] == 44);
	EXPECT_TRUE(z2.name() == "Z");


}
int main(int argc, char** argv) {
	    ::testing::InitGoogleTest(&argc, argv);
		    return RUN_ALL_TESTS();
}
