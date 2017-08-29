#include "XACC.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "GateInstruction.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pyxacc, m) {
    m.doc() = "Python bindings for XACC. XACC provides a plugin infrastructure for "
    		"programming, compiling, and executing quantum kernels in a language and "
    		"hardware agnostic manner.";

	// Expose the InstructionParameter
	py::class_<xacc::InstructionParameter> ip(m, "InstructionParameter",
			"The InstructionParameter provides a variant structure "
					"to provide parameters to XACC Instructions at runtime. "
					"This type can be an int, double, float, string, or complex value.");
	ip.def(py::init<int>(), "Construct as an int.");
    ip.def(py::init<double>(), "Construct as a double.");
    ip.def(py::init<std::string>(), "Construct as a string.");
    ip.def(py::init<std::complex<double>>(), "Construt as a complex double.");
    ip.def(py::init<float>(), "Construct as a float.");

    // Expose the Instruction interface
    py::class_<xacc::Instruction, std::shared_ptr<xacc::Instruction>> inst(m, "Instruction", "Instruction wraps the XACC C++ Instruction class -"
    		" the base for all XACC intermediate representation instructions. Instructions, for example, can be common gates like Hadamard or CNOT.");
    inst.def("getName", &xacc::Instruction::getName, "Return the name of this Instruction.");

    // Expose the Function interface
    py::class_<xacc::Function, std::shared_ptr<xacc::Function>> f(m, "Function", "Functions are composed of Instructions.");
    f.def("addInstruction", &xacc::Function::addInstruction, "Add an Instruction to this Function.");
    f.def("nInstructions", &xacc::Function::nInstructions, "Return the number of Instructions in this Function.");
    f.def("getInstruction", &xacc::Function::getInstruction, "Return the instruction at the provided index.");

    // Expose the IR interface
    py::class_<xacc::IR, std::shared_ptr<xacc::IR>> ir(m, "IR", "The XACC Intermediate Representation, "
    		"serves as a container for XACC Functions.");

    // Expose the Kernel
    py::class_<xacc::Kernel<>, std::shared_ptr<xacc::Kernel<>>> k(m, "Kernel", "The XACC Kernel is the "
    		"executable functor that executes XACC IR on the desired Accelerator.");
    k.def("getIRFunction", &xacc::Kernel<>::getIRFunction, "Return the IR Function instance this Kernel wraps.");
	k.def("execute",
			py::overload_cast<std::shared_ptr<xacc::AcceleratorBuffer>,
					std::vector<xacc::InstructionParameter>>(
					&xacc::Kernel<>::operator()), "Execute this Kernel with the given set of "
							"InstructionParamters. This set can be empty if there are no parameters.");

	// Expose the Accelerator
	py::class_<xacc::Accelerator, std::shared_ptr<xacc::Accelerator>> acc(m,
			"Accelerator", "Accelerator wraps the XACC C++ Accelerator class "
					"and provides a mechanism for creating buffers of qubits. Execution "
					"is handled by the XACC Kernels.");
	acc.def("name", &xacc::Accelerator::name, "Return the name of this Accelerator.");
	acc.def("createBuffer",
			py::overload_cast<const std::string&, const int>(
					&xacc::Accelerator::createBuffer),
			"Return a newly created register of qubits.");

	// Expose the AcceleratorBuffer
	py::class_<xacc::AcceleratorBuffer, std::shared_ptr<xacc::AcceleratorBuffer>> accb(m,
			"AcceleratorBuffer", "The AcceleratorBuffer models a register of qubits.");
	accb.def("printBuffer", py::overload_cast<>(&xacc::AcceleratorBuffer::print), "Print the AcceleratorBuffer to standard out.");
	accb.def("getExpectationValueZ", &xacc::AcceleratorBuffer::getExpectationValueZ, "Return the expectation value with respect to the Z operator.");
	accb.def("resetBuffer", &xacc::AcceleratorBuffer::resetBuffer, "Reset this buffer for use in another computation.");

	// Expose the Compiler
	py::class_<xacc::Compiler, std::shared_ptr<xacc::Compiler>> compiler(m,
			"Compiler", "The XACC Compiler takes as input quantum kernel source code, "
					"and compiles it to the XACC intermediate representation.");
	compiler.def("name", &xacc::Compiler::name, "Return the name of this Compiler.");
	compiler.def("compile", py::overload_cast<const std::string&, std::shared_ptr<xacc::Accelerator>>(&xacc::Compiler::compile), "Compile the "
			"given source code against the given Accelerator.");
	compiler.def("translate", &xacc::Compiler::translate, "Translate the given IR Function instance to source code in this Compiler's language.");

	// Expose the Program object
	py::class_<xacc::Program> program(m,
			"Program", "The Program is the primary entrypoint for compilation and execution in XACC. Clients provide quantum kernel source "
					"code and the Accelerator instance, and the Program handles compiling the code and provides Kernel instances to execute.");
	program.def(py::init<std::shared_ptr<xacc::Accelerator>, const std::string &>(), "The constructor");
	program.def("build", &xacc::Program::build, "Compile this program.");
	program.def("getKernel", py::overload_cast<const std::string&>(&xacc::Program::getKernel<>), "Return a Kernel representing the source code.");
    program.def("getKernels", &xacc::Program::getRuntimeKernels, "Return all Kernels.");

    // Expose XACC API functions
	m.def("Initialize",
			py::overload_cast<std::vector<std::string>>(&xacc::Initialize),
			"Initialize the framework. Can provide a list of strings to model command line arguments. For instance, "
			"to print the XACC help message, pass ['--help'] to this function, or to set the compiler to use, ['--compiler','scaffold'].");
	m.def("Initialize",
			py::overload_cast<>(&xacc::Initialize),
			"Initialize the framework. Use this if there are no command line arguments to pass.");
	m.def("getAccelerator",
			py::overload_cast<const std::string&>(&xacc::getAccelerator),
			"Return the accelerator with given name.");
	m.def("getCompiler",
			py::overload_cast<const std::string&>(&xacc::getCompiler),
			"Return the Compiler of given name.");
	m.def("setOption", &xacc::setOption, "Set an XACC framework option.");
	m.def("getOption", &xacc::getOption, "Get an XACC framework option.");
	m.def("optionExists", &xacc::optionExists, "Set an XACC framework option.");
	m.def("Finalize", &xacc::Finalize, "Finalize the framework");

	py::module gatesub = m.def_submodule("gate", "Gate model quantum computing data structures.");

	py::class_<xacc::quantum::GateInstruction,
			std::shared_ptr<xacc::quantum::GateInstruction>> gateinst(gatesub,
			"GateInstruction",
			"GateInstruction models gate model quantum computing instructions.");
	gateinst.def("getName", &xacc::quantum::GateInstruction::getName, "Return the name of this Instruction.");


	py::class_<xacc::quantum::GateInstructionRegistry, std::shared_ptr<xacc::quantum::GateInstructionRegistry>> gatereg(gatesub,
				"GateInstructionRegistry", "Registry of available quantum gates.");
	gatereg.def_static("instance", &xacc::quantum::GateInstructionRegistry::instance, "Singleton instance method.");
	gatereg.def("create", &xacc::quantum::GateInstructionRegistry::create, "Create");

}


