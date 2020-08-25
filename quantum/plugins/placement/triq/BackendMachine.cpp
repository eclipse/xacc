#include "BackendMachine.hpp"
#include "NoiseModel.hpp"

namespace xacc {
BackendMachine::BackendMachine(const NoiseModel &backendNoiseModel) {
  // Generates the three config files the initialize the base class:
  const size_t nbQubits = backendNoiseModel.nQubits();
  for (size_t i = 0; i < nbQubits; ++i) {
    qubits.push_back(new HwQubit(i));
  }
  // TODO: construct the three temporary files for single-qubit, readoutm and
  // two-qubit fidelity based on the noise model data.
  std::string sFileName, mFileName, tFileName;
  
  // Load to TriQ Machine model
  read_s_reliability(sFileName);
  read_m_reliability(mFileName);
  read_t_reliability(tFileName);
  compute_swap_paths();
  print_swap_paths();
  compute_swap_info();
}

} // namespace xacc