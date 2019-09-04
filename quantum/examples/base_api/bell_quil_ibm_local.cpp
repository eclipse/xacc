#include "xacc.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto accelerator = xacc::getAccelerator("local-ibm");

  // Allocate some qubits
  auto buffer = xacc::qalloc(2);

  auto quilCompiler = xacc::getCompiler("quil");
  auto ir = quilCompiler->compile(R"(__qpu__ void bell(qbit q) {
H 0
CX 0 1
MEASURE 0 [0]
MEASURE 1 [1]
})", accelerator);

  accelerator->execute(buffer, ir->getComposites()[0]);

  buffer->print();

  xacc::Finalize();

  return 0;
}