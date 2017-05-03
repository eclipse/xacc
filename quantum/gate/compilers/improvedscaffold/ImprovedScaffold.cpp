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
#include "ImprovedScaffold.hpp"
#include "GateQIR.hpp"

using namespace clang;

namespace xacc {

namespace quantum {

ImprovedScaffoldCompiler::ImprovedScaffoldCompiler() {
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

std::shared_ptr<IR> ImprovedScaffoldCompiler::compile(const std::string& src,
		std::shared_ptr<Accelerator> acc) {

	return std::make_shared<GateQIR>();
}

std::shared_ptr<IR> ImprovedScaffoldCompiler::compile(const std::string& src) {

	kernelSource = src;

	if (boost::contains(kernelSource, "__qpu__")) {
		kernelSource.erase(kernelSource.find("__qpu__"), 7);
		kernelSource = std::string("module") + kernelSource;
		std::cout << "\n" << kernelSource << "\n";
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

	consumer = std::make_shared<scaffold::ScaffoldASTConsumer>();//&CI->getASTContext());
	clang::ParseAST(CI->getPreprocessor(), consumer.get(), CI->getASTContext());

	auto qirFunction = consumer->getFunction();

	auto qir = std::make_shared<GateQIR>();

	qir->addKernel(qirFunction);

	return qir;
}

}

}
static xacc::RegisterCompiler<xacc::quantum::ImprovedScaffoldCompiler> X(
		"improvedscaffold");
