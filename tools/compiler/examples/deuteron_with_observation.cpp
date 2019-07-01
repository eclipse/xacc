#include "xacc_runtime.hpp"
#include <iostream>

__qpu__  void ansatz(qbit &q, double t0) {
  X(0);
  Ry(t0, 1);
  CX(1, 0);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  auto hamiltonian = xacc::getObservable("pauli", "5.907 - 2.1433 X0X1 "
                                                  "- 2.1433 Y0Y1"
                                                  "+ .21829 Z0 - 6.125 Z1");

  for (auto &t : xacc::linspace(-3.14, 3.14, 40)) {

    // Allocate a register of 2 qubits
    qbit q = xacc::qalloc(2);

    #pragma xacc observe hamiltonian
    ansatz(q, t);

    auto energy = q->getInformation("observation");
    std::cout << "E(" << t << ") = " << mpark::get<double>(energy) << "\n";
  }

  xacc::Finalize();

  return 0;
}