#include "ScaffoldCompiler.hpp"
#include "GraphIR.hpp"
#include <regex>
#include "ScaffCCAPI.hpp"

namespace xacc {

namespace quantum {

void ScaffoldCompiler::modifySource() {

	// Here we assume we've been given just
	// the body of the quantum code, as part
	// of an xacc __qpu__ kernel function.

	// First off, replace __qpu__ with 'module '
	kernelSource.erase(kernelSource.find("__qpu__"), 7);
	kernelSource = std::string("module ") + kernelSource;

	// Get the kernel name
	std::regex functionName("((\\w+)\\s*\\()\\s*");
	auto begin = std::sregex_iterator(kernelSource.begin(), kernelSource.end(),
			functionName);
	std::string fName = (*begin).str();

	// Now wrap in a main function for ScaffCC
	kernelSource = kernelSource + std::string("\nint main() {\n   ") + fName
			+ std::string(");\n}");

	std::cout << "\n" << kernelSource << "\n";
}

std::shared_ptr<IR> ScaffoldCompiler::compile() {

	// Create an instance of our ScaffCC API
	// so that we can interact with a locally installed
	// scaffcc executable
	scaffold::ScaffCCAPI scaffcc;

	// Compile the source code and return the QASM form
	// This will throw if it fails.
	auto qasm = scaffcc.getFlatQASMFromSource(kernelSource);

	// Generate a GraphIR instance, ie a graph
	// tensor references making up this QASM
	std::cout << "Flat QASM: \n" << qasm << "\n";
	return std::make_shared<GraphIR>();
}

} // end namespace quantum

} // end namespace xacc

// Required in CPP file to be discovered by factory pattern
REGISTER_QCIOBJECT_WITH_QCITYPE(xacc::quantum::ScaffoldCompiler, "compiler",
		"scaffold");

