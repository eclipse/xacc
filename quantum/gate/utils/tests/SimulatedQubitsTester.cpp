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
#define BOOST_TEST_MODULE QubitsTester

#include <boost/test/included/unit_test.hpp>
#include "SimulatedQubits.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkConstruction) {

	Eigen::VectorXcd initialState1(8);
	initialState1.setZero();
	initialState1(0) = 1;
	SimulatedQubits qubits1("name1", 3);
	BOOST_VERIFY(qubits1.size() == 3);
	BOOST_VERIFY(qubits1.name() == "name1");
	BOOST_VERIFY(qubits1.getState().size() == 8);
	BOOST_VERIFY(qubits1.getState() == initialState1);

	Eigen::VectorXcd initialState2(4);
	initialState2.setZero();
	initialState2(0) = 1;
	SimulatedQubits qubits2("name2", 2);
	BOOST_VERIFY(qubits2.size() == 2);
	BOOST_VERIFY(qubits2.name() == "name2");
	BOOST_VERIFY(qubits2.getState().size() == 4);
	BOOST_VERIFY(qubits2.getState() == initialState2);

}


