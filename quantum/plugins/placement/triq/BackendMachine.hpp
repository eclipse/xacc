#include "machine.hpp"
#include "targetter.hpp"
namespace xacc {
class NoiseModel;
// Override TriQ's Machine class
class BackendMachine : public ::Machine {
public:
  BackendMachine(const NoiseModel &backendNoiseModel);
};

// Override TriQ to print out QASM rather than Qiskit Python code:
class XaccTargetter : public ::Targetter {
public:
  XaccTargetter(::Machine *m, ::Circuit *c,
                std::map<::Qubit *, int> *initialMap,
                std::vector<::Gate *> *gateOrder,
                map<::Gate *, ::BacktrackSolution *> bSol)
      : ::Targetter(m, c, initialMap, gateOrder, bSol) {
    add_gate_print_maps();
  }

  virtual void print_one_qubit_gate(::Gate *g, ofstream &out_file,
                            int is_last_gate) override;
  virtual void print_two_qubit_gate(::Gate *g, ofstream &out_file,
                            int is_last_gate) override;
  virtual void print_header(ofstream &out_file) override;
  virtual void print_measure_ops(ofstream &out_file) override;
  virtual void print_footer(ofstream &out_file) override;
  void add_gate_print_maps();
};
} // namespace xacc