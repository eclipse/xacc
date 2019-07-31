#include "XACC.hpp"
#include "Optimizer.hpp"
#include "PauliOperator.hpp"
#include "xacc_service.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  // specified by --accelerator argument
  auto accelerator = xacc::getAccelerator();

  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 =
      std::make_shared<xacc::quantum::PauliOperator>("5.907 - 2.1433 X0X1 "
                                                     "- 2.1433 Y0Y1"
                                                     "+ .21829 Z0 - 6.125 Z1");

  auto optimizer = xacc::getService<xacc::Optimizer>("nlopt");

  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler quil
.function deuteron_ansatz
X 0
Ry(theta) 1
CNOT 1 0
)");
  auto ansatz = xacc::getCompiled("deuteron_ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({{"ansatz", ansatz},
                   {"observable", H_N_2},
                   {"accelerator", accelerator},
                   {"optimizer", optimizer}});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  vqe->execute(buffer);

  // Print the result
  std::cout << "Energy: "
            << buffer->getInformation("opt-val").as<double>() << "\n";
}