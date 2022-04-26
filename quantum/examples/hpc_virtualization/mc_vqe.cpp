#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "PauliOperator.hpp"
#include <random>
// Generate rando
auto random_vector(const double l_range, const double r_range,
                   const std::size_t size) {
  // Generate a random initial parameter set
  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  std::uniform_real_distribution<double> dist{l_range, r_range};
  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };
  std::vector<double> vec(size);
  std::generate(vec.begin(), vec.end(), gen);
  return vec;
}

std::shared_ptr<xacc::CompositeInstruction> getCircuit(const int nq) {
  auto provider = xacc::getIRProvider("quantum");
  auto circuit = provider->createComposite("circuit");
  std::vector<std::shared_ptr<xacc::Instruction>> gates;
  std::vector<double> x(2 * nq + 1, 1.0);
  
  int varIdx = 0;
  auto ry = provider->createInstruction("Ry", {0}, {x[varIdx++]});
  gates.push_back(ry);

  for (int i = 1; i < nq; i++) {

    std::size_t control = i - 1, target = i;
    // Ry(-theta/2)
    auto ry_minus =
        provider->createInstruction("Ry", {target}, {x[varIdx++]});
    gates.push_back(ry_minus);

    auto hadamard1 = provider->createInstruction("H", {target});
    gates.push_back(hadamard1);

    auto cnot = provider->createInstruction("CNOT", {control, target});
    gates.push_back(cnot);

    auto hadamard2 = provider->createInstruction("H", {target});
    gates.push_back(hadamard2);

    // Ry(+theta/2)
    auto ry_plus =
        provider->createInstruction("Ry", {target}, {x[varIdx++]});
    gates.push_back(ry_plus);
  }

  // Wall of CNOTs
  for (int i = nq - 2; i >= 0; i--) {
    for (int j = nq - 1; j > i; j--) {

      std::size_t control = j, target = i;
      auto cnot = provider->createInstruction("CNOT", {control, target});
      gates.push_back(cnot);
    }
  }

  circuit->addInstructions(gates);

  // end CIS

  

  auto entangler = [&](const std::size_t control, const std::size_t target) {

    std::vector<std::shared_ptr<xacc::Instruction>> gates;
    std::vector<std::string> varNames;
    auto cnot = provider->createInstruction("CNOT", {control, target});
    gates.push_back(cnot);

    std::string varName = "t" + std::to_string(varIdx++);
    auto ry = provider->createInstruction("Ry", {target}, {varName});
    gates.push_back(ry);
    varNames.push_back(varName);

    varName = "t" + std::to_string(varIdx++);
    ry = provider->createInstruction("Ry", {control}, {varName});
    gates.push_back(ry);
    varNames.push_back(varName);

    cnot = provider->createInstruction("CNOT", {control, target});
    gates.push_back(cnot);

    varName = "t" + std::to_string(varIdx++);
    ry = provider->createInstruction("Ry", {target}, {varName});
    gates.push_back(ry);
    varNames.push_back(varName);

    varName = "t" + std::to_string(varIdx++);
    ry = provider->createInstruction("Ry", {control},{varName});
    gates.push_back(ry);
    varNames.push_back(varName);

    circuit->addVariables(varNames);
    circuit->addInstructions(gates);
  };

  varIdx = 0;
  for (std::size_t i = 0; i < nq; i++) {
    std::string varName = "t" + std::to_string(varIdx++);
    auto ry = provider->createInstruction("Ry", {i}, {varName});
    circuit->addVariable(varName);
    circuit->addInstruction(ry);
  }

  for (int layer : {0, 1}) {
    for (int i = layer; i < nq - layer; i += 2) {
      std::size_t control = i, target = i + 1;
      entangler(control, target);
    }
  }

  return circuit;
}

std::shared_ptr<xacc::Observable> getH(const int nq) {

  xacc::quantum::PauliOperator hamiltonian;
  std::vector<double> coeff(2 * nq, 1.0);

  int A = 0;
  for (int A = 0; A < nq; A++) {
    hamiltonian += xacc::quantum::PauliOperator({{A, "Z"}}, coeff[2 * A]);
    hamiltonian += xacc::quantum::PauliOperator({{A, "X"}}, coeff[2 * A + 1]);
  }

  std::vector<std::vector<int>> pairs(nq);
  for (int A = 0; A < nq; A++) {
   if (A == 0) {
      pairs[A] = {A + 1};
    } else if (A == nq - 1) {
      pairs[A] = {A - 1};
    } else {
      pairs[A] = {A - 1, A + 1};
    }
  }

  coeff.resize(6 * nq);
  coeff = std::vector<double>(6 * nq, 1.0);
  int i = 0;
  for (int A = 0; A < nq; A++) {
    for (int B : pairs[A]) {

      hamiltonian += xacc::quantum::PauliOperator({{A, "X"}, {B, "X"}}, coeff[i++]);
      hamiltonian += xacc::quantum::PauliOperator({{A, "X"}, {B, "Z"}}, coeff[i++]);
      hamiltonian += xacc::quantum::PauliOperator({{A, "Z"}, {B, "X"}}, coeff[i]);
      hamiltonian += xacc::quantum::PauliOperator({{A, "Z"}, {B, "Z"}}, coeff[i++]);
    }
  }

    return std::make_shared<xacc::quantum::PauliOperator>(hamiltonian);

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  std::vector<std::string> arguments(argv + 1, argv + argc);
  int n_virt_qpus = 1,  nq = 4;
  std::string acc = "aer";
  for (int i = 0; i < arguments.size(); i++) {
    if (arguments[i] == "--n-virtual-qpus") {
      n_virt_qpus = std::stoi(arguments[i + 1]);
    }
    if (arguments[i] == "--n-qubits") {
      nq = std::stoi(arguments[i + 1]);
    }
    if (arguments[i] == "--accelerator") {
      acc = arguments[i + 1];
    }
  }

  xacc::ScopeTimer timer("mpi_timing", false);
  auto accelerator = xacc::getAccelerator(acc, {{"tnqvm-visitor", "exatn-mps"}});
  accelerator = xacc::getAcceleratorDecorator("hpc-virtualization", accelerator,
                                              {{"vqe-mode", false},
                                              {"n-virtual-qpus", n_virt_qpus}});
  auto buffer = xacc::qalloc(nq);

  auto ansatz = getCircuit(nq);
  std::vector<double> params(ansatz->nVariables(), 0.0); 
  
  auto observable = getH(nq);

  auto parameterShift = xacc::getGradient("parameter-shift", {{"observable", observable}, {"shift-scalar", 0.5}});
  auto gradientInstructions =
      parameterShift->getGradientExecutions(ansatz, params);
  std::cout << "Circuit depth: " << ansatz->depth() << "\n";
  std::cout << "Number of parameters: " << ansatz->nVariables() << "\n";
  std::cout << "Number of circuit executions: " << gradientInstructions.size() << "\n"; 

  accelerator->execute(buffer, gradientInstructions);

  auto isRank0 = buffer->hasExtraInfoKey("rank")
                             ? ((*buffer)["rank"].as<int>() == 0)
                             : true;

  if(isRank0) {
    std::vector<double> dx(ansatz->nVariables());
    parameterShift->compute(dx, buffer->getChildren());
    auto run_time = timer.getDurationMs();
    std::cout << "\nRuntime: " << run_time << "\n";
  }
  xacc::Finalize();
}