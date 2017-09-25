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
#define BOOST_TEST_MODULE EmbeddingTester

#include <boost/test/included/unit_test.hpp>
#include "Embedding.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkLoadPersist) {

	Embedding embedding;

	embedding.insert(std::make_pair(0, std::vector<int>{0, 4}));
	embedding.insert(std::make_pair(1, std::vector<int>{1, 5}));
	embedding.insert(std::make_pair(2, std::vector<int>{2, 6}));
	embedding.insert(std::make_pair(3, std::vector<int>{3}));
	embedding.insert(std::make_pair(4, std::vector<int>{7}));

	std::stringstream ss;

	embedding.persist(ss);

	std::string expected =
			"0: 0 4\n"
			"1: 1 5\n"
			"2: 2 6\n"
			"3: 3\n"
			"4: 7\n";


	BOOST_VERIFY(ss.str() == expected);

	Embedding toBeLoaded;
	std::istringstream iss(expected);

	toBeLoaded.load(iss);

	toBeLoaded.persist(std::cout);
	BOOST_VERIFY(toBeLoaded == embedding);
}




