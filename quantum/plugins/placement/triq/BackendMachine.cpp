#include "BackendMachine.hpp"
#include "NoiseModel.hpp"
#include "xacc.hpp"

namespace {
// Returns the temp. file name
template <typename RowIterFn>
std::string createConfigFile(size_t in_nbRows, RowIterFn rowIter) {
  std::stringstream ss;
  ss << in_nbRows << "\n";
  for (size_t i = 0; i < in_nbRows; ++i) {
    ss << rowIter(i) << "\n";
  }
  char fnTemplate[] = "/tmp/ConfigXXXXXX";
  if (mkstemp(fnTemplate) == -1) {
    xacc::error("Failed to create a temporary file.");
  }
  const std::string configFilename(fnTemplate);
  std::ofstream inFile(configFilename);
  inFile << ss.str();
  return configFilename;
}
} // namespace

namespace xacc {
BackendMachine::BackendMachine(const NoiseModel &backendNoiseModel) {
  // Generates the three config files the initialize the base class:
  const size_t nbQubits = backendNoiseModel.nQubits();
  nQ = nbQubits;
  for (size_t i = 0; i < nbQubits; ++i) {
    qubits.push_back(new HwQubit(i));
  }

  // Query fidelity information:
  const auto roErrors = backendNoiseModel.readoutErrors();
  const auto mFileName =
      createConfigFile(roErrors.size(), [&roErrors](size_t in_idx) {
        std::stringstream ss;
        const auto [meas0Prep1, meas1Prep0] = roErrors[in_idx];
        const double avgRoFidelity =
            0.5 * ((1.0 - meas0Prep1) + (1.0 - meas1Prep0));
        ss << in_idx << " " << avgRoFidelity;
        return ss.str();
      });

  const auto singleQubitFidelity =
      backendNoiseModel.averageSingleQubitGateFidelity();
  const auto sFileName = createConfigFile(
      singleQubitFidelity.size(), [&singleQubitFidelity](size_t in_idx) {
        std::stringstream ss;
        ss << in_idx << " " << singleQubitFidelity[in_idx];
        return ss.str();
      });

  const auto twoQubitFidelity = backendNoiseModel.averageTwoQubitGateFidelity();
  // TriQ expects single fidelity for a pair of qubits;
  // IBM provides fidelity for both cx_0_1 and cx_1_0
  // hence we need to average them.
  const auto twoQubitFidelityAvg = [&]() {
    std::vector<std::pair<size_t, size_t>> processedPairs;
    std::vector<std::tuple<size_t, size_t, double>> avgData;
    for (const auto &fidelity_data : twoQubitFidelity) {
      const auto q1 = std::get<0>(fidelity_data);
      const auto q2 = std::get<1>(fidelity_data);
      const auto fidelity = std::get<2>(fidelity_data);
      if (!xacc::container::contains(processedPairs, std::make_pair(q1, q2))) {
        assert(
            !xacc::container::contains(processedPairs, std::make_pair(q2, q1)));
        const double fid1 = fidelity;
        const auto iter =
            std::find_if(twoQubitFidelity.begin(), twoQubitFidelity.end(),
                         [&](const auto &fidTuple) {
                           return (std::get<0>(fidTuple) == q2) &&
                                  (std::get<1>(fidTuple) == q1);
                         });
        assert(iter != twoQubitFidelity.end());
        const double fid2 = std::get<2>(*iter);
        avgData.emplace_back(std::make_tuple(q1, q2, (fid1 + fid2) / 2.0));
        processedPairs.emplace_back(std::make_pair(q1, q2));
        processedPairs.emplace_back(std::make_pair(q2, q1));
      }
    }
    return avgData;
  }();
  assert(twoQubitFidelityAvg.size() * 2 == twoQubitFidelity.size());
  xacc::info("Two-qubit fidelity data: ");
  const auto tFileName = createConfigFile(
      twoQubitFidelityAvg.size(), [&twoQubitFidelityAvg](size_t in_idx) {
        std::stringstream ss;
        const auto [q1, q2, fidelity] = twoQubitFidelityAvg[in_idx];
        ss << q1 << " " << q2 << " " << fidelity;
        xacc::info("CX " + ss.str());
        return ss.str();
      });
  {
    std::cout << std::flush;
    auto origBuf = std::cout.rdbuf();
    if (!xacc::verbose) {
      std::cout.rdbuf(NULL);
    }
    // Load to TriQ Machine model
    read_s_reliability(sFileName);
    read_m_reliability(mFileName);
    read_t_reliability(tFileName);
    compute_swap_paths();
    print_swap_paths();
    compute_swap_info();
    std::cout.rdbuf(origBuf);
  }

  // Clean-up the temporary files.
  remove(sFileName.c_str());
  remove(mFileName.c_str());
  remove(tFileName.c_str());
}

void XaccTargetter::print_header(std::ofstream &out_file) {
  const auto nbQubits = M->nQ;
  out_file << "OPENQASM 2.0;\n";
  out_file << "include \"qelib1.inc\";\n";
  out_file << "qreg q[" << nbQubits << "];\n";
  out_file << "creg c[" << nbQubits << "];\n";
}

void XaccTargetter::add_gate_print_maps() {
  gate_print_ibm[typeid(RX)] = "rx";
  gate_print_ibm[typeid(RY)] = "ry";
  gate_print_ibm[typeid(RZ)] = "rz";
  gate_print_ibm[typeid(CZ)] = "cz";
}

void XaccTargetter::print_one_qubit_gate(::Gate *g, std::ofstream &out_file,
                                         int is_last_gate) {
  int qid = g->vars[0]->id;
  if (typeid(*g) == typeid(MeasZ)) {
    return;
  }
  if (typeid(*g) == typeid(U1) && g->lambda == 0) {
    return;
  }
  const auto gate_name = gate_print_ibm[typeid(*g)];
  out_file << gate_name;
  if (typeid(*g) == typeid(U1)) {
    out_file << setprecision(15) << "(" << g->lambda << ")";
  } else if (typeid(*g) == typeid(U2)) {
    out_file << setprecision(15) << "(" << g->phi << "," << g->lambda << ")";
  } else if (typeid(*g) == typeid(U3)) {
    out_file << setprecision(15) << "(" << g->theta << "," << g->phi << ","
             << g->lambda << ")";
  }
  out_file << " ";
  std::stringstream var_name;
  var_name << "q";
  var_name << "[";
  var_name << qid;
  var_name << "]";
  out_file << var_name.str() << ";\n";
}

void XaccTargetter::print_two_qubit_gate(::Gate *g, std::ofstream &out_file,
                                         int is_last_gate) {
  const auto gate_name = gate_print_ibm[typeid(*g)];
  out_file << gate_name << " q[" << g->vars[0]->id << "]"
           << ","
           << "q[" << g->vars[1]->id << "]"
           << ";\n";
}

void XaccTargetter::print_measure_ops(std::ofstream &out_file) {
  for (const auto &[q, hwq] : final_map) {
    out_file << "measure "
             << "q[" << hwq << "] -> c[" << q->id << "];\n";
  }
}
void XaccTargetter::print_footer(std::ofstream &out_file) {}
} // namespace xacc