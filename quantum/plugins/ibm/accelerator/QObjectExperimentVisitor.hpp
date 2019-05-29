/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_GATE_ACCELERATORS_QOBJECTVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QOBJECTVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include "QObject.hpp"

using namespace xacc::ibm;

namespace xacc {
namespace quantum {
class QObjectExperimentVisitor : public AllGateVisitor {
protected:
  constexpr static double pi = 3.1415926;

  std::vector<xacc::ibm::Instruction> instructions;
  Experiment experiment;
  std::string experimentName;
  int nTotalQubits = 0;
  std::map<int,int> qubit2MemorySlot;

public:
  int maxMemorySlots;
  const std::string name() const override { return "qobject-visitor"; }

  const std::string description() const override {
    return "Map XACC IR to QObject.";
  }

  QObjectExperimentVisitor(const std::string expName, std::vector<int> qubits)
      : experimentName(expName), nTotalQubits(qubits.size()) {
          int counter=0;
          for (auto& b : qubits) {
              qubit2MemorySlot.insert({b,counter});
              counter++;
          }
      }

  const std::string toString() override {
    experiment = getExperiment();
    nlohmann::json json;
    nlohmann::to_json(json, experiment);
    return json.dump();
  }

  Experiment getExperiment() {
    //   xacc::info("Getting Experiment " + experimentName + ", " + std::to_string(experiment.get_instructions().size()));
    if (experiment.get_instructions().empty()) {

      maxMemorySlots++;
      
      // construct config and header, add
      ExperimentHeader header;
      header.set_name(experimentName);
      header.set_qreg_sizes(
          {{QregSizeElement("q"), QregSizeElement(nTotalQubits)}});
      header.set_n_qubits(nTotalQubits);
      std::vector<std::vector<QregSizeElement>> qlabels;
      for (int i = 0; i < nTotalQubits; i++) {
        qlabels.push_back({"q", i});
      }
      header.set_qubit_labels(qlabels);
      header.set_memory_slots(maxMemorySlots);

      std::vector<std::vector<ClbitLabel>> clabels;
      for (int i = 0; i < maxMemorySlots; i++) {
        clabels.push_back({"c0", i});
      }
      header.set_clbit_labels(clabels);
      header.set_creg_sizes({{"c0", maxMemorySlots}});

      ExperimentConfig config;
      config.set_memory_slots(maxMemorySlots);
      config.set_n_qubits(nTotalQubits);

    //   Experiment experiment;
      experiment.set_config(config);
      experiment.set_header(header);
    //   xacc::info("Adding insts " + std::to_string(instructions.to))
      experiment.set_instructions(instructions);
    }
        //   xacc::info("Returning Experiment " + experimentName + ", " + std::to_string(experiment.get_instructions().size()));

    return experiment;
  }

  void visit(Hadamard &h) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(h.bits()[0]);
    inst.get_mutable_name() = "u2";
    inst.set_params({0.0, pi});

    instructions.push_back(inst);
  }

  void visit(Identity &i) override {
    // std::stringstream js;
    // native += "id q[" + std::to_string(i.bits()[0]) + "];\n";
    // js << "{\"name\":\"id\",\"params\": [],\"qubits\":[" <<
    // i.bits()[0]<<"]},"; operationsJsonStr += js.str();
  }

  void visit(CZ &cz) override { xacc::error("cz not supported"); }

  /**
   * Visit CNOT gates
   */
  void visit(CNOT &cn) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(cn.bits()[0]);
    inst.get_mutable_qubits().push_back(cn.bits()[1]);
    inst.get_mutable_name() = "cx";

    instructions.push_back(inst);
  }

  /**
   * Visit X gates
   */
  void visit(X &x) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(x.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({pi, 0.0, pi});

    instructions.push_back(inst);
  }

  void visit(Y &y) override {

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(y.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({pi, pi / 2.0, pi});

    instructions.push_back(inst);
  }

  void visit(Z &z) override {

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(z.bits()[0]);
    inst.get_mutable_name() = "u1";
    inst.set_params({pi});

    instructions.push_back(inst);
  }

  void visit(U &u) override {

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(u.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({u.getParameter(0).as<double>(),
                     u.getParameter(1).as<double>(),
                     u.getParameter(2).as<double>()});

    instructions.push_back(inst);
  }

  std::vector<int> usedMemorySlots;

  void visit(Measure &m) override {
    if (std::find(usedMemorySlots.begin(), usedMemorySlots.end(),
                  m.getParameter(0).as<int>()) != usedMemorySlots.end()) {
      xacc::error(
          "IBM: Invalid classical bit index for measurement, already used.");
    }

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(m.bits()[0]);
    inst.get_mutable_name() = "measure";
    inst.set_memory({qubit2MemorySlot[m.bits()[0]]});

    instructions.push_back(inst);

    if (qubit2MemorySlot[m.bits()[0]] > maxMemorySlots) {
        maxMemorySlots = qubit2MemorySlot[m.bits()[0]];
    }
  }

  void visit(ConditionalFunction &c) override {}

  void visit(Rx &rx) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(rx.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params(
        {rx.getParameter(0).as<double>(), -1.0 * pi / 2.0, pi / 2.0});

    instructions.push_back(inst);
  }

  void visit(Ry &ry) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(ry.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({ry.getParameter(0).as<double>(), 0.0, 0.0});

    instructions.push_back(inst);
  }

  void visit(Rz &rz) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(rz.bits()[0]);
    inst.get_mutable_name() = "u1";
    inst.set_params({rz.getParameter(0).as<double>()});

    instructions.push_back(inst);
  }

  void visit(CPhase &cp) override {}

  void visit(Swap &s) override {
    CNOT c1(s.bits()), c2(s.bits()[1], s.bits()[0]), c3(s.bits());
    visit(c1);
    visit(c2);
    visit(c3);
  }

  void visit(GateFunction &f) override { return; }

  /**
   * The destructor
   */
  virtual ~QObjectExperimentVisitor() {}
};

} // namespace quantum
} // namespace xacc

#endif
