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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "QsimAccelerator.hpp"
#include "xacc_plugin.hpp"
#include "IRUtils.hpp"
#include <cassert>
#include <optional>
#include <thread>
namespace {
inline bool isMeasureGate(const xacc::InstPtr &in_instr) {
  return (in_instr->name() == "Measure");
}

bool shotCountFromFinalStateVec(
    const std::shared_ptr<xacc::CompositeInstruction> &in_composite) {
  xacc::InstructionIterator it(in_composite);
  bool measureAtTheEnd = true;
  bool measureEncountered = false;

  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      if (isMeasureGate(nextInst)) {
        // Flag that we have seen a Measure gate.
        measureEncountered = true;
      }

      // We have seen a Measure gate but this one is not another Measure gate.
      if (measureEncountered && !isMeasureGate(nextInst)) {
        measureAtTheEnd = false;
      }
    }
  }

  // If Measure gates are at the very end,
  // this Composite can be simulated by random sampling from the state vec.
  return measureAtTheEnd;
}

// For debug:
template <typename StateSpace, typename State>
void PrintAmplitudes(unsigned num_qubits, const StateSpace &state_space,
                     const State &state) {
  static constexpr char const *bits[8] = {
      "000", "001", "010", "011", "100", "101", "110", "111",
  };

  uint64_t size = std::min(uint64_t{8}, uint64_t{1} << num_qubits);
  unsigned s = 3 - std::min(unsigned{3}, num_qubits);

  for (uint64_t i = 0; i < size; ++i) {
    auto a = state_space.GetAmpl(state, i);
    qsim::IO::messagef("%s:%16.8g%16.8g%16.8g\n", bits[i] + s, std::real(a),
                       std::imag(a), std::norm(a));
  }
}

// Compute exp-val-z
// 1. Copy state onto scratch
// 2. Evolve scratch forward with Z terms
// 3. Compute < state | scratch >
template <typename Params, typename StateSpace, typename State>
double computeExpValZ(const Params& params, size_t num_threads, const std::vector<size_t> &meas_bits,
                      const StateSpace &state_space, const State &state) {
  auto scratch = state_space.Create(state.num_qubits());
  // copy from src to scratch.
  const bool copyOk = state_space.Copy(state, scratch);
  assert(copyOk);
  qsim::Circuit<qsim::GateQSim<float>> meas_circuit;
  meas_circuit.num_qubits = state.num_qubits();
  size_t time = 0;
  for (const auto &bit : meas_bits) {
    meas_circuit.gates.emplace_back(qsim::GateZ<float>::Create(time++, bit));
  }
  using namespace qsim;
  using Fuser = MultiQubitGateFuser<IO, GateQSim<float>>;
  auto fused_circuit =
      Fuser::FuseGates(params, meas_circuit.num_qubits, meas_circuit.gates);
  xacc::quantum::QsimAccelerator::Simulator sim(num_threads);
  for (const auto &fused_gate : fused_circuit) {
    qsim::ApplyFusedGate(sim, fused_gate, scratch);
  }
  return state_space.RealInnerProduct(state, scratch);
}


} // namespace

namespace xacc {
namespace quantum {
void QsimAccelerator::initialize(const HeterogeneousMap &params) {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  // Seeding a random number.
  m_qsimParam.seed = std::rand();
  m_numThreads =
      std::max(1, static_cast<int>(std::thread::hardware_concurrency()));
  m_qsimParam.verbosity = xacc::verbose ? 1 : 0;
  m_shots = -1;
  if (params.keyExists<int>("shots")) {
    m_shots = params.get<int>("shots");
    if (m_shots < 1) {
      xacc::error("Invalid 'shots' parameter.");
    }
  }

  // Set Qsim runner params:
  if (params.keyExists<int>("seed")) {
    m_qsimParam.seed = params.get<int>("seed");
  }
  
  if (params.keyExists<int>("threads")) {
    m_numThreads = params.get<int>("threads");
  }
  // Enable VQE mode by default if not using shots.
  // Note: in VQE mode, only expectation values are computed.
  m_vqeMode = (m_shots < 1);
  if (params.keyExists<bool>("vqe-mode")) {
    m_vqeMode = params.get<bool>("vqe-mode");
    if (m_vqeMode) {
      xacc::info("Enable VQE Mode.");
    }
  }
}

void QsimAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> compositeInstruction) {
  const bool qsimSimulateSamples = m_shots > 0 && !m_vqeMode;
  const bool areAllMeasurementsTerminal =
      shotCountFromFinalStateVec(compositeInstruction);

  if (!qsimSimulateSamples || areAllMeasurementsTerminal) {
    // Construct Qsim circuit:
    QsimCircuitVisitor visitor(buffer->size());
    std::vector<size_t> measureBitIdxs;
    // Walk the IR tree, and visit each node
    InstructionIterator it(compositeInstruction);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->isEnabled()) {
        if (!isMeasureGate(nextInst)) {
          nextInst->accept(&visitor);
        } else {
          // Just collect the indices of measured qubit
          measureBitIdxs.emplace_back(nextInst->bits()[0]);
        }
      }
    }

    auto circuit = visitor.getQsimCircuit();
    StateSpace stateSpace(m_numThreads);
    State state = stateSpace.Create(circuit.num_qubits);
    stateSpace.SetStateZero(state);
    
    if (Runner::Run(m_qsimParam, Factory(m_numThreads), circuit, state)) {
      // PrintAmplitudes(circuit.num_qubits, stateSpace, state);
      if (qsimSimulateSamples) {
        // Generate bit strings
        xacc::info("Provided circuit has no intermediate measurements. "
                   "Sampling repeatedly from final state vector.");
        const auto samples =
            stateSpace.Sample(state, m_shots, m_qsimParam.seed);
        for (const auto &sample : samples) {
          std::string bitString;
          for (const auto &bit : measureBitIdxs) {
            const auto bit_mask = 1ULL << bit;
            bitString.push_back((sample & bit_mask) == bit_mask ? '1' : '0');
          }
          buffer->appendMeasurement(bitString);
        }
      } else {
        const double expectedValueZ =
            computeExpValZ(m_qsimParam, m_numThreads, measureBitIdxs, stateSpace, state);
        // Just add exp-val-z info
        buffer->addExtraInfo("exp-val-z", expectedValueZ);
      }
    } else {
      xacc::error("Failed to run the circuit.");
    }
  } else {
    // Must execute the circuit 'shots' times.
    // (measure gates in the middle, if statements, etc.)
    xacc::info("Provided circuit has intermediate measurements.");
    assert(m_shots > 0);
    for (size_t i = 0; i < m_shots; ++i) {
      const std::string tempBufferName =
          buffer->name() + "__" + std::to_string(i);
      auto temp_buffer = std::make_shared<xacc::AcceleratorBuffer>(
          tempBufferName, buffer->size());
      xacc::storeBuffer(temp_buffer);
      InstructionIterator it(compositeInstruction);
      while (it.hasNext()) {
        auto nextInst = it.next();
        if (nextInst->isEnabled() && !nextInst->isComposite()) {
          apply(temp_buffer, nextInst);
        }
        if (nextInst->name() == "ifstmt") {
          auto ifStmtCast = std::dynamic_pointer_cast<IfStmt>(nextInst);
          InstructionParameter ifBufferName(tempBufferName);
          ifStmtCast->setParameter(0, ifBufferName);
          ifStmtCast->expand({});
        }
      }
      buffer->appendMeasurement(
          temp_buffer->single_measurements_to_bitstring());
    }
  }
}

void QsimAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {
  // The time to do observed circuit analysis is not optimal for a large number
  // of circuits so just run it one by one.
  constexpr int MAX_NUMBER_CIRCUITS_TO_ANALYZE = 100;
  if (!m_vqeMode || compositeInstructions.size() <= 1 ||
      compositeInstructions.size() > MAX_NUMBER_CIRCUITS_TO_ANALYZE) {
    // Cannot run VQE mode, just run each composite independently.
    for (auto &f : compositeInstructions) {
      auto tmpBuffer =
          std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
      execute(tmpBuffer, f);
      buffer->appendChild(f->name(), tmpBuffer);
    }
  } else {
    xacc::info("Running VQE mode");
    auto kernelDecomposed =
        ObservedAnsatz::fromObservedComposites(compositeInstructions);
    // Always validate kernel decomposition in DEBUG
    assert(kernelDecomposed.validate(compositeInstructions));
    QsimCircuitVisitor visitor(buffer->size());
    // Base kernel:
    auto baseKernel = kernelDecomposed.getBase();
    // Basis-change + measures
    auto obsCircuits = kernelDecomposed.getObservedSubCircuits();
    // Walk the base IR tree, and visit each node
    InstructionIterator it(baseKernel);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->isEnabled() && !nextInst->isComposite()) {
        nextInst->accept(&visitor);
      }
    }

    // Run the base circuit:
    auto circuit = visitor.getQsimCircuit();
    StateSpace stateSpace(m_numThreads);
    State state = stateSpace.Create(circuit.num_qubits);
    stateSpace.SetStateZero(state);

    const bool runOk = Runner::Run(m_qsimParam, Factory(m_numThreads), circuit, state);
    assert(runOk);

    // Now we have a wavefunction that represents execution of the ansatz.
    // Run the observable sub-circuits (change of basis + measurements)
    for (int i = 0; i < obsCircuits.size(); ++i) {
      auto tmpBuffer = std::make_shared<xacc::AcceleratorBuffer>(
          obsCircuits[i]->name(), buffer->size());
      const double e = getExpectationValueZ(obsCircuits[i], stateSpace, state);
      tmpBuffer->addExtraInfo("exp-val-z", e);
      buffer->appendChild(obsCircuits[i]->name(), tmpBuffer);
    }
  }
}

double QsimAccelerator::getExpectationValueZ(
    std::shared_ptr<CompositeInstruction> compositeInstruction,
    const StateSpace &stateSpace, const State &state) const {
  // Construct Qsim circuit:
  QsimCircuitVisitor visitor(state.num_qubits());
  std::vector<size_t> measureBitIdxs;
  // Walk the IR tree, and visit each node
  InstructionIterator it(compositeInstruction);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      if (!isMeasureGate(nextInst)) {
        nextInst->accept(&visitor);
      } else {
        // Just collect the indices of measured qubit
        measureBitIdxs.emplace_back(nextInst->bits()[0]);
      }
    }
  }

  auto circuit = visitor.getQsimCircuit();
  if (!circuit.gates.empty()) {
    auto scratch = stateSpace.Create(state.num_qubits());
    // copy from src to scratch.
    const bool copyOk = stateSpace.Copy(state, scratch);
    assert(copyOk);
    auto fused_circuit =
        Fuser::FuseGates(m_qsimParam, circuit.num_qubits, circuit.gates);
    xacc::quantum::QsimAccelerator::Simulator sim(m_numThreads);
    for (const auto &fused_gate : fused_circuit) {
      qsim::ApplyFusedGate(sim, fused_gate, scratch);
    }
    return computeExpValZ(m_qsimParam, m_numThreads, measureBitIdxs, stateSpace, scratch);
  } else {
    return computeExpValZ(m_qsimParam, m_numThreads, measureBitIdxs, stateSpace, state);
  }
}

// Sync. (FTQC) gate application.
void QsimAccelerator::apply(std::shared_ptr<AcceleratorBuffer> buffer,
                            std::shared_ptr<Instruction> inst) {
  static std::shared_ptr<AcceleratorBuffer> current_buffer;
  static std::unique_ptr<QsimCircuitVisitor> visitor;
  static StateSpace stateSpace(m_numThreads);
  static std::optional<State> current_state;
  xacc::info("Apply: " + inst->toString());
  if (!current_buffer || (current_buffer->name() != buffer->name())) {
    current_buffer = buffer;
    visitor = std::make_unique<QsimCircuitVisitor>(buffer->size());
    current_state = stateSpace.Create(buffer->size());
    stateSpace.SetStateZero(*current_state);
  }

  assert(!inst->isComposite());
  inst->accept(visitor.get());
  if (isMeasureGate(inst)) {
    auto qsimCirc = visitor->getQsimCircuit();
    auto scratch = stateSpace.Create(current_state->num_qubits());
    // copy from src to scratch.
    const bool copyOk = stateSpace.Copy(*current_state, scratch);
    assert(copyOk);
    std::vector<StateSpace::MeasurementResult> meas_results;
    // std::cout << "Before: \n";
    // PrintAmplitudes(scratch.num_qubits(), stateSpace, scratch);
    m_qsimParam.seed = time(NULL);
    const bool runOk =
        Runner::Run(m_qsimParam, Factory(m_numThreads), qsimCirc, scratch, meas_results);
    assert(runOk);
    if (meas_results.size() == 1 && meas_results[0].bitstring.size() == 1) {
      const auto bitResult = meas_results[0].bitstring[0];
      assert(bitResult == 0 || bitResult == 1);
      buffer->measure(inst->bits()[0], bitResult);
    } else {
      xacc::error("Unexpected measurement results encountered.");
    }

    // std::cout << "After: \n";
    // PrintAmplitudes(scratch.num_qubits(), stateSpace, scratch);

    // Update the state:
    stateSpace.Copy(scratch, *current_state);

    // Make new visitor, i.e. clear the circuit.
    visitor = std::make_unique<QsimCircuitVisitor>(buffer->size());
  }
}
} // namespace quantum
} // namespace xacc

REGISTER_ACCELERATOR(xacc::quantum::QsimAccelerator)
