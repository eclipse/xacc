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
             CompositeInstruction * program) {
  std::cout << "Executing:\n" << program->toString() << "\n";
  auto qpu = xacc::getAccelerator(qpu_name);
  auto program_as_shared = std::shared_ptr<CompositeInstruction>(
      program, empty_delete<CompositeInstruction>());
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, empty_delete<AcceleratorBuffer>());
  qpu->execute(buffer_as_shared, program_as_shared);
}
} // namespace internal_compiler
} // namespace xacc