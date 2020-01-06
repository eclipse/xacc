#include "xacc.hpp"
#include "xacc_observable.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get the desired Accelerator and Optimizer
  auto qpu =
    xacc::getAccelerator("qpp");
  auto optimizer = xacc::getOptimizer("nlopt");

  // Create the N=3 deuteron Hamiltonian
  auto H_N_3 = xacc::quantum::getObservable(
     "pauli",
     std::string("15.531709 - 2.1433 X0X1 - "
        "2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 - "
        "9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
    .compiler xasm
    .circuit ansatz
    .parameters t0, t1
    .qbit q
    X(q[0]);
    exp_i_theta(q, t0,
        {{"pauli", "X0 Y1 - Y0 X1"}});
    exp_i_theta(q, t1,
        {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
    )");
  auto ansatz = xacc::getCompiled("ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({
        std::make_pair("ansatz", ansatz),
        std::make_pair("observable", H_N_3),
        std::make_pair("accelerator", qpu),
        std::make_pair("optimizer", optimizer)
        });

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(3);
  vqe->execute(buffer);

  // Get the result
  auto energy =
        (*buffer)["opt-val"].as<double>();
  std::cout << "Energy: " << energy << "\n";
  xacc::Finalize();
  return 0;
}