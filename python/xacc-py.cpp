#include "XACC.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>

#include "GateInstruction.hpp"
#include "GateFunction.hpp"
#include "GateIR.hpp"

namespace py = pybind11;
using namespace xacc;
using namespace xacc::quantum;

// `boost::variant` as an example -- can be any `std::variant`-like container
namespace pybind11 { namespace detail {
    template <typename... Ts>
    struct type_caster<boost::variant<Ts...>> : variant_caster<boost::variant<Ts...>> {};

    // Specifies the function used to visit the variant -- `apply_visitor` instead of `visit`
    template <>
    struct visit_helper<boost::variant> {
        template <typename... Args>
        static auto call(Args &&...args) -> decltype(boost::apply_visitor(args...)) {
            return boost::apply_visitor(args...);
        }
    };
}} // namespace pybind11::detail

PYBIND11_MODULE(_pyxacc, m) {
    m.doc() = "Python bindings for XACC. XACC provides a plugin infrastructure for "
    		"programming, compiling, and executing quantum kernels in a language and "
    		"hardware agnostic manner.";

    py::class_<xacc::InstructionParameter> (m, "InstructionParameter",
			"The InstructionParameter provides a variant structure "
					"to provide parameters to XACC Instructions at runtime. "
					"This type can be an int, double, float, string, or complex value.")
		.def(py::init<int>(), "Construct as an int.")
		.def(py::init<double>(), "Construct as a double.")
		.def(py::init<std::string>(), "Construct as a string.")
		.def(py::init<std::complex<double>>(), "Construct as a complex double.")
		.def(py::init<float>(), "Construct as a float.");

    py::class_<xacc::Instruction, std::shared_ptr<xacc::Instruction>>(m, "Instruction", "")
    		.def("nParameters", &xacc::Instruction::nParameters, "")
    		.def("toString", &xacc::Instruction::toString, "")
    		.def("bits", &xacc::Instruction::bits, "")
    		.def("getParameter", &xacc::Instruction::getParameter, "")
    		.def("getParameters", &xacc::Instruction::getParameters, "")
    		.def("setParameter", &xacc::Instruction::setParameter, "")
    		.def("mapBits", &xacc::Instruction::mapBits, "")
    		.def("getTag", &xacc::Instruction::getTag, "")
    		.def("name", &xacc::Instruction::name, "")
		.def("description", &xacc::Instruction::description, "");

    py::class_<xacc::Function, std::shared_ptr<xacc::Function>>(m, "Function", "")
    		.def("nInstructions", &xacc::Function::nInstructions, "")
		.def("getInstruction", &xacc::Function::getInstruction, "")
		.def("getInstructions", &xacc::Function::getInstructions, "")
		.def("removeInstruction", &xacc::Function::removeInstruction, "")
		.def("replaceInstruction", &xacc::Function::replaceInstruction, "")
		.def("insertInstruction", &xacc::Function::insertInstruction, "")
		.def("addInstruction", &xacc::Function::addInstruction, "")
		.def("addParameter", &xacc::Function::addParameter, "")
		.def("eval", &xacc::Function::operator(), "")
		.def("name", &xacc::Function::name, "")
		.def("description", &xacc::Function::description, "")
		.def("nParameters", &xacc::Function::nParameters, "")
		.def("toString", &xacc::Function::toString, "")
		.def("getParameter", &xacc::Function::getParameter, "")
		.def("getParameters", &xacc::Function::getParameters, "")
		.def("setParameter", &xacc::Function::setParameter, "")
		.def("getTag", &xacc::Function::getTag, "")
		.def("mapBits", &xacc::Function::mapBits, "");

    // Expose the IR interface
    py::class_<xacc::IR, std::shared_ptr<xacc::IR>> (m, "IR", "The XACC Intermediate Representation, "
    		"serves as a container for XACC Functions.")
    		.def("getKernels", &xacc::IR::getKernels, "Return the kernels in this IR")
			.def("addKernel", &xacc::IR::addKernel, "");

    py::class_<xacc::IRPreprocessor, std::shared_ptr<xacc::IRPreprocessor>> (m, "IRPreprocesor", "").def("process", &xacc::IRPreprocessor::process, "");
    py::class_<xacc::AcceleratorBufferPostprocessor, std::shared_ptr<xacc::AcceleratorBufferPostprocessor>> (m, "AcceleratorBufferPostprocessor", "").def("process", &xacc::AcceleratorBufferPostprocessor::process, "");

    // Expose the Kernel
    py::class_<xacc::Kernel<>, std::shared_ptr<xacc::Kernel<>>>(m, "Kernel", "The XACC Kernel is the "
    		"executable functor that executes XACC IR on the desired Accelerator.")
    		.def("getIRFunction", &xacc::Kernel<>::getIRFunction,
    				py::return_value_policy::reference,
					"Return the IR Function instance this Kernel wraps.")
		.def("execute", (void (xacc::Kernel<>::*)(std::shared_ptr<xacc::AcceleratorBuffer>,
				std::vector<xacc::InstructionParameter>)) &xacc::Kernel<>::operator()
					, "Execute this Kernel with the given set of "
							"InstructionParamters. This set can be empty "
							"if there are no parameters.");

	py::class_<xacc::KernelList<>>(m, "KernelList", "The XACC KernelList is a vector of "
			"Kernels that provides a operator() implementation to execute multiple kernels at once.")
		.def("execute",
			(std::vector<std::shared_ptr<xacc::AcceleratorBuffer>> (xacc::KernelList<>::*)(
					std::shared_ptr<xacc::AcceleratorBuffer>,
					std::vector<xacc::InstructionParameter>)) &xacc::KernelList<>::operator(),
					"Execute a list of Kernels at once.")
		.def("__getitem__", [](const xacc::KernelList<>& kl, int i) -> xacc::Kernel<> {
			if (i >= kl.size()) throw py::index_error();
			return kl[i];
		})
		.def("__setitem__", [](xacc::KernelList<>& kl, size_t i, xacc::Kernel<> v) {
			if (i >= kl.size()) throw py::index_error();
			kl[i] = v;
		})
		.def("__len__", &xacc::KernelList<>::size)
		.def("__iter__", [](const xacc::KernelList<>& kl) {
			return py::make_iterator(kl.begin(), kl.end());
			}, py::keep_alive<0,1>())
		.def("__getitem__",
			[]( xacc::KernelList<>& s, py::slice slice) -> xacc::KernelList<>* {
				size_t start, stop, step, slicelength;
				if (!slice.compute(s.size(), &start, &stop, &step, &slicelength))
				throw py::error_already_set();
				xacc::KernelList<> *seq = new xacc::KernelList<>(s.getAccelerator());
				for (size_t i = 0; i < slicelength; ++i) {
					(*seq).push_back(s[start]);
					start += step;
				}
				return seq;
			});

	// Expose the Accelerator
	py::class_<xacc::Accelerator, std::shared_ptr<xacc::Accelerator>>(m,
			"Accelerator", "Accelerator wraps the XACC C++ Accelerator class "
					"and provides a mechanism for creating buffers of qubits. Execution "
					"is handled by the XACC Kernels.")
		.def("name", &xacc::Accelerator::name, "Return the name of this Accelerator.")
		.def("createBuffer", (std::shared_ptr<xacc::AcceleratorBuffer> (xacc::Accelerator::*)(const std::string&, const int))
					&xacc::Accelerator::createBuffer, py::return_value_policy::reference,
			"Return a newly created register of qubits.")
		.def("execute", (void (xacc::Accelerator::*)(std::shared_ptr<AcceleratorBuffer>, std::shared_ptr<Function>)) &xacc::Accelerator::execute, "Execute the Function with the given AcceleratorBuffer.");

	// Expose the AcceleratorBuffer
	py::class_<xacc::AcceleratorBuffer, std::shared_ptr<xacc::AcceleratorBuffer>>(m,
			"AcceleratorBuffer", "The AcceleratorBuffer models a register of qubits.")
		.def("getExpectationValueZ", &xacc::AcceleratorBuffer::getExpectationValueZ, "Return the expectation value with respect to the Z operator.")
		.def("resetBuffer", &xacc::AcceleratorBuffer::resetBuffer, "Reset this buffer for use in another computation.")
		.def("appendMeasurement", (void (xacc::AcceleratorBuffer::*)(const std::string&)) &xacc::AcceleratorBuffer::appendMeasurement, "Append the measurement string")
		.def("getMeasurementStrings", &xacc::AcceleratorBuffer::getMeasurementStrings, "Return observed measurement bit strings")
		.def("computeMeasurementProbability", &xacc::AcceleratorBuffer::computeMeasurementProbability, "Compute the probability of a given bit string")
		.def("getMeasurementCounts", &xacc::AcceleratorBuffer::getMeasurementCounts, "Return the mapping of measure bit strings to their counts.");

	// Expose the Compiler
	py::class_<xacc::Compiler, std::shared_ptr<xacc::Compiler>>(m,
			"Compiler", "The XACC Compiler takes as input quantum kernel source code, "
					"and compiles it to the XACC intermediate representation.")
		.def("name", &xacc::Compiler::name, "Return the name of this Compiler.")
		.def("compile", (std::shared_ptr<xacc::IR> (xacc::Compiler::*)(const std::string&, std::shared_ptr<xacc::Accelerator>)) &xacc::Compiler::compile, "Compile the "
			"given source code against the given Accelerator.")
		.def("translate", &xacc::Compiler::translate, "Translate the given IR Function instance to source code in this Compiler's language.");

	// Expose the Program object
	py::class_<xacc::Program>(m,
			"Program", "The Program is the primary entrypoint for compilation and execution in XACC. Clients provide quantum kernel source "
					"code and the Accelerator instance, and the Program handles compiling the code and provides Kernel instances to execute.")
		.def(py::init<std::shared_ptr<xacc::Accelerator>, const std::string &>(), "The constructor")
		.def(py::init<std::shared_ptr<xacc::Accelerator>, std::shared_ptr<xacc::IR>>(), "The constructor")
		.def("build", &xacc::Program::build, "Compile this program.")
		.def("getKernel", (xacc::Kernel<> (xacc::Program::*)(const std::string&)) &xacc::Program::getKernel<>,
				py::return_value_policy::reference, "Return a Kernel representing the source code.")
    		.def("getKernels", &xacc::Program::getRuntimeKernels, "Return all Kernels.")
		.def("nKernels", &xacc::Program::nKernels, "Return the number of kernels compiled by this program");

    // Expose XACC API functions
	m.def("Initialize", (void (*)(std::vector<std::string>))
		&xacc::Initialize,
			"Initialize the framework. Can provide a list of strings to model command line arguments. For instance, "
			"to print the XACC help message, pass ['--help'] to this function, or to set the compiler to use, ['--compiler','scaffold'].");
	m.def("Initialize", (void (*)()) &xacc::Initialize,
			"Initialize the framework. Use this if there are no command line arguments to pass.");
	m.def("getAccelerator", (std::shared_ptr<xacc::Accelerator> (*)(const std::string&))
			&xacc::getAccelerator, py::return_value_policy::reference,
			"Return the accelerator with given name.");
	m.def("getCompiler", (std::shared_ptr<xacc::Compiler> (*)(const std::string&))
			&xacc::getCompiler, py::return_value_policy::reference,
			"Return the Compiler of given name.");
	m.def("getIRPreprocessor", (std::shared_ptr<xacc::IRPreprocessor> (*)(const std::string&))
			&xacc::getService<IRPreprocessor>, py::return_value_policy::reference,
			"Return the Compiler of given name.");
	m.def("setOption", &xacc::setOption, "Set an XACC framework option.");
	m.def("getOption", &xacc::getOption, "Get an XACC framework option.");
	m.def("optionExists", &xacc::optionExists, "Set an XACC framework option.");
	m.def("Finalize", &xacc::Finalize, "Finalize the framework");

	m.def("compileKernel", [](std::shared_ptr<Accelerator> acc, const std::string& src, const std::string& compilerName = "") -> std::shared_ptr<Function>{
		    if (!compilerName.empty()) xacc::setOption("compiler", compilerName);
			xacc::Program p(acc,src);
			p.build();
			return p.getRuntimeKernels()[0].getIRFunction();
	}, py::arg("acc"), py::arg("src"), py::arg("compilerName") = std::string(""), py::return_value_policy::move, "");
	
	m.def("functionToInstruction", [](std::shared_ptr<Function> f) {return std::dynamic_pointer_cast<Instruction>(f);}, py::return_value_policy::copy);

	py::module gatesub = m.def_submodule("gate", "Gate model quantum computing data structures.");
	gatesub.def("create",
			[](const std::string& name, std::vector<int> qbits, std::vector<InstructionParameter> params = std::vector<InstructionParameter> {}) -> std::shared_ptr<Instruction> {
				return xacc::getService<IRProvider>("gate")->createInstruction(name, qbits, params);
			}, "Convenience function for creating a new GateInstruction.",
			py::arg("name"), py::arg("qbits"),
			py::arg("params") = std::vector<InstructionParameter> { });
	gatesub.def("createFunction",
			[](const std::string& name, std::vector<int> qbits, std::vector<InstructionParameter> params = std::vector<InstructionParameter> {}) -> std::shared_ptr<Function> {
				return xacc::getService<IRProvider>("gate")->createFunction(name, qbits, params);
			}, "Convenience function for creating a new GateFunction.",
			py::arg("name"), py::arg("qbits"),
			py::arg("params") = std::vector<InstructionParameter> { });
	gatesub.def("createIR",
			[]() -> std::shared_ptr<IR> {
				return xacc::getService<IRProvider>("gate")->createIR();
			}, "Convenience function for creating a new GateIR.");

}


