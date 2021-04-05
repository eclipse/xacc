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
#include "CircuitOptimizer.hpp"
#include "Graph.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "CommonGates.hpp"
#include "Circuit.hpp"
#include "GateIR.hpp"
#include "Utils.hpp"
#include "xacc_service.hpp"
#include "xacc.hpp"
#include <assert.h>
#include "PhasePolynomialRepresentation.hpp"
#include <regex>

namespace {
  // Convert InstructionParameter (i.e. a variant) to double,
  // return 0.0 if it's not convertible to a floating point number.
  inline double ipToDouble(const xacc::InstructionParameter& in_param) {
    if (in_param.which() == 0) {
      return (double)in_param.as<int>();
    } else if (in_param.which() == 1) {
      return in_param.as<double>();
    } else {
      std::stringstream s;
      s << in_param.toString();
      xacc::error("CircuitOptimizer: invalid gate parameter " +
                  std::to_string(in_param.which()) + ", " + s.str());
    }
    return 0.0;
  };

  // Returns the rotation angle in the [-pi, pi) range
  inline double getNormalizedRotationAngle(double in_angle) {
    // Modulo by 2*pi
    double modAngle = fmod(in_angle, 2.0 * M_PI);
    modAngle = fmod(modAngle + 2.0 * M_PI, 2.0 * M_PI);
    // Normalize the angle in the (-pi, pi] range:
    return (modAngle <= M_PI) ? modAngle : modAngle - 2.0 * M_PI;
  }

  const double ANGLE_EPS_RAD = 1e-12;

  inline bool isPiOver2(double in_angle)
  {
    assert(in_angle <= M_PI && in_angle >= -M_PI);
    return fabs(in_angle - M_PI_2) < ANGLE_EPS_RAD;
  }

  inline bool isMinusPiOver2(double in_angle)
  {
    assert(in_angle <= M_PI && in_angle >= -M_PI);
    return fabs(in_angle + M_PI_2) < ANGLE_EPS_RAD;
  }
} // anon namespace

namespace xacc {
namespace quantum {

void CircuitOptimizer::apply(std::shared_ptr<CompositeInstruction> gateFunction,
                             const std::shared_ptr<Accelerator> accelerator,
                             const HeterogeneousMap &options) {

  tryPermuteAndCancelXGate(gateFunction);
  tryReduceHadamardGates(gateFunction);
  tryRotationMergingUsingPhasePolynomials(gateFunction);

  auto isRotation = [](const std::string inst) {
    return inst == "Rz" || inst == "Ry" || inst == "Rx";
  };

  auto ipToDouble = [](xacc::InstructionParameter p) {
    if (p.which() == 0) {
      return (double)p.as<int>();
    } else if (p.which() == 1) {
      return p.as<double>();
    } else {
      std::stringstream s;
      s << p.toString();
      xacc::error("CircuitOptimizer: invalid gate parameter " +
                  std::to_string(p.which()) + ", " + s.str());
    }
    return 0.0;
  };

  int nTries = 2;

  //   for (auto &k : gateir->getComposites()) {
  for (int j = 0; j < nTries; ++j) {

    // Remove any zero rotations (Rz(theta<1e-12))
    for (int i = 0; i < gateFunction->nInstructions(); i++) {
      auto inst = gateFunction->getInstruction(i);
      if (isRotation(inst->name())) {
        auto param = inst->getParameter(0);
        if (!param.isVariable()) {
          double val = ipToDouble(param);
          if (std::fabs(val) < 1e-12) {
            inst->disable();
          }
        } else {
            // Check for 0 * t or things like that
            auto parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
            auto expr = param.toString();
            auto split = xacc::split(expr, '*');
            if (split.size() == 2) {
                xacc::trim(split[0]);
                xacc::trim(split[1]);
                auto coeff_str = split[0];
                double d;
                bool is_constant = parsingUtil->isConstant(coeff_str, d);
                if (is_constant && std::fabs(d) < 1e-12 && xacc::container::contains(gateFunction->getVariables(), split[1])) {
                    // then this is 0 * var, can remove
                    inst->disable();
                }
            }
        }
      }
    }
    
    // Remove all CNOT(p,q) CNOT(p,q) pairs
    while (true) {
      bool modified = false;
      gateFunction->removeDisabled();
      // gateFunction = std::dynamic_pointer_cast<Circuit>(
      // gateFunction->enabledView());
      auto graphView = gateFunction->toGraph();
      for (int i = 1; i < graphView->order() - 2; i++) {
        auto node = graphView->getVertexProperties(i);
        if (node.getString("name") == "CNOT" &&
            gateFunction->getInstruction(node.get<std::size_t>("id") - 1)
                ->isEnabled()) {
          auto nAsVec = graphView->getNeighborList(node.get<std::size_t>("id"));
          // std::vector<int> nAsVec(neighbors.begin(), neighbors.end());
          // Note: There is an edge-case if the CNOT is last gate on a pair of qubit wires,
          // i.e. both of its neighbors will be the final state node.
          // In that case, we need to skip.
          if (nAsVec[0] == nAsVec[1] && nAsVec[0] != graphView->order() - 1) {
            // Check that the neighbor gate is indeed a CNOT gate, i.e. not a different 2-qubit gate.
            if (gateFunction->getInstruction(nAsVec[0] - 1)->name() == "CNOT") {
              gateFunction->getInstruction(node.get<std::size_t>("id") - 1)->disable();
              gateFunction->getInstruction(nAsVec[0] - 1)->disable();
              modified = true;
              break;
            }
          }
        }
      }
      if (!modified)
        break;
    }
    // Remove all H(p)H(p) pairs
    while (true) {
      bool modified = false;
      gateFunction->removeDisabled();
      // gateFunction = std::dynamic_pointer_cast<Circuit>(
      // gateFunction->enabledView());
      auto graphView = gateFunction->toGraph();

      for (int i = 1; i < graphView->order() - 2; ++i) {
        auto node = graphView->getVertexProperties(i);

        auto nAsVec = graphView->getNeighborList(node.get<std::size_t>("id"));
        //   std::vector<int> nAsVec(adj.begin(), adj.end());
        if (nAsVec.size() == 1) {
          auto nextNode = graphView->getVertexProperties(nAsVec[0]);
          if (node.getString("name") == "H" &&
              nextNode.getString("name") == "H") {
            gateFunction->getInstruction(node.get<std::size_t>("id") - 1)->disable();
            gateFunction->getInstruction(nAsVec[0] - 1)->disable();
            modified = true;
            break;
          }
        }
      }

      if (!modified)
        break;
    }
    // Merge adjacent rotation gates Rz()Rz() or Rx()Rx() or Ry()Ry()
    while (true) {
      bool modified = false;
      gateFunction->removeDisabled();

      // gateFunction =
      // std::dynamic_pointer_cast<Circuit>(gateFunction->enabledView());
      auto graphView = gateFunction->toGraph();
      // start/end nodes get added: start at 1, stop at n-2 b/c last will be n-2
      // neighbor
      for (int i = 1; i < graphView->order() - 2; i++) {
        auto node = graphView->getVertexProperties(i);
        auto nAsVec = graphView->getNeighborList(node.get<std::size_t>("id"));
        // if it has more than 1 neighbor, don't consider
        if (nAsVec.size() == 1) {
          auto nextNode = graphView->getVertexProperties(nAsVec[0]);
          if (isRotation(node.getString("name")) &&
              isRotation(nextNode.getString("name")) &&
              node.getString("name") == nextNode.getString("name")) {
            auto val1 =
                ipToDouble(gateFunction->getInstruction(node.get<std::size_t>("id") - 1)
                               ->getParameter(0));
            auto val2 = ipToDouble(
                gateFunction->getInstruction(nextNode.get<std::size_t>("id") - 1)
                    ->getParameter(0));

            if (std::fabs(val1 + val2) < 1e-12) {
              gateFunction->getInstruction(node.get<std::size_t>("id") - 1)->disable();
              gateFunction->getInstruction(nextNode.get<std::size_t>("id") - 1)
                  ->disable();
            } else {
              InstructionParameter tmp(val1 + val2);
              gateFunction->getInstruction(node.get<std::size_t>("id") - 1)
                  ->setParameter(0, tmp);
              gateFunction->getInstruction(nextNode.get<std::size_t>("id") - 1)
                  ->disable();
            }
            modified = true;
            break;
          }
        }
      }

      if (!modified)
        break;
    }
  }
  //   }
  return;
}

bool CircuitOptimizer::tryPermuteAndCancelXGate(std::shared_ptr<CompositeInstruction>& io_program) {
  // ============   TODO ====================
  // (1) Technically, this procedure can be done in parallel, 1 thread for each qubit wire.
  // (2) We can also push X gate through CCNOT (Toffoli) gates,
  // but because we don't support CCNOT gates atm, hence skip.
  // (3) If we support Toffoli gates, then we can push X gate through one of the two *control* qubits as well
  // and *negate* that control line. Then, the negated control can be further optimized during decomposition.
  // See Section 4.3 of https://arxiv.org/pdf/1710.07345.pdf for details.
  // ========================================
  std::vector<int> removedInstructionIdx;
  for (int i = 0; i < io_program->nInstructions(); ++i) {
    const auto& inst = io_program->getInstruction(i);
    if (inst->name() == "X") {
      if (container::contains(removedInstructionIdx, i)) {
        // If this X gate has been marked for removal (by looking ahead from a previous X gate),
        // just skip it and continue to the next gate.
        continue;
      }

      const auto qubitIdx = inst->bits()[0];
      for (int lookAheadIdx = i + 1; lookAheadIdx < io_program->nInstructions(); ++lookAheadIdx) {
        const auto& nextInst = io_program->getInstruction(lookAheadIdx);
        if (nextInst->name() == "X" && nextInst->bits()[0] == qubitIdx) {
          // Found an adjacent X after permutation
          // Cancel both of them and break the look-ahead loop
          removedInstructionIdx.emplace_back(i);
          removedInstructionIdx.emplace_back(lookAheadIdx);
          break;
        }

        if ((nextInst->name() == "I") ||
            !container::contains(nextInst->bits(), qubitIdx) ||
            (nextInst->name() == "CNOT" &&  nextInst->bits()[1] == qubitIdx)) {
              continue;
        } else {
          // we cannot move any further, break the look-ahead loop.
          break;
        }
      }
    }
  }
  // There must be pairs of X gates.
  assert(removedInstructionIdx.size() % 2 == 0);
  // This list must only contain unique indices.
  assert(std::unique(removedInstructionIdx.begin(), removedInstructionIdx.end()) == removedInstructionIdx.end());
  // Sort the list of instruction indices before removal (left to right)
  std::sort(removedInstructionIdx.begin(), removedInstructionIdx.end());
  // Remove those X instructions from the circuit
  for (int i = 0; i < removedInstructionIdx.size(); ++i) {
    // Since we are removing one instruction at a time,
    // the indices will be shifted by one after each removal.
    // Hence, adjust the index accordingly.
    const int instIndexToRemove = removedInstructionIdx[i] - i;
    assert(io_program->getInstruction(instIndexToRemove)->name() == "X");
    io_program->removeInstruction(instIndexToRemove);
  }
  // If we have found and removed some redundant X instructions, returns true.
  return !removedInstructionIdx.empty();
}

bool CircuitOptimizer::tryReduceHadamardGates(std::shared_ptr<CompositeInstruction>& io_program) {
  // Algorithm: we iterate the circuit until we hit an Hadamard gate,
  // then check if the sequence is one of those in Figure 4 of https://arxiv.org/pdf/1710.07345.pdf
  auto graphView = io_program->toGraph();
  std::vector<std::size_t> hadamardNodeIds;
  // We collect all Hadamard nodes ahead of time for the iteration and matching
  for (int i = 1; i < graphView->order() - 1; ++i) {
    const auto node = graphView->getVertexProperties(i);
    if (node.getString("name") == "H") {
      hadamardNodeIds.emplace_back(node.get<std::size_t>("id"));
    }
  }

  std::vector<std::vector<std::size_t>> matchedReductionPatterns;
  // Set of Hadamard node Ids that have already been matched against a pattern,
  // hence no need to match any more to prevent double matching.
  // The below matching is constructed to prioritize patterns that result in more
  // gate count reduction, hence we want to keep track of which H gates have already been matched.
  std::unordered_set<std::size_t> matchedHadamardNodeIds;

  for (const auto& hadamardNode: hadamardNodeIds) {
    if (container::contains(matchedHadamardNodeIds, hadamardNode)) {
      continue;
    }

    const auto& hadamardInst = io_program->getInstruction(hadamardNode - 1);
    assert(hadamardInst->bits().size() == 1);
    const auto qubitIndex = hadamardInst->bits().front();
    const auto neighborNodes = graphView->getNeighborList(hadamardNode);
    // This is a single-qubit gate, hence must have only one neighbor
    // (either another gate or the final state)
    assert(neighborNodes.size() == 1);
    const auto nextNode = graphView->getVertexProperties(neighborNodes.front());
    const auto nextGateName = nextNode.getString("name");

    if (nextGateName == "CNOT") {
      // We try to match against this gate pattern:
      // H --------- H
      //       |
      //       |
      // H-----+-----H
      const auto cnotInst = io_program->getInstruction(nextNode.get<std::size_t>("id") - 1);
      const auto cnotNeighborNodes = graphView->getNeighborList(nextNode.get<std::size_t>("id"));
      if (cnotNeighborNodes.size() == 2) {
        if (container::contains(hadamardNodeIds, cnotNeighborNodes[0]) && container::contains(hadamardNodeIds, cnotNeighborNodes[1])) {
          // Try to find the remaining left leg
          std::size_t remainingHadamardNodeId = 0;
          for (const auto& checkNode: hadamardNodeIds) {
            if (checkNode != hadamardNode && graphView->getNeighborList(checkNode)[0] == nextNode.get<std::size_t>("id")) {
              remainingHadamardNodeId = checkNode;
              break;
            }
          }

          if (remainingHadamardNodeId != 0) {
            // We've found the complete pattern.
            matchedReductionPatterns.emplace_back(std::vector<std::size_t>({ hadamardNode, remainingHadamardNodeId, nextNode.get<std::size_t>("id"), (std::size_t) cnotNeighborNodes[0], (std::size_t) cnotNeighborNodes[1] }));
            // Add all 4 hadamard gates to the tracking list
            matchedHadamardNodeIds.emplace(hadamardNode);
            matchedHadamardNodeIds.emplace(remainingHadamardNodeId);
            matchedHadamardNodeIds.emplace(cnotNeighborNodes[0]);
            matchedHadamardNodeIds.emplace(cnotNeighborNodes[1]);
          }
        }
      }
    }

    if (nextGateName == "Rz")
    {
      assert(io_program->getInstruction(nextNode.get<std::size_t>("id") - 1)->bits()[0] == qubitIndex);
      // We only match Rz(+/- pi/2), i.e. the Phase gate and its dagger.

      // Check if total angle is either +/- pi/2, i.e. equivalent to a P gate or its dagger.
      const double rawAngle = ipToDouble(io_program->getInstruction(nextNode.get<std::size_t>("id") - 1)->getParameter(0));
      const double normalizedAngle = getNormalizedRotationAngle(rawAngle);
      const bool isPhaseGate = isPiOver2(normalizedAngle);
      const bool isPhaseDaggerGate = isMinusPiOver2(normalizedAngle);

      if (isPhaseGate || isPhaseDaggerGate) {
        // There are two potential patterns that we need to check:
        // (1) H - P - H (or P dagger)
        // (2) H - P - CNOT^k - P - H (or P dagger)
        const auto phaseGateNeighborNodeIds = graphView->getNeighborList(nextNode.get<std::size_t>("id"));
        assert(phaseGateNeighborNodeIds.size() == 1);
        const auto phaseGateNeighborNode = graphView->getVertexProperties(phaseGateNeighborNodeIds.front());
        if (phaseGateNeighborNode.getString("name") == "H") {
          // Got it, this is the H - P - H (or P dagger)
          assert(container::contains(hadamardNodeIds, phaseGateNeighborNode.get<std::size_t>("id")));
          matchedReductionPatterns.emplace_back(std::vector<std::size_t>({ hadamardNode, nextNode.get<std::size_t>("id"), phaseGateNeighborNode.get<std::size_t>("id") }));
          // Add the two Hadamard gates to the tracking list
          matchedHadamardNodeIds.emplace(hadamardNode);
          matchedHadamardNodeIds.emplace(phaseGateNeighborNode.get<std::size_t>("id"));
        }

        // Lastly, try match against the H - P - CNOT^k - P - H (or P dagger) pattern
        // where any number of CNOT gates can be accepted as long as the *target* qubit is on this qubit wire.
        // Helper lambda to check if the next node (from a startNodeId) is a CNOT whose target qubit is the one specified.
        // If so, returns true and append the CNOT node to the node Id vector.
        const auto isNextNodeCnotTargetQubit = [&graphView, &io_program](int in_qubitIndex, int in_startNodeId, std::vector<int>& io_cnotNodeIdToAppend) ->bool {
          const auto neighbors = graphView->getNeighborList(in_startNodeId);
          for (const auto& neighbor: neighbors) {
            const auto nodeProps = graphView->getVertexProperties(neighbor);
            const auto instIdx = nodeProps.get<std::size_t>("id") - 1;
            if (instIdx < io_program->nInstructions()) {
              const auto inst = io_program->getInstruction(instIdx);
              if (inst->name() == "CNOT" && inst->bits()[1] == in_qubitIndex) {
                io_cnotNodeIdToAppend.emplace_back(neighbor);
                return true;
              }
            }
          }
          return false;
        };
        std::vector<int> cnotNodeIdx;
        // We start from the Phase gate node
        int startVerticeId = nextNode.get<std::size_t>("id");
        while (true) {
          if (isNextNodeCnotTargetQubit(qubitIndex, startVerticeId, cnotNodeIdx)) {
            // If found a CNOT gate (target the correct qubit), continue from that CNOT
            startVerticeId = cnotNodeIdx.back();
          } else {
            break;
          }
        }

        // We have found the correct H-P-CNOT^k pattern:
        if (!cnotNodeIdx.empty()) {
          // Since a CNOT will normally connect to 2 nodes, we need to check both nodes after CNOT match the expected pattern,
          // return the node Id if found, otherwise, returns 0
          const auto matchingRzNodeIdAfterCnot = [&]() -> int {
            // Need to check the remaining pattern: Rz(+/-pi/2) then H
            const auto nodesAfterLastCnot = graphView->getNeighborList(cnotNodeIdx.back());
            for (const auto& nodeToCheck: nodesAfterLastCnot) {
              const auto nodeProp = graphView->getVertexProperties(nodeToCheck);
              if (nodeProp.getString("name") == "Rz") {
                // Must be Rz on that qbit line
                if(io_program->getInstruction(nodeToCheck - 1)->bits()[0] == qubitIndex) {
                  // This Rz (after CNOT's) must match the one before CNOT's,
                  // i.e. if it was Pi/2 (P) before, it must be -Pi/2 here, and vice versa.
                  if (( isPhaseGate && isMinusPiOver2(getNormalizedRotationAngle(ipToDouble(io_program->getInstruction(nodeToCheck - 1)->getParameter(0))))) ||
                      ( isPhaseDaggerGate && isPiOver2(getNormalizedRotationAngle(ipToDouble(io_program->getInstruction(nodeToCheck - 1)->getParameter(0)))))) {
                    // Everything is good, we have found the matching Rz as a neighbor of the CNOT
                    return nodeToCheck;
                  }
                }
              }
            }
            // Cannot find the matching Rz pattern in any of the two CNOT gate's neighbor
            return 0;
          }();

          if (matchingRzNodeIdAfterCnot != 0) {
            // Check the last H gate of the pattern
            const auto lastNodeToCheck = graphView->getNeighborList(matchingRzNodeIdAfterCnot)[0];
            if (container::contains(hadamardNodeIds, lastNodeToCheck)) {
              // Found the complete pattern
              // Sanity check: it is indeed an Hadamard on the same qubit line
              assert(io_program->getInstruction(lastNodeToCheck - 1)->bits()[0] == qubitIndex);
              std::vector<std::size_t> completePattern;
              // First H
              completePattern.emplace_back(hadamardNode);
              // First Phase gate (or phase dagger)
              completePattern.emplace_back(nextNode.get<std::size_t>("id"));
              // Sequence of CNOT gates
              for (const auto& cnotId : cnotNodeIdx) {
                completePattern.emplace_back(cnotId);
              }
              // Matching Phase/Phase dagger gate
              completePattern.emplace_back(matchingRzNodeIdAfterCnot);
              // Last H gate
              completePattern.emplace_back(lastNodeToCheck);

              matchedReductionPatterns.emplace_back(std::move(completePattern));

              // Add the two hadamard nodes to the list
              matchedHadamardNodeIds.emplace(hadamardNode);
              matchedHadamardNodeIds.emplace(lastNodeToCheck);
            }
          }
        }
      }
    }
  }

  // Circuit rewrite:
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  for (const auto& matchedPattern : matchedReductionPatterns) {
    if (matchedPattern.size() == 3) {
      // First pattern: H - P - H => P_dagger - H - P_dagger; or vice-versa
      const auto phaseGate = io_program->getInstruction(matchedPattern[1] - 1);
      if(isPiOver2(getNormalizedRotationAngle(ipToDouble(phaseGate->getParameter(0))))) {
        io_program->replaceInstruction(matchedPattern[0] - 1,
          gateRegistry->createInstruction("Rz", phaseGate->bits(), { -M_PI_2 }));
        io_program->replaceInstruction(matchedPattern[1] - 1,
          gateRegistry->createInstruction("H", phaseGate->bits()));
        io_program->replaceInstruction(matchedPattern[2] - 1,
          gateRegistry->createInstruction("Rz", phaseGate->bits(), { -M_PI_2 }));
      }
      else {
        io_program->replaceInstruction(matchedPattern[0] - 1,
          gateRegistry->createInstruction("Rz", phaseGate->bits(), { M_PI_2 }));
        io_program->replaceInstruction(matchedPattern[1] - 1,
          gateRegistry->createInstruction("H", phaseGate->bits()));
        io_program->replaceInstruction(matchedPattern[2] - 1,
          gateRegistry->createInstruction("Rz", phaseGate->bits(), { M_PI_2 }));
      }
    } else if (matchedPattern.size() == 5 && container::contains(hadamardNodeIds, matchedPattern[0]) && container::contains(hadamardNodeIds, matchedPattern[1])) {
      // Pattern: H - H - CNOT - H - H pattern
      // Remove all four H gates and invert the CNOT
      io_program->getInstruction(matchedPattern[0] - 1)->disable();
      io_program->getInstruction(matchedPattern[1] - 1)->disable();

      {
        // Handle the middle CNOT gate
        auto cnotQubits = io_program->getInstruction(matchedPattern[2] - 1)->bits();
        assert(cnotQubits.size() == 2);
        std::reverse(cnotQubits.begin(), cnotQubits.end());
        io_program->replaceInstruction(matchedPattern[2] - 1,
          gateRegistry->createInstruction("CX", cnotQubits));
      }

      io_program->getInstruction(matchedPattern[3] - 1)->disable();
      io_program->getInstruction(matchedPattern[4] - 1)->disable();
    } else {
      // Last case: H-P-CNOT-P_dag-H
      assert(matchedPattern.size() >= 5);
      // We just need to remove the leading and trailing H gates then inverse the normalized Rz phase.
      assert(io_program->getInstruction(matchedPattern.front() - 1)->name() == "H");
      assert(io_program->getInstruction(matchedPattern.back() - 1)->name() == "H");
      io_program->getInstruction(matchedPattern.front() - 1)->disable();
      io_program->getInstruction(matchedPattern.back() - 1)->disable();
      // Inverse the normalized rotation phase: P->P_dag and vice versa
      auto headRz = io_program->getInstruction(matchedPattern[1] - 1);
      auto tailRz = io_program->getInstruction(matchedPattern[matchedPattern.size() - 2]- 1);
      assert(headRz->name() == "Rz" && tailRz->name() == "Rz");
      const auto headRzAngle = getNormalizedRotationAngle(ipToDouble(headRz->getParameter(0)));
      const auto tailRzAngle = getNormalizedRotationAngle(ipToDouble(tailRz->getParameter(0)));
      // They must have opposite side (+/- pi/2)
      assert(fabs(headRzAngle + tailRzAngle) < ANGLE_EPS_RAD);
      // Flip the angles
      headRz->setParameter(0, -headRzAngle);
      tailRz->setParameter(0, -tailRzAngle);
    }
  }

  // Remove all redundant gates that we've disabled earlier during circuit rewriting.
  io_program->removeDisabled();

  return !matchedReductionPatterns.empty();
}

bool CircuitOptimizer::tryRotationMergingUsingPhasePolynomials(std::shared_ptr<CompositeInstruction>& io_program) {
  auto graphView = io_program->toGraph();
  // The list of graph node Ids on each qubit wire.
  // (to handle backward search from a CNOT anchor point)
  std::unordered_map<int, std::vector<int>> qubitToNodeIds;
  for (int i = 1; i < graphView->order() - 1; ++i) {
    const auto node = graphView->getVertexProperties(i);
    const auto& instruction = io_program->getInstruction(node.get<std::size_t>("id") - 1);
    for (const auto& bit: instruction->bits()) {
      auto& currentList = qubitToNodeIds[bit];
      // Add the node Id to the tracking list of this qubit.
      currentList.emplace_back(node.get<std::size_t>("id"));
    }
  }

  for (int i = 1; i < graphView->order() - 1; /* increment in loop*/) {
    std::unordered_map<int, std::pair<int, int>> qubitToBoundary;
    const auto node = graphView->getVertexProperties(i);
    // Starting at a CNOT gate as anchor point
    if (node.getString("name") == "CNOT") {
      const auto& cnotInst = io_program->getInstruction(node.get<std::size_t>("id") - 1);
      assert(cnotInst->bits().size() == 2);
      const auto ctrlIndex = cnotInst->bits()[0];
      const auto targetIndex = cnotInst->bits()[1];

      const auto findLeftBoundary = [&](int in_qbitIdx, int in_nodeId) -> int {
        // Get the list of gates on this wire (up to this point)
        const auto& nodesOnWire = qubitToNodeIds[in_qbitIdx];
        // reverse iteration
        for (auto it = nodesOnWire.rbegin(); it != nodesOnWire.rend(); ++it) {
          if (*it >= in_nodeId) {
            continue;
          }
          const auto& inst = io_program->getInstruction(*it - 1);
          if (inst->name() != "Rz" && inst->name() != "X") {
            // It is no longer { X, Rz, CNOT } sub-circuit
            return *it;
          }
        }
        // No left boundary, from the start of the circuit.
        return 0;
      };

      // Backward search
      {
        qubitToBoundary[ctrlIndex] = std::make_pair(findLeftBoundary(ctrlIndex, i), graphView->order());
        qubitToBoundary[targetIndex] = std::make_pair(findLeftBoundary(targetIndex, i), graphView->order());
      }

      // Helper to find the right boundary
      const std::function<void(int, int)> findRightBoundary = [&](int in_qbitIdx, int in_startNodeId) -> void {
        if (in_startNodeId < graphView->order() - 1) {
          const auto& inst = io_program->getInstruction(in_startNodeId - 1);
          if (inst->name() == "Rz" || inst->name() == "X") {
            assert(inst->bits().size() == 1 && inst->bits()[0] == in_qbitIdx);
            const auto subsequentNodes = graphView->getNeighborList(in_startNodeId);
            assert(subsequentNodes.size() == 1);
            // Traverse to the right
            assert(inst->bits()[0] == in_qbitIdx);
            return findRightBoundary(inst->bits()[0], subsequentNodes[0]);
          } else if (inst->name() == "CNOT") {
            for (const auto& bit: inst->bits()) {
              const auto neighborNodes = graphView->getNeighborList(in_startNodeId);
              if (qubitToBoundary.find(bit) == qubitToBoundary.end()) {
                // No left boundary yet on this qubit line
                qubitToBoundary[bit] = std::make_pair(findLeftBoundary(bit, in_startNodeId), graphView->order());
                for (const auto& neighbor: neighborNodes) {
                  if (neighbor < graphView->order() - 1 && container::contains(io_program->getInstruction(neighbor - 1)->bits(), bit)) {
                    findRightBoundary(bit, neighbor);
                  }
                }
              }
              // Traverse the neighbor associated with this qubit wire
              for (const auto& neighbor: neighborNodes) {
                if (neighbor < graphView->order() - 1 && container::contains(io_program->getInstruction(neighbor - 1)->bits(), in_qbitIdx)) {
                  // If it's still within the boundary, explore further
                  // Otherwise, the sub-circuit has had termination point on this qubit wire.
                  if (neighbor < qubitToBoundary[in_qbitIdx].second) {
                    return findRightBoundary(in_qbitIdx, neighbor);
                  }
                }
              }
            }
          }
        }
        // End of the sub-circuit: add the right boundary if not having one.
        if (in_startNodeId < qubitToBoundary[in_qbitIdx].second) {
          qubitToBoundary[in_qbitIdx].second = in_startNodeId;
        }
      };

      // Forward search:
      {
        // Control wire:
        for (const auto& nodeToCheck: graphView->getNeighborList(node.get<std::size_t>("id"))) {
          if (nodeToCheck < graphView->order() - 1 && container::contains(io_program->getInstruction(nodeToCheck - 1)->bits(), ctrlIndex)) {
            findRightBoundary(ctrlIndex, nodeToCheck);
          }
        }
        // Target wire"
        for (const auto& nodeToCheck: graphView->getNeighborList(node.get<std::size_t>("id"))) {
          if (nodeToCheck < graphView->order() - 1 && container::contains(io_program->getInstruction(nodeToCheck - 1)->bits(), targetIndex)) {
            findRightBoundary(targetIndex, nodeToCheck);
          }
        }
      }
      // End sub-circuit finding for this CNOT
      // Sub-circuit pruning
      std::vector<std::shared_ptr<Instruction>> subCircuit;
      {
        std::vector<int> qubits;
        for (const auto& kv: qubitToBoundary) {
          qubits.emplace_back(kv.first);
          std::stringstream ss;
          ss << "Q" << kv.first << ": " << kv.second.first << " --> " << kv.second.second;
          xacc::info(ss.str());
        }

        for (int idx = 1; idx < graphView->order() - 1; ++idx) {
          const auto node = graphView->getVertexProperties(idx);
          const auto& instruction = io_program->getInstruction(node.get<std::size_t>("id") - 1);

          if (instruction->bits().size() == 1 && container::contains(qubits, instruction->bits()[0])) {
            const auto& boundary = qubitToBoundary[instruction->bits()[0]];
            if (idx > boundary.first && idx < boundary.second) {
              subCircuit.emplace_back(instruction);
            }
          }
          else if (instruction->bits().size() == 2) {
            assert(instruction->name() == "CNOT");
            // If the control is *outside* the boundary, we need to terminate, hence prune the subcircuit.
            const auto controlIdx = instruction->bits()[0];
            const auto targetIdx = instruction->bits()[1];
            if (!container::contains(qubits, controlIdx) && (qubitToBoundary.find(targetIdx) != qubitToBoundary.end())) {
              auto& boundary = qubitToBoundary[targetIdx];
              boundary.second = idx;
            }
            else {
              subCircuit.emplace_back(instruction);
              // Move the outter loop counter pass this CNOT gate
              // since it's already included in this subcircuit (via forward search).
              i = idx + 1;
            }
          }
        }

        int countRz = 0;
        for (const auto& inst: subCircuit) {
          if (inst->name() == "Rz") {
            countRz++;
          }
        }
        // If the subcircuit has less than 2 Rz gates, then nothing we can optimize here.
        if (countRz >= 2) {
          std::stringstream ss;
          ss << "Found sub-circuit:\n";
          for (const auto &inst : subCircuit) {
            ss << inst->toString() << "\n";
          }
          xacc::info(ss.str());

          PhasePolynomialRep phasePolynomialRep(qubitToNodeIds.size(), subCircuit);
          if (phasePolynomialRep.hasMergableGates()) {
            for (const auto& gate: subCircuit) {
              if (gate->name() == "Rz" && gate->isEnabled()) {
                const auto& affineFunc = phasePolynomialRep.getAffineFunctionOfRzGate(gate.get());
                const auto& gatesThatHaveSameAffineFunc = phasePolynomialRep.getRzGatesWithAffineFunction(affineFunc);
                if (gatesThatHaveSameAffineFunc.size() > 1) {
                  double totalAngle = ipToDouble(gate->getParameter(0));
                  for (const auto& inst: gatesThatHaveSameAffineFunc) {
                    if (inst != gate.get()) {
                      totalAngle += ipToDouble(inst->getParameter(0));
                      // Disable gates that were merged for later removal.
                      inst->disable();
                    }
                  }

                  assert(gate->isEnabled());
                  // Combine the angle to the first Rz gate of this affine function.
                  gate->setParameter(0, totalAngle);
                }
              }
            }
          }
        }
      }
    }
    else {
      // Not a CNOT gate, continue
      ++i;
    }
  }

  // Remove those gates that were merged.
  io_program->removeDisabled();

  return true;
}
} // namespace quantum
} // namespace xacc