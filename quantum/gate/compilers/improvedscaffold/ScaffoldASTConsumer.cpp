#include "ScaffoldASTConsumer.hpp"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "ParameterizedGateInstruction.hpp"

namespace scaffold {

bool ScaffoldASTConsumer::HandleTopLevelDecl(DeclGroupRef DR) {
	for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
		// Traverse the declaration using our AST visitor.
		TraverseDecl(*b);
	}
	return true;
}

bool ScaffoldASTConsumer::VisitDecl(Decl *d) {
	if (isa<VarDecl>(d)) {
		auto varDecl = cast<VarDecl>(d);
		auto varType = varDecl->getType().getAsString();
		if (boost::contains(varType, "cbit")) {
			cbitVarName = varDecl->getDeclName().getAsString();
			std::cout << "Found " << cbitVarName << "\n";
		} else if (boost::contains(varType, "qbit")) {
			qbitVarName = varDecl->getDeclName().getAsString();
			std::cout << "Found " << qbitVarName << "\n";
		}
	} else if (isa<FunctionDecl>(d)) {
		auto c = cast<FunctionDecl>(d);
		function = std::make_shared<xacc::quantum::GateFunction>(0,
				c->getDeclName().getAsString());
	}
	return true;
}

bool ScaffoldASTConsumer::VisitStmt(Stmt *s) {

	if (isa<IfStmt>(s)) {
		auto ifStmt = cast<IfStmt>(s);
		clang::LangOptions lo;
		clang::PrintingPolicy policy(lo);
		std::string ifStr;
		llvm::raw_string_ostream ifS(ifStr);
		ifStmt->printPretty(ifS, nullptr, policy);
		std::cout << "HELLO IF:\n" << ifS.str() << "\n";

		if (const auto binOp = llvm::dyn_cast<BinaryOperator>(
				ifStmt->getCond())) {
			if (binOp->getOpcode() == BO_EQ) {
				// We have an equality check...
				auto LHS = binOp->getLHS();
				std::string str;
				llvm::raw_string_ostream s(str);
				LHS->printPretty(s, nullptr, policy);
				if (boost::contains(s.str(), cbitVarName)) {

					auto RHS = binOp->getRHS();
					std::string rhsstr;
					llvm::raw_string_ostream rhss(rhsstr);
					RHS->printPretty(rhss, nullptr, policy);

					auto thenCode = ifStmt->getThen();
					std::string thenStr;
					llvm::raw_string_ostream thenS(thenStr);
					thenCode->printPretty(thenS, nullptr, policy);
					auto then = thenS.str();
					std::cout << "ThenStmt:\n" << then << "\n";
					then.erase(std::remove(then.begin(), then.end(), '\t'),
							then.end());
					boost::replace_all(then, "{\n", "");
					boost::replace_all(then, "}\n", "");
					boost::replace_all(then, "    ", "");
					boost::trim(then);
				}
			}
		}
	}
	return true;
}

bool ScaffoldASTConsumer::VisitCallExpr(CallExpr* c) {
	clang::LangOptions lo;
	clang::PrintingPolicy policy(lo);
	auto q = c->getType();
	auto t = q.getTypePtrOrNull();

	if (t != NULL) {
		bool isParameterizedInst = false;
		auto fd = c->getDirectCallee();
		std::cout << "HOWDY: " << fd->getNameInfo().getAsString() << "\n";
		auto gateName = fd->getNameInfo().getAsString();
		std::vector<int> qubits;
		std::vector<double> params;
		for (auto i = c->arg_begin(); i != c->arg_end(); ++i) {
			std::string arg;
			llvm::raw_string_ostream argstream(arg);
			i->printPretty(argstream, nullptr, policy);
			auto argStr = argstream.str();
			std::cout << "Arg: " << argstream.str() << "\n";

			if (boost::contains(argStr, qbitVarName)) {
				boost::replace_all(argStr, qbitVarName, "");
				boost::replace_all(argStr, "[", "");
				boost::replace_all(argStr, "]", "");
				qubits.push_back(std::stoi(argStr));
			} else {
				// This is a gate parameter!!!
				isParameterizedInst = true;
				params.push_back(std::stod(argStr));
			}
		}

		std::shared_ptr<xacc::quantum::GateInstruction> inst;
		if (isParameterizedInst) {
			if (params.size() == 1) {
				inst = xacc::quantum::ParameterizedGateInstructionRegistry<
						double>::instance()->create(gateName, currentInstId,
						currentInstLayer, qubits, params[0]);
			} else if (params.size() == 2) {
				inst = xacc::quantum::ParameterizedGateInstructionRegistry<
						double, double>::instance()->create(gateName,
						currentInstId, currentInstLayer, qubits, params[0],
						params[1]);
			} else {
				XACCError(
						"Can only handle 1 and 2 parameter gates... and only doubles... for now.");
			}
			std::cout << "CREATED A " << gateName << " gate\n";

		} else if (gateName != "MeasZ") {

			inst = xacc::quantum::GateInstructionRegistry::instance()->create(
					gateName, currentInstId, currentInstLayer, qubits);
			std::cout << "CREATED A " << gateName << " gate\n";
		}

		if (gateName != "MeasZ") function->addInstruction(inst);
	}

	return true;
}

bool ScaffoldASTConsumer::VisitBinaryOperator(BinaryOperator * b) {

	clang::LangOptions lo;
	clang::PrintingPolicy policy(lo);

	if (b->isAssignmentOp()) {
		auto rhs = b->getRHS();
		std::string rhsstr;
		llvm::raw_string_ostream rhss(rhsstr);
		rhs->printPretty(rhss, nullptr, policy);
		auto rhsString = rhss.str();
		std::cout << "HELLO BINOP: " << rhsString << "\n";

		if (boost::contains(rhsString, "MeasZ")) {
			auto lhs = b->getLHS();
			std::string lhsstr;
			llvm::raw_string_ostream lhss(lhsstr);
			lhs->printPretty(lhss, nullptr, policy);
			auto lhsString = lhss.str();
			std::cout << "HELLO BINOP LHS: " << lhsString << "\n";

			boost::replace_all(lhsString, cbitVarName, "");
			boost::replace_all(lhsString, "[","");
			boost::replace_all(lhsString, "]", "");

			boost::replace_all(rhsString, "MeasZ", "");
			boost::replace_all(rhsString, "(","");
			boost::replace_all(rhsString, ")", "");
			boost::replace_all(rhsString, qbitVarName, "");
			boost::replace_all(rhsString, "[","");
			boost::replace_all(rhsString, "]", "");

			// lhsString now just contains the classical index bit
			auto inst =
					xacc::quantum::ParameterizedGateInstructionRegistry<int>::instance()->create(
							"Measure", currentInstId, currentInstLayer,
							std::vector<int> { std::stoi(rhsString) },
							std::stoi(lhsString));

			function->addInstruction(inst);

		}
	}

	return true;
}
}
