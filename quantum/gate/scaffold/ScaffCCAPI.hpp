#ifndef QUANTUM_GATE_SCAFFOLD_SCAFFCCAPI_HPP_
#define QUANTUM_GATE_SCAFFOLD_SCAFFCCAPI_HPP_

#include <cstdlib>
#include "QCIError.hpp"
#include <iostream>
#include <fstream>

using namespace qci::common;

namespace scaffold {

class ScaffCCAPI {

public:

	std::string getFlatQASMFromSource(const std::string& source) {

		// Check if scaffcc exists on the PATH
		if (std::system("which scaffcc > /dev/null 2>&1") == 0) {

			std::ofstream tempSrcFile(".tmpSrcFile.scaffold");
			tempSrcFile << source;
			tempSrcFile.close();

			std::system("scaffcc -fp .tmpSrcFile.scaffold");
			std::remove(".tmpSrcFile.scaffold");

			std::ifstream flatQASM(".tmpSrcFile.qasmf");
			std::string qasm((std::istreambuf_iterator<char>(flatQASM)),
					std::istreambuf_iterator<char>());

			std::remove(".tmpSrcFile.qasmf");
			std::remove(".tmpSrcFile.qasmh");

			return qasm;
		} else {
			QCIError(
					"Cannot find scaffold compiler. Make sure scaffcc is in PATH and executable.")
		}
	}
};

}



#endif
