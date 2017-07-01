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
#include <regex>
#include "GateQIR.hpp"
#include "ScaffoldCompiler.hpp"
#include "ScaffoldIRToSrcVisitor.hpp"

using namespace clang;

namespace xacc {

namespace quantum {

ScaffoldCompiler::ScaffoldCompiler() {
	CI = std::make_shared<CompilerInstance>();
	CI->createDiagnostics(0, 0);
	TargetOptions targetOptions;
	targetOptions.Triple = llvm::sys::getDefaultTargetTriple();
	TargetInfo *pTargetInfo = TargetInfo::CreateTargetInfo(CI->getDiagnostics(),
			targetOptions);
	CI->setTarget(pTargetInfo);
	CI->createFileManager();
	CI->createSourceManager(CI->getFileManager());
	CI->createPreprocessor();
	CI->getPreprocessorOpts().UsePredefines = false;
	CI->createASTContext();
}

std::shared_ptr<IR> ScaffoldCompiler::compile(const std::string& src,
		std::shared_ptr<Accelerator> acc) {

	// Set the Kernel Source code
	kernelSource = src;

	// Replace the __qpu__ attribute with module
	if (boost::contains(kernelSource, "__qpu__")) {
		boost::replace_all(kernelSource, "__qpu__", "module");
	}

	// Make a copy, and then delete the body
	// of the kernel in that copy, leaving
	// just the functionDeclaration(qbit qvar) {}
	auto copyOfSrc = kernelSource;
	auto firstIdx = copyOfSrc.find_first_of("{");
	auto lastIdx = copyOfSrc.find_last_of("}");
	copyOfSrc.erase(firstIdx+1, lastIdx);
	copyOfSrc = copyOfSrc + "}";

	// Now we just want to get the qubit variable name,
	// so lets compile the function declaration

	// Create a temporary scaffold source file
	std::ofstream tempSrcFile(".tmpSrcFile.scaffold");
	tempSrcFile << copyOfSrc;
	tempSrcFile.close();
	const FileEntry *pFile = CI->getFileManager().getFile(
			".tmpSrcFile.scaffold");
	CI->getSourceManager().createMainFileID(pFile);
	CI->getASTContext().BuiltinInfo.InitializeBuiltins(
			CI->getPreprocessor().getIdentifierTable(), CI->getLangOpts());
	consumer = std::make_shared<ScaffoldASTConsumer>();
	clang::ParseAST(CI->getPreprocessor(), consumer.get(), CI->getASTContext());
	std::remove(".tmpSrcFile.scaffold");

	// Get the Qubit variable name
	auto qubitVarName = consumer->getQubitVariableName();

	// Update the kernelSource with the correct
	// number of qubits in this variable
	auto nBits = acc->getBuffer(qubitVarName)->size();
	auto searchFor = "qbit " + qubitVarName;
	auto replaceWith = "qbit " + qubitVarName + "[" + std::to_string(nBits) + "]";
	boost::replace_all(kernelSource, searchFor, replaceWith);

	// Create a compiler all over again
	CI = std::make_shared<CompilerInstance>();
	CI->createDiagnostics(0, 0);
	TargetOptions targetOptions;
	targetOptions.Triple = llvm::sys::getDefaultTargetTriple();
	TargetInfo *pTargetInfo = TargetInfo::CreateTargetInfo(CI->getDiagnostics(),
			targetOptions);
	CI->setTarget(pTargetInfo);
	CI->createFileManager();
	CI->createSourceManager(CI->getFileManager());
	CI->createPreprocessor();
	CI->getPreprocessorOpts().UsePredefines = false;
	CI->createASTContext();

	// Create a source file with all the code in it
	std::ofstream tempSrcFile2(".tmpSrcFile2.scaffold");
	tempSrcFile2 << kernelSource;
	tempSrcFile2.close();
	const FileEntry *pFile2 = CI->getFileManager().getFile(
			".tmpSrcFile2.scaffold");
	CI->getSourceManager().createMainFileID(pFile2);
	CI->getASTContext().BuiltinInfo.InitializeBuiltins(
			CI->getPreprocessor().getIdentifierTable(), CI->getLangOpts());

	// Compile the code!
	consumer = std::make_shared<ScaffoldASTConsumer>();
	clang::ParseAST(CI->getPreprocessor(), consumer.get(), CI->getASTContext());

	// Get the IR Function representation
	return consumer->getIR();

}

std::shared_ptr<IR> ScaffoldCompiler::compile(const std::string& src) {

	kernelSource = src;

	// Replace the __qpu__ attribute with module
	if (boost::contains(kernelSource, "__qpu__")) {
		boost::replace_all(kernelSource, "__qpu__", "module");
	}

	// Create a temporary scaffold source file
	std::ofstream tempSrcFile(".tmpSrcFile.scaffold");
	tempSrcFile << kernelSource;
	tempSrcFile.close();

	const FileEntry *pFile = CI->getFileManager().getFile(
			".tmpSrcFile.scaffold");
	CI->getSourceManager().createMainFileID(pFile);
	CI->getASTContext().BuiltinInfo.InitializeBuiltins(
			CI->getPreprocessor().getIdentifierTable(), CI->getLangOpts());

	consumer = std::make_shared<ScaffoldASTConsumer>();
	clang::ParseAST(CI->getPreprocessor(), consumer.get(), CI->getASTContext());

	return consumer->getIR();
}

const std::string ScaffoldCompiler::translate(const std::string& bufferVariable,
		std::shared_ptr<Function> function) {

	auto visitor = std::make_shared<ScaffoldIRToSrcVisitor>(bufferVariable);
	InstructionIterator it(function);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();
		if (nextInst->isEnabled()) {
			nextInst->accept(visitor);
		}
	}

	return visitor->getScaffoldString();
}



}

}
