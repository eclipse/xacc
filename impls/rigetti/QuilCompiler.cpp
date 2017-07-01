/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
#include "GateQIR.hpp"
#include "QuilCompiler.hpp"
#include "QuilVisitor.hpp"

namespace xacc {

namespace quantum {

QuilCompiler::QuilCompiler() {
}

std::shared_ptr<IR> QuilCompiler::compile(const std::string& src,
		std::shared_ptr<Accelerator> acc) {
	accelerator = acc;
	return compile(src);
}

std::shared_ptr<IR> QuilCompiler::compile(const std::string& src) {

	// Need to analyze string for function calls
	// FIXME for now just assume one function

	auto ir = std::make_shared<GateQIR>();

	auto f = std::make_shared<GateFunction>("name");

	// First off, split the string into lines
	std::vector<std::string> lines;

	boost::split(lines, src, boost::is_any_of("\n"));

	auto firstCodeLine = lines.begin() + 1;
	auto lastCodeLine = lines.end() - 2;
	std::vector<std::string> quil(firstCodeLine, lastCodeLine);
	bool isConditional = false;
	std::shared_ptr<xacc::Instruction> instruction;
	auto gateRegistry = xacc::quantum::GateInstructionRegistry::instance();
	std::vector<std::shared_ptr<xacc::Instruction>> measurements;
	std::string currentLabel;
	std::shared_ptr<ConditionalFunction> currentConditional;

	for (auto quilLine : quil) {
		boost::trim(quilLine);

		if (!boost::starts_with(quilLine, "#")) {
			std::vector<std::string> splitSpaces;
			std::vector<int> qubits;

			boost::split(splitSpaces, quilLine, boost::is_any_of(" "));

			auto gateName = splitSpaces[0];
			boost::trim(gateName);

			if (gateName == "MEASURE") {
				qubits.push_back(std::stoi(splitSpaces[1]));
				boost::replace_all(splitSpaces[2], "[", "");
				boost::replace_all(splitSpaces[2], "]", "");
				int classicalBit = std::stoi(splitSpaces[2]);
				instruction = gateRegistry->create("Measure", qubits);
				xacc::InstructionParameter p(classicalBit);
				instruction->setParameter(0,p);
				measurements.push_back(instruction);
			} else if (gateName == "JUMP-UNLESS") {
				isConditional = true;
				currentLabel = splitSpaces[1];
				boost::replace_all(splitSpaces[2], "[", "");
				boost::replace_all(splitSpaces[2], "]", "");
				auto classicalIdx = std::stoi(splitSpaces[2]);
				int conditionalQubit = -1;
				for (auto m : measurements) {
					if (boost::get<int>(m->getParameter(0)) == classicalIdx) {
						conditionalQubit = m->bits()[0];
						break;
					}
				}

				currentConditional = std::make_shared<
												xacc::quantum::ConditionalFunction>(
												conditionalQubit);
				continue;
			} else if (gateName == "LABEL") {
				auto label = splitSpaces[1];
				if (label == currentLabel) {
					isConditional = false;
					f->addInstruction(currentConditional);
				}
				continue;
			} else {

				for (int i = 1; i < splitSpaces.size(); i++) {
					qubits.push_back(std::stoi(splitSpaces[i]));
				}

				instruction = gateRegistry->create(gateName, qubits);

			}

			if (isConditional) {
				currentConditional->addInstruction(instruction);
			} else {
				f->addInstruction(instruction);
			}

		}
	}

	ir->addKernel(f);

	return ir;
}

const std::string QuilCompiler::translate(const std::string& bufferVariable,
		std::shared_ptr<Function> function) {
	auto visitor = std::make_shared<QuilVisitor>();
	InstructionIterator it(function);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();
		if (nextInst->isEnabled()) {
			nextInst->accept(visitor);
		}
	}

	return visitor->getQuilString();
}

}

}
