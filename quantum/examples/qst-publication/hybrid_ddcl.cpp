#include "xacc.hpp"
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto qpu =
      xacc::getAccelerator("qpp");

  auto opt =
            xacc::getOptimizer("mlpack");
  xacc::qasm(R"(
.compiler xasm
.circuit ansatz
.parameters x
.qbit q
U(q[0], x[0], -pi/2, pi/2 );
U(q[0], 0, 0, x[1]);
U(q[1], x[2], -pi/2, pi/2);
U(q[1], 0, 0, x[3]);
CNOT(q[0], q[1]);
U(q[0], 0, 0, x[4]);
U(q[0], x[5], -pi/2, pi/2);
U(q[1], 0, 0, x[6]);
U(q[1], x[7], -pi/2, pi/2);
)");
  auto ansatz = xacc::getCompiled("ansatz");

  std::vector<double> target_distribution
                    {.5, .5, .5, .5};

  auto ddcl = xacc::getAlgorithm("ddcl");
  ddcl->initialize({
        std::make_pair("ansatz", ansatz),
        std::make_pair("target_dist",
                target_distribution),
        std::make_pair("accelerator", qpu),
        std::make_pair("loss", "js"),
        std::make_pair("gradient",
                "js-parameter-shift"),
        std::make_pair("optimizer", opt)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  ddcl->execute(buffer);

  // Print the result
  std::cout << "Loss: " <<
        (*buffer)["opt-val"].as<double>()
            << "\n";
}
