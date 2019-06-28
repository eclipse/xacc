#include "xacc_runtime.hpp"

__qpu__ void bell(qbit &q) {
  H(0);
  CX(0, 1);
  Measure(0);
  Measure(1);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Allocate a register of 2 qubits
  qbit q = xacc::qalloc(2);

  // Execute the bell state computation
  bell(q);

  // Print the results
  q->print();

  xacc::Finalize();

  return 0;
}