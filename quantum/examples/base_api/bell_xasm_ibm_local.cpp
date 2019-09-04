#include "xacc.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto accelerator =
      xacc::getAccelerator("local-ibm", {std::make_pair("shots", 5000),
                                         std::make_pair("cx-p-depol", .3)});

  // Allocate some qubits
  auto buffer = xacc::qalloc(2);

  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
})", accelerator);

  accelerator->execute(buffer, ir->getComposites()[0]);

  buffer->print();

  xacc::Finalize();

  return 0;
}