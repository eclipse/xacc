/*
 * MyASTConsumer.hpp
 *
 *  Created on: Apr 19, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_COMPILERS_SCAFFOLD_SCAFFOLDASTCONSUMER_HPP_
#define QUANTUM_GATE_COMPILERS_SCAFFOLD_SCAFFOLDASTCONSUMER_HPP_

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Type.h"

#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "GateInstruction.hpp"
#include "GateFunction.hpp"
#include "ConditionalFunction.hpp"
#include "GateQIR.hpp"

using namespace clang;

namespace xacc {
namespace quantum {
class ScaffoldASTConsumer: public clang::ASTConsumer,
		public clang::RecursiveASTVisitor<ScaffoldASTConsumer> {

protected:

	std::string cbitVarName;
	std::string qbitVarName;

	std::shared_ptr<xacc::quantum::GateQIR> ir;

	std::shared_ptr<xacc::quantum::GateFunction> function;
	std::shared_ptr<xacc::quantum::ConditionalFunction> currentConditional;

	int nCallExprToSkip = 0;

	std::map<std::string, int> cbitRegToMeasuredQubit;

public:

	ScaffoldASTConsumer() : ir(std::make_shared<xacc::quantum::GateQIR>()) {}

	// Override the method that gets called for each parsed top-level
	// declaration.
	virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
		for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
			// Traverse the declaration using our AST visitor.
			TraverseDecl(*b);
		}
		return true;
	}

	bool VisitStmt(clang::Stmt *s) {
		if (isa<IfStmt>(s)) {
			auto ifStmt = cast<IfStmt>(s);
			clang::LangOptions lo;
			clang::PrintingPolicy policy(lo);
			std::string ifStr;
			llvm::raw_string_ostream ifS(ifStr);
			ifStmt->printPretty(ifS, nullptr, policy);
			//		std::cout << "HELLO IF:\n" << ifS.str() << "\n";

			if (const auto binOp = llvm::dyn_cast<BinaryOperator>(
					ifStmt->getCond())) {
				if (binOp->getOpcode() == BO_EQ) {
					// We have an equality check...
					auto LHS = binOp->getLHS();
					std::string str;
					llvm::raw_string_ostream s(str);
					LHS->printPretty(s, nullptr, policy);
					//				std::cout << "LHS IF: " << s.str() << "\n";
					if (boost::contains(s.str(), cbitVarName)) {

						auto RHS = binOp->getRHS();
						std::string rhsstr;
						llvm::raw_string_ostream rhss(rhsstr);
						RHS->printPretty(rhss, nullptr, policy);
						//					std::cout << "RHS IF: " << rhss.str() << "\n";

						auto thenCode = ifStmt->getThen();
						std::string thenStr;
						llvm::raw_string_ostream thenS(thenStr);
						thenCode->printPretty(thenS, nullptr, policy);
						auto then = thenS.str();
						//					std::cout << "ThenStmt:\n" << then << "\n";
						then.erase(std::remove(then.begin(), then.end(), '\t'),
								then.end());
						boost::replace_all(then, "{\n", "");
						boost::replace_all(then, "}\n", "");
						boost::replace_all(then, "    ", "");
						boost::trim(then);

						int conditionalQubit = cbitRegToMeasuredQubit[s.str()];
						currentConditional = std::make_shared<
								xacc::quantum::ConditionalFunction>(
								conditionalQubit);

						std::vector<std::string> vec;
						boost::split(vec, then, boost::is_any_of("\n"));
						nCallExprToSkip = vec.size();
						//					std::cout << "NCALLEXPRTOSKIP = " << nCallExprToSkip << "\n";
					}
				}
			}
		}
		return true;
	}
	bool VisitDecl(clang::Decl *d) {
		if (isa<VarDecl>(d)) {
			auto varDecl = cast<VarDecl>(d);
			auto varType = varDecl->getType().getAsString();
			if (boost::contains(varType, "cbit")) {
				cbitVarName = varDecl->getDeclName().getAsString();
				//			std::cout << "Found " << cbitVarName << "\n";
			} else if (boost::contains(varType, "qbit")) {
				qbitVarName = varDecl->getDeclName().getAsString();
				boost::trim(qbitVarName);
				//			std::cout << "Found " << qbitVarName << "\n";
			}
		} else if (isa<FunctionDecl>(d)) {
			auto c = cast<FunctionDecl>(d);

			// Skip first parameter since its supposed to be the qubit register
			std::vector<xacc::InstructionParameter> parameters;
			clang::FunctionDecl::param_iterator pBegin = c->param_begin();
			clang::FunctionDecl::param_iterator pEnd = c->param_end();
			for (auto i = pBegin+1; i != pEnd; ++i) {
				auto paramName = (*i)->getNameAsString();
				boost::trim(paramName);
				xacc::InstructionParameter p(paramName);
				parameters.push_back(p);
			}
			function = std::make_shared<xacc::quantum::GateFunction>(
					c->getDeclName().getAsString(), parameters);
			ir->addKernel(function);
		}
		return true;
	}
	bool VisitCallExpr(CallExpr * c) {
		clang::LangOptions lo;
		clang::PrintingPolicy policy(lo);
		auto q = c->getType();
		auto t = q.getTypePtrOrNull();

		if (t != NULL) {
			bool isParameterizedInst = false;
			auto fd = c->getDirectCallee();
			auto gateName = fd->getNameInfo().getAsString();
			auto gateRegistry = xacc::quantum::GateInstructionRegistry::instance();
			auto availableGates = gateRegistry->getRegisteredIds();
			std::shared_ptr<xacc::Instruction> inst;

			if (!std::any_of(availableGates.cbegin(), availableGates.cend(),
					[=](std::string i) {return i == gateName;})) {

				if (ir->kernelExists(gateName)) {
					inst = ir->getKernel(gateName);
				}

			} else {

				std::vector<int> qubits;
				std::vector<xacc::InstructionParameter> params;
				for (auto i = c->arg_begin(); i != c->arg_end(); ++i) {
					std::string arg;
					llvm::raw_string_ostream argstream(arg);
					i->printPretty(argstream, nullptr, policy);
					auto argStr = argstream.str();
//				std::cout << "Arg: " << argstream.str() << "\n";

					if (boost::contains(argStr, qbitVarName)) {
						boost::replace_all(argStr, qbitVarName, "");
						boost::replace_all(argStr, "[", "");
						boost::replace_all(argStr, "]", "");
						qubits.push_back(std::stoi(argStr));
					} else {
						// This is a gate parameter!!!
						isParameterizedInst = true;

						// This parameter could just be a hard-coded value
						// or it could be a reference to a variable parameter...
						try {
							double d = boost::lexical_cast<double>(argStr);
							params.push_back(d);
						} catch (const boost::bad_lexical_cast &) {
							params.push_back(argStr);
						}
					}
				}

				if (isParameterizedInst) {

					int idx = 0;
					inst = gateRegistry->create(gateName, qubits);
					for (auto p : params) {
						inst->setParameter(idx, p);
						idx++;
					}

				} else if (gateName != "MeasZ") {

					inst = gateRegistry->create(gateName, qubits);
				}

			}

			if (gateName != "MeasZ") {

				if (nCallExprToSkip == 0) {
					function->addInstruction(inst);
				} else {
					//				std::cout << "Adding Conditional Inst: " << gateName << "\n";
					currentConditional->addInstruction(inst);
					nCallExprToSkip--;

					if (nCallExprToSkip == 0) {
						function->addInstruction(currentConditional);
					}
				}
			}
		}

		return true;
	}
	bool VisitBinaryOperator(BinaryOperator *b) {

		clang::LangOptions lo;
		clang::PrintingPolicy policy(lo);

		if (b->isAssignmentOp()) {
			auto rhs = b->getRHS();
			std::string rhsstr;
			llvm::raw_string_ostream rhss(rhsstr);
			rhs->printPretty(rhss, nullptr, policy);
			auto rhsString = rhss.str();

			if (boost::contains(rhsString, "MeasZ")) {
				auto lhs = b->getLHS();
				std::string lhsstr;
				llvm::raw_string_ostream lhss(lhsstr);
				lhs->printPretty(lhss, nullptr, policy);
				auto lhsString = lhss.str();

				boost::replace_all(lhsString, cbitVarName, "");
				boost::replace_all(lhsString, "[", "");
				boost::replace_all(lhsString, "]", "");

				boost::replace_all(rhsString, "MeasZ", "");
				boost::replace_all(rhsString, "(", "");
				boost::replace_all(rhsString, ")", "");
				boost::replace_all(rhsString, qbitVarName, "");
				boost::replace_all(rhsString, "[", "");
				boost::replace_all(rhsString, "]", "");

				// lhsString now just contains the classical index bit
				auto inst =
						xacc::quantum::GateInstructionRegistry::instance()->create(
								"Measure",
								std::vector<int> { std::stoi(rhsString) });
				xacc::InstructionParameter classicalIdx(std::stoi(lhsString));
				inst->setParameter(0, classicalIdx);

				cbitRegToMeasuredQubit.insert(
						std::make_pair(lhss.str(), std::stoi(rhsString)));

				function->addInstruction(inst);

			}
		}

		return true;
	}
	std::shared_ptr<xacc::IR> getIR() {
		return ir;
	}

	const std::string getQubitVariableName() {
		return qbitVarName;
	}

	virtual ~ScaffoldASTConsumer() {
	}

};
}
}

#endif
