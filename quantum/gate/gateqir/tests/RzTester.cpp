
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
#define BOOST_TEST_MODULE RzTester

#include <boost/test/included/unit_test.hpp>
#include "Rz.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	Rz rz(0, 0, 0, 3.14);
	BOOST_VERIFY(rz.getParameter(0) == 3.14);
	BOOST_VERIFY(rz.toString("qreg") == "Rz(3.140000) qreg0");
	BOOST_VERIFY(rz.getId() == 0);
	BOOST_VERIFY(rz.layer() == 0);
	BOOST_VERIFY(rz.qubits().size() == 1);
	BOOST_VERIFY(rz.qubits()[0] == 0);
	BOOST_VERIFY(rz.getName() == "Rz");

	Rz rz2(3, 22, 44, 1.71234);
	BOOST_VERIFY(rz2.getParameter(0) == 1.71234);
	BOOST_VERIFY(rz2.toString("qreg") == "Rz(1.712340) qreg44");
	BOOST_VERIFY(rz2.getId() == 3);
	BOOST_VERIFY(rz2.layer() == 22);
	BOOST_VERIFY(rz2.qubits().size() == 1);
	BOOST_VERIFY(rz2.qubits()[0] == 44);
	BOOST_VERIFY(rz2.getName() == "Rz");


}

BOOST_AUTO_TEST_CASE(checkAutoRegistration) {

	auto rz = ParameterizedGateInstructionRegistry<double>::instance()->create("Rz", 1, 1, std::vector<int>{0}, 3.14);
//
	BOOST_VERIFY(rz->getName() == "Rz");
	BOOST_VERIFY(rz->getParameter(0) == 3.14);
}
