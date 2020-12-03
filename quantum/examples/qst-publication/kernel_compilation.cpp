#include "xacc.hpp"
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto accelerator =
     xacc::getAccelerator("qpp");

  // Allocate some qubits
  auto buffer = xacc::qalloc(2);

  // Compile
  auto quil = xacc::getCompiler("quil");
  auto ir = quil->compile(
    R"(__qpu__ void bell(qbit q) {
H 0
CX 0 1
MEASURE 0 [0]
MEASURE 1 [1]
})", accelerator);

  // Execute
  accelerator->execute(buffer,
            ir->getComposites()[0]);

  // View results
  buffer->print();

  xacc::Finalize();
  return 0;
}
