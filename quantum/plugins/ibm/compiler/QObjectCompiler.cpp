/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "QObjectCompiler.hpp"
#include "QObject.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"
#include "QObjectExperimentVisitor.hpp"
#include "InstructionIterator.hpp"

namespace xacc {

namespace quantum {

std::shared_ptr<IR> QObjectCompiler::compile(const std::string &src,
                                             std::shared_ptr<Accelerator> acc) {

  auto jsonstr = std::regex_replace(src, std::regex("\'"), "\"");
  auto replaceFalse = [](std::string &str) {
    size_t start_pos = 0;
    while ((start_pos = str.find("False", start_pos)) != std::string::npos) {
      str.replace(start_pos, std::string("False").length(), "false");
      start_pos +=
          std::string("false").length();
    }
  };

  replaceFalse(jsonstr);

  auto provider = xacc::getService<IRProvider>("quantum");
  auto ir = provider->createIR();
  auto json = nlohmann::json::parse(jsonstr);
  xacc::ibm::QObjectRoot qobj;
  nlohmann::from_json(json, qobj);

  auto experiments = qobj.get_q_object().get_experiments();
  for (auto &e : experiments) {
    auto function = provider->createComposite(e.get_header().get_name());
    auto instructions = e.get_instructions();
    for (auto &i : instructions) {
      auto name = i.get_name();
      auto tmpbits = i.get_qubits();
      std::vector<std::size_t> bits(tmpbits.begin(), tmpbits.end());
      std::shared_ptr<Instruction> xi;
      if (name == "measure") {
        int memory = i.get_memory()[0];
        xi = provider->createInstruction("Measure", bits);
      } else {
        auto params = i.get_params();
        if (name == "u1") {
          xi = provider->createInstruction("U", bits, {0.0, 0.0, params[0]});
        } else if (name == "u2") {
          xi = provider->createInstruction(
              "U", bits, {3.1415926 / 2., params[0], params[1]});
        } else if (name == "cx") {
          xi = provider->createInstruction("CNOT", bits, {});
        } else if (name == "u3") {
          // U3
          xi = provider->createInstruction("U", bits,
                                           {params[0], params[1], params[2]});
        } else {

            name[0] = std::toupper(name[0]);

            xi = provider->createInstruction(name, bits);
            int counter = 0;
            for (auto& p : params) {
                InstructionParameter ip(p);
                xi->setParameter(counter, ip);
                counter++;
            }

        }
      }

      if (xi)
        function->addInstruction(xi);
    }

    ir->addComposite(function);
  }

  return ir;
}

std::shared_ptr<IR> QObjectCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string
QObjectCompiler::translate(std::shared_ptr<xacc::CompositeInstruction> function) {

  xacc::ibm::QObject qobj;
  qobj.set_qobj_id("xacc-qobj-id");
  qobj.set_schema_version("1.1.0");
  qobj.set_type("QASM");
  qobj.set_header(QObjectHeader());
  std::vector<xacc::ibm::Experiment> experiments;

  auto uniqueBits = function->uniqueBits();
  // The number of qubits required for an experiment is the number of *physical* qubits,
  // i.e. the max index of qubit used in the circuit.  
  auto nbRequiredBits = function->nPhysicalBits();
  auto visitor =
      std::make_shared<QObjectExperimentVisitor>(function->name(), nbRequiredBits);

  InstructionIterator it(function);
  int memSlots = 0;
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }

  // After calling getExperiment, maxMemorySlots should be
  // maxClassicalBit + 1
  auto experiment = visitor->getExperiment();
  experiments.push_back(experiment);
  int maxMemSlots = visitor->maxMemorySlots;

  // Create the QObj Config
  xacc::ibm::QObjectConfig config;
  config.set_shots(1024);
  config.set_memory(false);
  config.set_meas_level(2);
  config.set_memory_slots(maxMemSlots);
  config.set_meas_return("avg");
  config.set_memory_slot_size(100);
  config.set_n_qubits(50);

  // Add the experiments and config
  qobj.set_experiments(experiments);
  qobj.set_config(config);

  // Set the Backend
  xacc::ibm::Backend bkend;
  bkend.set_name("ibmq_qasm_simulator");

  // Create the Root node of the QObject
  xacc::ibm::QObjectRoot root;
  root.set_backend(bkend);
  root.set_q_object(qobj);

  // Create the JSON String to send
  nlohmann::json j;
  nlohmann::to_json(j, root);
  return j.dump();
}

const std::string
QObjectCompiler::translate(std::shared_ptr<CompositeInstruction> function,
                           HeterogeneousMap &options) {
  bool shouldSkipIdGates = true;
  if (options.keyExists<bool>("skip-id-gates")) {
    shouldSkipIdGates = options.get<bool>("skip-id-gates");
  }

  xacc::ibm::QObject qobj;
  qobj.set_qobj_id("xacc-qobj-id");
  qobj.set_schema_version("1.1.0");
  qobj.set_type("QASM");
  qobj.set_header(QObjectHeader());
  std::vector<xacc::ibm::Experiment> experiments;

  auto uniqueBits = function->uniqueBits();
  // The number of qubits required for an experiment is the number of *physical*
  // qubits, i.e. the max index of qubit used in the circuit.
  auto nbRequiredBits = function->nPhysicalBits();
  auto visitor = std::make_shared<QObjectExperimentVisitor>(
      function->name(), nbRequiredBits, QObjectExperimentVisitor::GateSet::U_CX,
      // Include/skip Id gates depending on users' options
      shouldSkipIdGates);

  InstructionIterator it(function);
  int memSlots = 0;
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }

  // After calling getExperiment, maxMemorySlots should be
  // maxClassicalBit + 1
  auto experiment = visitor->getExperiment();
  experiments.push_back(experiment);
  int maxMemSlots = visitor->maxMemorySlots;

  // Create the QObj Config
  xacc::ibm::QObjectConfig config;
  config.set_shots(1024);
  config.set_memory(false);
  config.set_meas_level(2);
  config.set_memory_slots(maxMemSlots);
  config.set_meas_return("avg");
  config.set_memory_slot_size(100);
  config.set_n_qubits(50);

  // Add the experiments and config
  qobj.set_experiments(experiments);
  qobj.set_config(config);

  // Set the Backend
  xacc::ibm::Backend bkend;
  bkend.set_name("ibmq_qasm_simulator");

  // Create the Root node of the QObject
  xacc::ibm::QObjectRoot root;
  root.set_backend(bkend);
  root.set_q_object(qobj);

  // Create the JSON String to send
  nlohmann::json j;
  nlohmann::to_json(j, root);
  return j.dump();
}
} // namespace quantum
} // namespace xacc
