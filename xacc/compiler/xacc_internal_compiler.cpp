#include "xacc_internal_compiler.hpp"
#include "xacc.hpp"

namespace xacc {
namespace internal_compiler {

void compiler_InitializeXACC() {
  if (!xacc::isInitialized())
    xacc::Initialize();
}

// Map kernel source string representing a single
// kernel function to a single CompositeInstruction (src to IR)
CompositeInstruction *compile(const char *compiler_name,
                                    const char *kernel_src,
                                    const char *qpu_name) {
  auto qpu = xacc::getAccelerator(qpu_name);
  auto compiler = xacc::getCompiler(compiler_name);
  auto IR = compiler->compile(kernel_src, qpu);
  auto program = IR->getComposites()[0];
  return program.get();
}

CompositeInstruction * getCompiled(const char * kernel_name) {
  return xacc::hasCompiled(kernel_name) ? xacc::getCompiled(kernel_name).get() : nullptr;
}

// Run quantum compilation routines on IR
void optimize(CompositeInstruction *program, const char *qpu_name,
              const OptLevel opt) {

  auto qpu = xacc::getAccelerator(qpu_name);
  auto as_shared = std::shared_ptr<CompositeInstruction>(
      program, empty_delete<CompositeInstruction>());
  if (opt == DEFAULT) {
    auto optimizer = xacc::getIRTransformation("circuit-optimizer");
    optimizer->apply(as_shared, qpu);
  } else {
    xacc::error("Other Optimization Levels not yet supported.");
  }
}

// Execute on the specified QPU, persisting results to
// the provided buffer.
void execute(AcceleratorBuffer * buffer, const char *qpu_name,
             CompositeInstruction * program, double* parameters) {
//   std::cout << "Executing:\n" << program->toString() << "\n";
  std::shared_ptr<CompositeInstruction> program_as_shared;
  if (parameters) {
      std::vector<double> values(parameters, parameters + program->nVariables());
      program_as_shared = program->operator()(values);
  } else {
    program_as_shared = std::shared_ptr<CompositeInstruction>(
      program, empty_delete<CompositeInstruction>());
  }
  auto qpu = xacc::getAccelerator(qpu_name);
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, empty_delete<AcceleratorBuffer>());
  qpu->execute(buffer_as_shared, program_as_shared);
}
} // namespace internal_compiler
} // namespace xacc