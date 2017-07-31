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
#include "XACC.hpp"

const std::string src =
	"__qpu__ factoring15() {\n"
		"   0 0 20\n"
		"   1 1 50\n"
		"   2 2 60\n"
		"   4 4 50\n"
		"   5 5 60\n"
		"   6 6 -160\n"
		"   1 4 -1000\n"
		"   2 5 -1000\n"
		"   0 4 -14\n"
		"   0 5 -12\n"
		"   0 6 32\n"
		"   1 5 68\n"
		"   1 6 -128\n"
		"   2 6 -128\n"
	"}";

int main (int argc, char** argv) {

	// Initialize the XACC Framework
	xacc::Initialize(argc, argv);

	(*xacc::RuntimeOptions::instance())["compiler"] = "dwave-qmi";

	auto qpu = xacc::getAccelerator("dwave");

	auto qubits = qpu->createBuffer("qbits");

	// Create a Program
	xacc::Program program(qpu, src);

	auto factoring15 = program.getKernel("factoring15");

	// Execute!
	factoring15(qubits);

	// Finalize the XACC Framework
	xacc::Finalize();

	return 0;
}



