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
#include "SimulatedQubits.hpp"

using namespace xacc::quantum;

TEST(SimulatedQubitsTester,checkConstruction) {

	Eigen::VectorXcd initialState1(8);
	initialState1.setZero();
	initialState1(0) = 1;
	SimulatedQubits qubits1("name1", 3);
	EXPECT_TRUE(qubits1.size() == 3);
	EXPECT_TRUE(qubits1.name() == "name1");
	EXPECT_TRUE(qubits1.getState().size() == 8);
	EXPECT_TRUE(qubits1.getState() == initialState1);

	Eigen::VectorXcd initialState2(4);
	initialState2.setZero();
	initialState2(0) = 1;
	SimulatedQubits qubits2("name2", 2);
	EXPECT_TRUE(qubits2.size() == 2);
	EXPECT_TRUE(qubits2.name() == "name2");
	EXPECT_TRUE(qubits2.getState().size() == 4);
	EXPECT_TRUE(qubits2.getState() == initialState2);

}

int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}

