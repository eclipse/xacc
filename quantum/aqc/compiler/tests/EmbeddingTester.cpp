/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
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




