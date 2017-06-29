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
#ifndef QUANTUM_GATE_JSONVISITOR_HPP_
#define QUANTUM_GATE_JSONVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace xacc {

namespace quantum {

using Writer = PrettyWriter<StringBuffer>;

/**
 * FIXME write this
 */
class JsonVisitor: public AllGateVisitor {

protected:
	std::shared_ptr<StringBuffer> buffer;
	std::shared_ptr<Writer> writer;
	std::shared_ptr<Function> function;
	std::shared_ptr<InstructionIterator> topLevelInstructionIterator;

	std::vector<std::shared_ptr<Function>> functions;

public:

	JsonVisitor(std::shared_ptr<xacc::Function> f) :
			buffer(std::make_shared<StringBuffer>()), writer(
					std::make_shared<PrettyWriter<StringBuffer>>(
							*buffer.get())), functions{f} {
	}

	JsonVisitor(std::vector<std::shared_ptr<xacc::Function>> fs) :
			buffer(std::make_shared<StringBuffer>()), writer(
					std::make_shared<PrettyWriter<StringBuffer>>(
							*buffer.get())), functions(fs) {
	}

	std::string write() {
		writer->StartObject();

		writer->String("kernels");
		writer->StartArray();
		for (auto f : functions) {
			// This is a Function, start it as an Object
			writer->StartObject();

			writer->String("function");
			writer->String(f->getName());

			// All functions have instructions, start
			// that array here.
			writer->String("instructions");
			writer->StartArray();

			topLevelInstructionIterator = std::make_shared<
					xacc::InstructionIterator>(f);
			while (topLevelInstructionIterator->hasNext()) {
				// Get the next node in the tree
				auto nextInst = topLevelInstructionIterator->next();
				nextInst->accept(this);
			}

			// End Instructions
			writer->EndArray();

			// End Function
			writer->EndObject();
		}
		writer->EndArray();
		writer->EndObject();
		return buffer->GetString();
	}

//	std::string write() {
//		// This is a Function, start it as an Object
//		writer->StartObject();
//		writer->String("function");
//		writer->String(function->getName());
//
//		// All functions have instructions, start
//		// that array here.
//		writer->String("instructions");
//		writer->StartArray();
//
//		topLevelInstructionIterator = std::make_shared<xacc::InstructionIterator>(function);
//		while (topLevelInstructionIterator->hasNext()) {
//			// Get the next node in the tree
//			auto nextInst = topLevelInstructionIterator->next();
//			nextInst->accept(this);
//		}
//
//		// End Instructions
//		writer->EndArray();
//
//		// End Function
//		writer->EndObject();
//		return buffer->GetString();
//	}


	void visit(Hadamard& h) {
		baseGateInst(dynamic_cast<GateInstruction&>(h));
	}

	void visit(CNOT& cn) {
		baseGateInst(dynamic_cast<GateInstruction&>(cn));
	}

	void visit(Rz& rz) {
		baseGateInst(dynamic_cast<GateInstruction&>(rz), false);
		writer->String("angle");
		auto p = rz.getParameter(0);
		switch(p.which()) {
				case 0: writer->Int(boost::get<int>(p));break;
				case 1: writer->Double(boost::get<double>(p));break;
				case 2: writer->Double((double)boost::get<float>(p));break;
				case 3: writer->String(boost::get<std::string>(p));break;
			}
		writer->EndObject();
	}

	void visit(Rx& rx) {
		baseGateInst(dynamic_cast<GateInstruction&>(rx), false);
		writer->String("angle");
		auto p = rx.getParameter(0);
		switch(p.which()) {
					case 0: writer->Int(boost::get<int>(p));break;
					case 1: writer->Double(boost::get<double>(p));break;
					case 2: writer->Double((double)boost::get<float>(p));break;
					case 3: writer->String(boost::get<std::string>(p));break;
				}
		writer->EndObject();
	}

	void visit(Ry& ry) {
		baseGateInst(dynamic_cast<GateInstruction&>(ry), false);
		writer->String("angle");
		auto p = ry.getParameter(0);

		switch(p.which()) {
			case 0: writer->Int(boost::get<int>(p));break;
			case 1: writer->Double(boost::get<double>(p));break;
			case 2: writer->Double((double)boost::get<float>(p));break;
			case 3: writer->String(boost::get<std::string>(p));break;
		}
		writer->EndObject();
	}

	void visit(CPhase& cp) {
		baseGateInst(dynamic_cast<GateInstruction&>(cp), false);
		writer->String("angle");
		auto p = cp.getParameter(0);

		switch(p.which()) {
			case 0: writer->Int(boost::get<int>(p));break;
			case 1: writer->Double(boost::get<double>(p));break;
			case 2: writer->Double((double)boost::get<float>(p));break;
			case 3: writer->String(boost::get<std::string>(p));break;
		}
		writer->EndObject();
	}

	void visit(ConditionalFunction& cn) {
		writer->StartObject();
		writer->String("conditional_function");
		writer->String(cn.getName());

		writer->String("conditional_qubit");
		writer->Int(cn.getConditionalQubit());
		writer->String("instructions");
		writer->StartArray();

		auto cnAsPtr = std::make_shared<ConditionalFunction>(cn);
		int nInsts = cnAsPtr->nInstructions();
		xacc::InstructionIterator it(cnAsPtr);
		it.next();
		while (it.hasNext()) {
			// Get the next node in the tree
			auto nextInst = it.next();
			nextInst->accept(this);
		}

		// End Instructions
		writer->EndArray();
		writer->EndObject();

		// Move the Top Level Iterator past these instructions that were in
		// the conditional function
		for (int i = 0; i < nInsts; i++) topLevelInstructionIterator->next();

	}

	void visit(Measure& cn) {
		baseGateInst(dynamic_cast<GateInstruction&>(cn), false);
		writer->String("classicalBitIdx");
		writer->Int(cn.getClassicalBitIndex());
		writer->EndObject();
	}

	void visit(X& cn) {
		baseGateInst(dynamic_cast<GateInstruction&>(cn));
	}

	void visit(Y& y) {
		baseGateInst(dynamic_cast<GateInstruction&>(y));
	}

	void visit(Z& z) {
		baseGateInst(dynamic_cast<GateInstruction&>(z));
	}

	void visit(GateFunction& function) {
	}

protected:

	void baseGateInst(GateInstruction& inst, bool endObject = true) {
		writer->StartObject();
		writer->String("gate");
		writer->String(inst.getName().c_str());
		writer->String("enabled");
		writer->Bool(inst.isEnabled());
		writer->String("qubits");
		writer->StartArray();
		for (auto qi : inst.bits()) {
			writer->Int(qi);
		}
		writer->EndArray();
		if (endObject) {
			writer->EndObject();
		}

	}

};
}
}
#endif
