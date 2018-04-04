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
			writer->String(f->name());

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
//		writer->String(function->name());
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

	void visit(Identity& i) {
		baseGateInst(dynamic_cast<GateInstruction&>(i));
	}

	void visit(Hadamard& h) {
		baseGateInst(dynamic_cast<GateInstruction&>(h));
	}

	void visit(CNOT& cn) {
		baseGateInst(dynamic_cast<GateInstruction&>(cn));
	}

	void visit(CZ& cz) {
		baseGateInst(dynamic_cast<GateInstruction&>(cz));
	}

	void visit(Swap& s) {
		baseGateInst(dynamic_cast<GateInstruction&>(s));
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
		writer->String(cn.name());

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
		writer->String(inst.name().c_str());
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
