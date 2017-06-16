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
#include "GateFunction.hpp"
#include "GateInstruction.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <regex>

using namespace boost::program_options;

int main(int argc, char** argv) {
	// Declare the supported options.
	options_description desc("QASM to XACC Graph IR Utility");
	desc.add_options()("help",
			"The following arguments are valid for this utility.")("input",
			value<std::string>()->required(), "Provide the name of the input qasm file.")(
			"output", value<std::string>()->default_value("graphIR.xir"),
			"Provide the name of the output file *.xir file name - "
			"default is graphIR.xir");
	// Get the user-specified compiler parameters as a map
	variables_map compileParameters;
	store(command_line_parser(argc, argv).options(desc).run(),
			compileParameters);

	auto inputFileName = compileParameters["input"].as<std::string>();
	auto outputFileName = (
			compileParameters.count("output") ?
					compileParameters["output"].as<std::string>() :
					"graphIR.xir");

	std::ifstream inputStream(inputFileName);
	std::ofstream outputStream(outputFileName);
	std::stringstream ss;
	if (!inputStream) {
		XACCError("Invalid input qasm file");
	}

	ss << inputStream.rdbuf();
	inputStream.close();

	auto flatQasmStr = ss.str();
	std::map<std::string, int> qubitVarNameToId;
	std::vector<std::string> qasmLines;
	std::vector<int> allQbitIds;
	std::regex newLineDelim("\n"), spaceDelim(" ");
	std::regex qubitDeclarations("\\s*qubit\\s*\\w+");
	std::sregex_token_iterator first{flatQasmStr.begin(), flatQasmStr.end(), newLineDelim, -1}, last;
	int nQubits = 0, qbitId = 0, layer = 1, gateId = 1;
	std::string qubitVarName;
	qasmLines = {first, last};

	// Let's now loop over the qubit declarations,
	// and construct a mapping of qubit var names to integer id,
	// and get the total number of qubits
	for (auto i = std::sregex_iterator(flatQasmStr.begin(), flatQasmStr.end(),
					qubitDeclarations); i != std::sregex_iterator(); ++i) {
		std::string qubitLine = (*i).str();
		qubitLine.erase(std::remove(qubitLine.begin(), qubitLine.end(), '\n'), qubitLine.end());
		std::sregex_token_iterator first{qubitLine.begin(), qubitLine.end(), spaceDelim, -1}, last;
		std::vector<std::string> splitQubitLine = {first, last};
		qubitVarNameToId[splitQubitLine[1]] = qbitId;
		splitQubitLine[1].erase(
		  std::remove_if(splitQubitLine[1].begin(), splitQubitLine[1].end(), &isdigit),
		  splitQubitLine[1].end());
		qubitVarName = splitQubitLine[1];
		allQbitIds.push_back(qbitId);
		qbitId++;
	}

	// Set the number of qubits
	nQubits = qubitVarNameToId.size();
	int c = 0;

	auto function = std::make_shared<xacc::quantum::GateFunction>(inputFileName);

	for (auto line : qasmLines) {
		// If this is a gate line...
		if (!boost::contains(line, "qubit") && !boost::contains(line, "cbit")) {
			// Split the current qasm command at the spaces
			std::sregex_token_iterator first { line.begin(),
					line.end(), spaceDelim, -1 }, last;
			std::vector<std::string> gateCommand = {first, last};


			// Set the qubits this gate acts on
			std::vector<int> actingQubits;
			if (!boost::contains(gateCommand[1], ",")) {
				actingQubits.push_back(qubitVarNameToId[gateCommand[1]]);
				std::shared_ptr<xacc::Instruction> inst;
				if (gateCommand[0] == "MeasZ" || gateCommand[0] == "MeasX"
						|| gateCommand[0] == "Measure") {
					inst = xacc::quantum::ParameterizedGateInstructionRegistry<
							int>::instance()->create("Measure", actingQubits,
							c);
					c++;
				} else {
					if (gateCommand[0] == "T") {
						inst =
								xacc::quantum::ParameterizedGateInstructionRegistry<
										double>::instance()->create("Rz",
										actingQubits, (3.1415 / 4.0));
					} else {
						inst =
								xacc::quantum::GateInstructionRegistry::instance()->create(
										gateCommand[0], actingQubits);
					}
				}

				function->addInstruction(inst);
			} else {

				// FIXME Need to differentiate between qubits and parameters here
				// First we need the qubit register variable name

				int counter = 0;
				std::vector<std::string> splitComma;
				std::vector<double> props;
				boost::split(splitComma, gateCommand[1], boost::is_any_of(","));
				for (auto segment : splitComma) {
					if (boost::contains(segment, qubitVarName)) {
						actingQubits.push_back(qubitVarNameToId[segment]);
					} else {
						// This is not a qubit, it must be a parameter for gate
						std::cout << "param " << segment << "\n";
						props.push_back(std::stod(segment));
					}
				}

				if (props.empty()) {
					auto inst =
							xacc::quantum::GateInstructionRegistry::instance()->create(
									gateCommand[0], actingQubits);
					function->addInstruction(inst);
				} else {
					std::shared_ptr<xacc::Instruction> inst;
					// We only allow parameters of size 2 and doubles
					if (props.size() == 1) {
						inst =
								xacc::quantum::ParameterizedGateInstructionRegistry<
										double>::instance()->create(
										gateCommand[0], actingQubits,
										props[0]);
					} else if (props.size() == 2) {
						inst =
								xacc::quantum::ParameterizedGateInstructionRegistry<
										double, double>::instance()->create(
										gateCommand[0], actingQubits,
										props[0],
										props[1]);
					}
					function->addInstruction(inst);
				}
			}

		}
	}

	std::cout << "FTOSTR:\n" << function->toString("qreg") << "\n";

	return 0;
}
