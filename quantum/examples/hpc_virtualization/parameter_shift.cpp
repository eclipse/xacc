#include "xacc.hpp"
#include "xacc_observable.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  auto accelerator = xacc::getAccelerator("qpp");
  accelerator = xacc::getAcceleratorDecorator("hpc-virtualization", accelerator,
                                              {{"n-virtual-qpus", 2}});
  auto buffer = xacc::qalloc(3);
  auto observable = xacc::quantum::getObservable("pauli", std::string("Y0 Z2"));

  auto provider = xacc::getIRProvider("quantum");
  auto ansatz = provider->createComposite("testCircuit");
  std::vector<std::string> varNames = {"x0", "x1", "x2", "x3", "x4", "x5"};
  ansatz->addVariables(varNames);
  ansatz->addInstruction(provider->createInstruction("Rx", {0}, {"x0"}));
  ansatz->addInstruction(provider->createInstruction("Ry", {1}, {"x1"}));
  ansatz->addInstruction(provider->createInstruction("Rz", {2}, {"x2"}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {0, 1}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {1, 2}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {2, 0}));
  ansatz->addInstruction(provider->createInstruction("Rx", {0}, {"x3"}));
  ansatz->addInstruction(provider->createInstruction("Ry", {1}, {"x4"}));
  ansatz->addInstruction(provider->createInstruction("Rz", {2}, {"x5"}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {0, 1}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {1, 2}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {2, 0}));


  std::vector<double> params{0.37454012, 0.95071431, 0.73199394, 0.59865848, 0.15601864, 0.15599452};
  auto parameterShift = xacc::getGradient("parameter-shift", {{"observable", observable}, {"shift-scalar", 0.5}});
  xacc::ScopeTimer timer("mpi_timing", false);
  auto gradientInstructions =
      parameterShift->getGradientExecutions(ansatz, params);
      accelerator->execute(buffer, gradientInstructions);


  auto isRank0 = buffer->hasExtraInfoKey("rank")
                             ? ((*buffer)["rank"].as<int>() == 0)
                             : true;

  if(isRank0) {
    std::vector<double> dx(6);
    parameterShift->compute(dx, buffer->getChildren());
    auto run_time = timer.getDurationMs();
    for (auto grad : dx) std::cout << grad << ", ";
      std::cout << "\nRuntime: " << run_time << " ms.\n";
    }
  xacc::Finalize();
}