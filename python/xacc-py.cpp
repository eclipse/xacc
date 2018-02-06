#include "XACC.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>

#include "GateInstruction.hpp"
#include "GateFunction.hpp"
#include "GateQIR.hpp"

namespace py = pybind11;
using namespace xacc;
using namespace xacc::quantum;

std::shared_ptr<GateInstruction> create(const std::string& name, std::vector<int> qbits, std::vector<InstructionParameter> params = std::vector<InstructionParameter>{}) {
	auto g = GateInstructionRegistry::instance()->create(name, qbits);
	int idx = 0;
	for (auto& a : params) {
		g->setParameter(idx, a);
		idx++;
	}
	return g;
}

void setCredentials(const std::string& accelerator, const std::string& apiKey, const std::string& url = "", const std::string& userId = "") {

	std::string filename = std::string(std::getenv("HOME")) + "/";
	std::stringstream s;
	if (accelerator == "ibm") {
		filename += ".ibm_config";
		std::string tmpUrl = url;
		if (url.empty()) {
			tmpUrl = "https://quantumexperience.ng.bluemix.net";
		}

		s << "url: " << tmpUrl << "\n";
		s << "key: " << apiKey << "\n";

	} else if (accelerator == "rigetti") {
		filename += ".pyquil_config";
		if (userId.empty()) {
			xacc::error("You must provide your Rigetti QVM User Id.");
		}
		std::string tmpUrl = url;
		if (url.empty()) {
			tmpUrl = "https://api.rigetti.com/qvm";
		}

		s << "url: " << tmpUrl << "\n";
		s << "key: " << apiKey << "\n";
		s << "user_id: " << userId << "\n";

	} else if (accelerator == "dwave") {
		filename += ".dwave_config";
		std::string tmpUrl = url;
		if (url.empty()) {
			tmpUrl = "https://qubist.dwavesys.com";
		}

		s << "url: " << tmpUrl << "\n";
		s << "key: " << apiKey << "\n";
	}

	if (!s.str().empty()) {
		std::ofstream out(filename);
		out << s.str();
		out.close();
	}

	return;
}

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
    py::class_<xacc::Function, xacc::Instruction, std::shared_ptr<xacc::Function>> f(m, "Function", "Functions are composed of Instructions.");
    f.def("addInstruction", &xacc::Function::addInstruction, "Add an Instruction to this Function.");
    f.def("nInstructions", &xacc::Function::nInstructions, "Return the number of Instructions in this Function.");
    f.def("getInstruction", &xacc::Function::getInstruction, "Return the instruction at the provided index.");

    // Expose the IR interface
    py::class_<xacc::IR, std::shared_ptr<xacc::IR>> ir(m, "IR", "The XACC Intermediate Representation, "
    		"serves as a container for XACC Functions.");
    ir.def("getKernels", &xacc::IR::getKernels, "Return the kernels in this IR");

    // Expose the Kernel
    py::class_<xacc::Kernel<>, std::shared_ptr<xacc::Kernel<>>> k(m, "Kernel", "The XACC Kernel is the "
    		"executable functor that executes XACC IR on the desired Accelerator.");
    k.def("getIRFunction", &xacc::Kernel<>::getIRFunction, "Return the IR Function instance this Kernel wraps.");
	k.def("execute", (void (xacc::Kernel<>::*)(std::shared_ptr<xacc::AcceleratorBuffer>, std::vector<xacc::InstructionParameter>)) &xacc::Kernel<>::operator()
					, "Execute this Kernel with the given set of "
							"InstructionParamters. This set can be empty if there are no parameters.");

	py::class_<xacc::KernelList<>> klist(m, "KernelList", "The XACC KernelList is a vector of Kernels that provides a operator() implementation to execute multiple kernels at once.");
	klist.def("execute",
			(std::vector<std::shared_ptr<xacc::AcceleratorBuffer>> (xacc::KernelList<>::*)(
					std::shared_ptr<xacc::AcceleratorBuffer>,
					std::vector<xacc::InstructionParameter>)) &xacc::KernelList<>::operator(), "Execute a list of Kernels at once.");
	klist.def("__getitem__", [](const xacc::KernelList<>& kl, int i) -> xacc::Kernel<> {
		if (i >= kl.size()) throw py::index_error();
		return kl[i];
	});
	klist.def("__setitem__", [](xacc::KernelList<>& kl, size_t i, xacc::Kernel<> v) {
		if (i >= kl.size()) throw py::index_error();
		kl[i] = v;
	});
	klist.def("__len__", &xacc::KernelList<>::size);
	klist.def("__iter__", [](const xacc::KernelList<>& kl) {return py::make_iterator(kl.begin(), kl.end());}, py::keep_alive<0,1>());
	klist.def("__getitem__",
			[]( xacc::KernelList<>& s, py::slice slice) -> xacc::KernelList<>* {
				size_t start, stop, step, slicelength;
				if (!slice.compute(s.size(), &start, &stop, &step, &slicelength))
				throw py::error_already_set();
				xacc::KernelList<> *seq = new xacc::KernelList<>(s.getAccelerator());
				for (size_t i = 0; i < slicelength; ++i) {
					(*seq).push_back(s[start]);
					start += step;
//					(*seq)[i] = s[start]; start += step;
				}
				return seq;
			});
	// Expose the Accelerator
	py::class_<xacc::Accelerator, std::shared_ptr<xacc::Accelerator>> acc(m,
			"Accelerator", "Accelerator wraps the XACC C++ Accelerator class "
					"and provides a mechanism for creating buffers of qubits. Execution "
					"is handled by the XACC Kernels.");
	acc.def("name", &xacc::Accelerator::name, "Return the name of this Accelerator.");
	acc.def("createBuffer", (std::shared_ptr<xacc::AcceleratorBuffer> (xacc::Accelerator::*)(const std::string&, const int)) 
			//py::overload_cast<const std::string&, const int>(
					&xacc::Accelerator::createBuffer,
			"Return a newly created register of qubits.");

	// Expose the AcceleratorBuffer
	py::class_<xacc::AcceleratorBuffer, std::shared_ptr<xacc::AcceleratorBuffer>> accb(m,
			"AcceleratorBuffer", "The AcceleratorBuffer models a register of qubits.");
	accb.def("printBuffer", (void (xacc::AcceleratorBuffer::*)()) &xacc::AcceleratorBuffer::print, "Print the AcceleratorBuffer to standard out.");
	accb.def("getExpectationValueZ", &xacc::AcceleratorBuffer::getExpectationValueZ, "Return the expectation value with respect to the Z operator.");
	accb.def("resetBuffer", &xacc::AcceleratorBuffer::resetBuffer, "Reset this buffer for use in another computation.");
	accb.def("getMeasurementStrings", &xacc::AcceleratorBuffer::getMeasurementStrings, "");
	accb.def("computeMeasurementProbability", &xacc::AcceleratorBuffer::computeMeasurementProbability, "");

	// Expose the Compiler
	py::class_<xacc::Compiler, std::shared_ptr<xacc::Compiler>> compiler(m,
			"Compiler", "The XACC Compiler takes as input quantum kernel source code, "
					"and compiles it to the XACC intermediate representation.");
	compiler.def("name", &xacc::Compiler::name, "Return the name of this Compiler.");
	compiler.def("compile", (std::shared_ptr<xacc::IR> (xacc::Compiler::*)(const std::string&, std::shared_ptr<xacc::Accelerator>)) &xacc::Compiler::compile, "Compile the "
			"given source code against the given Accelerator.");
	compiler.def("translate", &xacc::Compiler::translate, "Translate the given IR Function instance to source code in this Compiler's language.");

	// Expose the Program object
	py::class_<xacc::Program> program(m,
			"Program", "The Program is the primary entrypoint for compilation and execution in XACC. Clients provide quantum kernel source "
					"code and the Accelerator instance, and the Program handles compiling the code and provides Kernel instances to execute.");
	program.def(py::init<std::shared_ptr<xacc::Accelerator>, const std::string &>(), "The constructor");
	program.def(py::init<std::shared_ptr<xacc::Accelerator>, std::shared_ptr<xacc::IR>>(), "The constructor");
	program.def("build", &xacc::Program::build, "Compile this program.");
	program.def("getKernel", (xacc::Kernel<> (xacc::Program::*)(const std::string&)) &xacc::Program::getKernel<>, "Return a Kernel representing the source code.");
    program.def("getKernels", &xacc::Program::getRuntimeKernels, "Return all Kernels.");
	program.def("nKernels", &xacc::Program::nKernels, "Return the number of kernels compiled by this program");

    // Expose XACC API functions
	m.def("Initialize", (void (*)(std::vector<std::string>))
		&xacc::Initialize,
			"Initialize the framework. Can provide a list of strings to model command line arguments. For instance, "
			"to print the XACC help message, pass ['--help'] to this function, or to set the compiler to use, ['--compiler','scaffold'].");
	m.def("Initialize", (void (*)()) &xacc::Initialize,
			"Initialize the framework. Use this if there are no command line arguments to pass.");
	m.def("getAccelerator", (std::shared_ptr<xacc::Accelerator> (*)(const std::string&))
			&xacc::getAccelerator,
			"Return the accelerator with given name.");
	m.def("getCompiler", (std::shared_ptr<xacc::Compiler> (*)(const std::string&))
			&xacc::getCompiler,
			"Return the Compiler of given name.");
	m.def("setOption", &xacc::setOption, "Set an XACC framework option.");
	m.def("getOption", &xacc::getOption, "Get an XACC framework option.");
	m.def("optionExists", &xacc::optionExists, "Set an XACC framework option.");
	m.def("translate", &xacc::translate, "Translate one language quantum kernel to another");
	m.def("translateWithVisitor", &xacc::translateWithVisitor, "Translate one language quantum kernel to another");
	m.def("Finalize", &xacc::Finalize, "Finalize the framework");

	py::module gatesub = m.def_submodule("gate", "Gate model quantum computing data structures.");

	py::class_<xacc::quantum::GateInstruction, xacc::Instruction,
			std::shared_ptr<xacc::quantum::GateInstruction>> gateinst(gatesub,
			"GateInstruction",
			"GateInstruction models gate model quantum computing instructions.");
	gateinst.def("getName", &xacc::quantum::GateInstruction::getName, "Return the name of this Instruction.");
	gateinst.def("bits", &xacc::quantum::GateInstruction::bits, "Return the qubits this gate operates on");
	gateinst.def("nParameters", &xacc::quantum::GateInstruction::nParameters, "Return the number of parameters this gate requires.");
	gateinst.def("getParameter", &xacc::quantum::GateInstruction::getParameter, "Return the parameter at the given index");
	gateinst.def("setParameter", &xacc::quantum::GateInstruction::setParameter, "Set the parameter value at the give index");
	gateinst.def("toString", &xacc::quantum::GateInstruction::toString, "Return the instruction as a string representation.");

//	py::class_<xacc::quantum::GateInstructionRegistry, std::shared_ptr<xacc::quantum::GateInstructionRegistry>> gatereg(gatesub,
//				"GateInstructionRegistry", "Registry of available quantum gates.");
//	gatereg.def_static("instance", &xacc::quantum::GateInstructionRegistry::instance, "Singleton instance method.");
//	gatereg.def("create", &xacc::quantum::GateInstructionRegistry::create, "Create");

	py::class_<xacc::quantum::GateFunction, xacc::Function, std::shared_ptr<xacc::quantum::GateFunction>> gatefunction(gatesub, 
			"GateFunction",
			"GateFunction description, fill this in later");
	gatefunction.def(py::init<const std::string&>(), "The constructor");
	gatefunction.def(py::init<const std::string&, std::vector<xacc::InstructionParameter>>(), "The constructor");
	gatefunction.def("getName", &xacc::quantum::GateFunction::getName, "Return the name of this Instruction.");
	gatefunction.def("add", &xacc::quantum::GateFunction::addInstruction, "Add an Instruction to this function.");
	gatefunction.def("toString", &xacc::quantum::GateFunction::toString, "Return the function as a string representation.");

	py::class_<xacc::quantum::GateQIR, xacc::IR, std::shared_ptr<xacc::quantum::GateQIR>> gateqir(gatesub, 
			"GateQIR",
			"GateQIR description, fill this in later");
	gateqir.def(py::init<>(), "The constructor");
	gateqir.def("addKernel", &xacc::quantum::GateQIR::addKernel, "Add an Kernel to this GateQIR.");

	gatesub.def("create", &create, "", py::arg("name"), py::arg("qbits"),
			py::arg("params") = std::vector<InstructionParameter> { });

	m.def("setCredentials", &setCredentials,
			"This function sets the users credentials for the given accelerator. URL and UserId are optional parameters.",
			py::arg("accelerator"), py::arg("apiKey"), py::arg("url") =
					std::string(""), py::arg("userId") = std::string(""));

}


