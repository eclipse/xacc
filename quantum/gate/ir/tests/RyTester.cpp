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
#include "Ry.hpp"

using namespace xacc::quantum;

TEST(RyTester,checkCreation) {

	Ry ry(0, 3.14);
	EXPECT_TRUE(boost::get<double>(ry.getParameter(0)) == 3.14);
	EXPECT_TRUE(ry.toString("qreg") == "Ry(3.14) qreg0");
	EXPECT_TRUE(ry.bits().size() == 1);
	EXPECT_TRUE(ry.bits()[0] == 0);
	EXPECT_TRUE(ry.name() == "Ry");

	Ry ry2(44, 1.71234);

	EXPECT_TRUE(boost::get<double>(ry2.getParameter(0)) == 1.71234);
	EXPECT_TRUE(ry2.toString("qreg") == "Ry(1.71234) qreg44");
	EXPECT_TRUE(ry2.bits().size() == 1);
	EXPECT_TRUE(ry2.bits()[0] == 44);
	EXPECT_TRUE(ry2.name() == "Ry");


}

int main(int argc, char** argv) {
	    ::testing::InitGoogleTest(&argc, argv);
		    return RUN_ALL_TESTS();
}
