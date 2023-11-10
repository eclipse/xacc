
#include "xacc.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto accelerator = xacc::getAccelerator("ionq", {{"shots", 1024}});

  // Allocate some qubits
  auto buffer = xacc::qalloc(2);

  xacc::qasm(R"(
.compiler xasm
.circuit ansatz
.qbit q
H(q[0]);
CX(q[0],q[1]);
Measure(q[0]);
Measure(q[1]);
)");
  auto ansatz = xacc::getCompiled("ansatz");

  accelerator->execute(buffer, ansatz);

  xacc::Finalize();

  return 0;
}
