#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
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

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  std::vector<std::string> arguments(argv + 1, argv + argc);
  int n_virt_qpus = 1,  n_layers = 10;
  std::string acc;
  xacc::set_verbose(true);
  xacc::setLoggingLevel(10);
  for (int i = 0; i < arguments.size(); i++) {
    if (arguments[i] == "--n-virtual-qpus") {
      n_virt_qpus = std::stoi(arguments[i + 1]);
    }
    if (arguments[i] == "--n-layers") {
      n_layers = std::stoi(arguments[i + 1]);
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
  auto buffer = xacc::qalloc(2);
  auto observable = xacc::quantum::getObservable("pauli", 
      std::string("-0.349833 - 0.388748 Z0 - 0.388748 Z1 + 0.181771 X0X1 + 0.0111772 Z0Z1"));

  auto ansatz = std::dynamic_pointer_cast<xacc::CompositeInstruction>(
      xacc::getService<xacc::Instruction>("hwe"));

  ansatz->expand({
          std::make_pair("nq", 2),
          std::make_pair("layers", n_layers),
          std::make_pair("coupling", std::vector<std::pair<int,int>>{{0, 1}})
          });

  std::vector<double> params = random_vector(-1.0, 1.0, ansatz->nVariables());

  auto parameterShift = xacc::getGradient("parameter-shift", {{"observable", observable}, {"shift-scalar", 0.5}});
  auto gradientInstructions =
      parameterShift->getGradientExecutions(ansatz, params);
  accelerator->execute(buffer, gradientInstructions);


  auto isRank0 = buffer->hasExtraInfoKey("rank")
                             ? ((*buffer)["rank"].as<int>() == 0)
                             : true;

  if(isRank0) {
    std::vector<double> dx(ansatz->nVariables());
    parameterShift->compute(dx, buffer->getChildren());
    auto run_time = timer.getDurationMs();
    std::cout << run_time << "\n";
  }
  xacc::Finalize();
}