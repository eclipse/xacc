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
#ifndef QUANTUM_GATE_SCAFFOLD_SCAFFCCAPI_HPP_
#define QUANTUM_GATE_SCAFFOLD_SCAFFCCAPI_HPP_

#include <cstdlib>
#include "QCIError.hpp"
#include <iostream>
#include <fstream>

using namespace qci::common;

namespace scaffold {

/**
 * The ScaffCCAPI is a simple utility class that provides
 * methods for interacting with an installed Scaffold compiler.
 * It's main function is to take user-specified XACC kernel
 * source code and compile it to flatted QASM using the
 * Scaffold compiler.
 */
class ScaffCCAPI {

public:

	/**
	 * Return a string containing flatted QASM representing
	 * the provided high level Scaffold source code. This method
	 * assumes that the Scaffold compiler is on the current user's
	 * PATH and is called scaffcc.
	 *
	 * @param source The scaffold kernel source code.
	 * @return flattenedQASM The QASM representation of the given source.
	 */
	std::string getFlatQASMFromSource(const std::string& source) {

		// Check if scaffcc exists on the PATH
		if (std::system("which scaffcc > /dev/null 2>&1") == 0) {

			// Create a temporary scaffold soruce file
			std::ofstream tempSrcFile(".tmpSrcFile.scaffold");
			tempSrcFile << source;
			tempSrcFile.close();

			// Execute the scaffold compiler
			std::system("scaffcc -fp .tmpSrcFile.scaffold &> /dev/null");

			// Remove the temporary source file, we don't need it anymore
			std::remove(".tmpSrcFile.scaffold");

			// Read in the generated QASM
			std::ifstream flatQASM(".tmpSrcFile.qasmf");
			std::string qasm((std::istreambuf_iterator<char>(flatQASM)),
					std::istreambuf_iterator<char>());

			// Remove created scaffold files.
			std::remove(".tmpSrcFile.qasmf");
			std::remove(".tmpSrcFile.qasmh");

			// Return the QASM
			return qasm;
		} else {
			QCIError(
					"Cannot find scaffold compiler. Make sure scaffcc is in PATH and executable.")
		}
	}
};

}



#endif
