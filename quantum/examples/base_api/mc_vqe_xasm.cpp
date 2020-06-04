#include "xacc.hpp"
#include "xacc_observable.hpp"
#include <chrono>

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);
  // Get the desired Accelerator and Optimizer
  auto optimizer = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-maxeval", 1)});

  // Create the N=3 deuteron Hamiltonian
  auto H = xacc::quantum::getObservable(
     "pauli", std::string("(-0.004787,0) X1 + (-0.035825,0) Z1 + (0.004509,0) + "
     "(-0.003032,0) Z0 X1 + (-0.013232,0) X0 X1 + (-0.00082,0) Z0 Z1 + "
     "(-0.003612,0) X0 Z1 + (-0.03604,0) Z0 + (0.002149,0) X0"));

  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
    .compiler xasm
    .circuit ansatz
    .parameters t0, t1, t2, t3, t4, t5, t6, t7, t8, t9
    .qbit q
    Ry(q[0], 0.0860755);
    Ry(q[1], -0.789979);
    H(q[1]);
    CX(q[0], q[1]);
    H(q[1]);
    Ry(q[1], 0.789979);
    CX(q[1], q[0]);
    Ry(q[0], t0);
    Ry(q[1], t1);
    CX(q[0], q[1]);
    Ry(q[0], t2);
    Ry(q[1], t3);
    CX(q[0], q[1]);
    Ry(q[0], t4);
    Ry(q[1], t5);
    CX(q[1], q[0]);
    Ry(q[1], t6);
    Ry(q[0], t7);
    CX(q[1], q[0]);
    Ry(q[1], t8);
    Ry(q[0], t9);
    )");
  auto ansatz = xacc::getCompiled("ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe1 = xacc::getAlgorithm("vqe");
  auto itensor = xacc::getAccelerator("tnqvm", {std::make_pair("tnqvm-visitor", "itensor-mps")});
  vqe1->initialize({
        std::make_pair("ansatz", ansatz),
        std::make_pair("observable", H),
        std::make_pair("accelerator", itensor),
        std::make_pair("optimizer", optimizer)
        });

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  auto itensorStart = std::chrono::high_resolution_clock::now();
  vqe1->execute(buffer);
  auto itensorEnd = std::chrono::high_resolution_clock::now();

  auto vqe2 = xacc::getAlgorithm("vqe");
  auto exatn = xacc::getAccelerator("tnqvm", {std::make_pair("tnqvm-visitor", "exatn")});
  vqe2->initialize({
        std::make_pair("ansatz", ansatz),
        std::make_pair("observable", H),
        std::make_pair("accelerator", exatn),
        std::make_pair("optimizer", optimizer)
        });

  // Allocate some qubits and execute
  //auto buffer = xacc::qalloc(2);
  auto exatnStart = std::chrono::high_resolution_clock::now();
  vqe2->execute(buffer);
  auto exatnEnd = std::chrono::high_resolution_clock::now();

  std::cout << "ITensor runtime: " << std::chrono::duration_cast<std::chrono::duration<double>>(itensorEnd - itensorStart).count() << "\n";
  std::cout << "ExaTN runtime: " << std::chrono::duration_cast<std::chrono::duration<double>>(exatnEnd - exatnStart).count() << "\n";
  // Get the result
  auto energy =
        (*buffer)["opt-val"].as<double>();
  //std::cout << "Energy: " << energy << "\n";
  xacc::Finalize();
  return 0;
}