#include "XACC.hpp"
#include "Optimizer.hpp"
#include "PauliOperator.hpp"
#include "xacc_service.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  // specified by --accelerator argument
  auto accelerator = xacc::getAccelerator();

  // Allocate some qubits
  auto buffer = xacc::qalloc(2);

  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 =
      std::make_shared<xacc::quantum::PauliOperator>("5.907 - 2.1433 X0X1 "
                                                     "- 2.1433 Y0Y1"
                                                     "+ .21829 Z0 - 6.125 Z1");
  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler quil
.function deuteron_ansatz
X 0
Ry(theta) 1
CNOT 1 0
)");
  auto ansatz = xacc::getCompiled("deuteron_ansatz");

  // Observe the Ansatz, creating measured kernels
  auto kernels = H_N_2->observe(ansatz);

 // Create the OptFunction to be optimized
  xacc::OptFunction vqeOptFunc(
      [&](const std::vector<double> &x) -> double {
        std::vector<double> coefficients;
        std::vector<std::shared_ptr<xacc::Function>> fsToExec;

        // Get function coefficients (pauli term weights)
        // into a list, separate from the identity coeff
        double identityCoeff = 0.0;
        for (auto &f : kernels) {
          std::complex<double> coeff =
              f->getOption("coefficient").as<std::complex<double>>();

          if (f->nInstructions() > ansatz->nInstructions()) {
            fsToExec.push_back(f->operator()(x));
            coefficients.push_back(std::real(coeff));
          } else {
            identityCoeff += std::real(coeff);
          }
        }

        // Execute on the QPU!
        auto buffers = accelerator->execute(buffer, fsToExec);

        // Compute the Energy
        double energy = identityCoeff;
        for (int i = 0; i < buffers.size(); i++) {
          energy += buffers[i]->getExpectationValueZ() * coefficients[i];
        }

        // Print the energy at this iteration
        std::stringstream ss;
        ss << "E(" << x[0];
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << energy;
        xacc::info(ss.str());
        return energy;
      },
      ansatz->nParameters());

  // Run Optimization routine, default is NLOPT COBYLA
  auto optimizer = xacc::getOptimizer("nlopt");
  auto results = optimizer->optimize(vqeOptFunc);

  // Print the result
  std::cout << "Energy: " << results.first << "\n";
}