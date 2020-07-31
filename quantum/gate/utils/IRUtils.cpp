/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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

#include "IRUtils.hpp"
#include "CompositeInstruction.hpp"
#include "InstructionIterator.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"
#include <cassert>
namespace {
// Computes the base length of a Composite:
// i.e. not include measure gates.
size_t
getBaseLength(const std::shared_ptr<xacc::CompositeInstruction> &in_composite) {
  size_t length = 0;
  xacc::InstructionIterator it(in_composite);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled() && !nextInst->isComposite()) {
      if (nextInst->name() == "Measure") {
        return length;
      }
      length++;
    }
  }
  return length;
}

bool compareInst(xacc::InstPtr in_a, xacc::InstPtr in_b) {
  if (!in_a || !in_b) {
    return false;
  }
  // We should only compare elementary instructions.
  assert(!in_a->isComposite() && !in_b->isComposite());
  assert(in_a->isEnabled() && in_b->isEnabled());
  return (in_a->name() == in_b->name()) && (in_a->bits() == in_b->bits()) &&
         (in_a->getParameters() == in_b->getParameters());
}
// Helper to pop the instruction stack (so that we can walk both trees
// simultaneously)
xacc::InstPtr getNextInstruction(xacc::InstructionIterator &iter) {
  while (iter.hasNext()) {
    auto nextInst = iter.next();
    if (nextInst->isEnabled() && !nextInst->isComposite()) {
      return nextInst;
    }
  }
  return nullptr;
}
} // namespace
namespace xacc {
namespace quantum {
ObservedAnsatz ObservedAnsatz::fromObservedComposites(
    const std::vector<std::shared_ptr<CompositeInstruction>> &in_composites) {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto baseComposite = gateRegistry->createComposite("__BASE_COMPOSITE__");
  ObservedAnsatz result;

  // If there is only one composite.
  if (in_composites.size() == 1) {
    // Just use an empty base,
    // i.e. the only circuit is the observable circuit.
    result.m_baseAnsatz = baseComposite;
    result.m_obsCircuits = {in_composites[0]};
    return result;
  }

  std::shared_ptr<CompositeInstruction> shortestComposite = in_composites[0];
  // Find the shortest composite (not include the measure part)
  for (size_t i = 1; i < in_composites.size(); ++i) {
    if (getBaseLength(in_composites[i]) < getBaseLength(shortestComposite)) {
      shortestComposite = in_composites[i];
    }
  }

  InstructionIterator it(shortestComposite);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled() && !nextInst->isComposite()) {
      if (nextInst->name() == "Measure") {
        break;
      }
      baseComposite->addInstruction(nextInst->clone());
    }
  }

  // Check if a composite is the base of the other
  const auto isBaseOf =
      [](const std::shared_ptr<CompositeInstruction> &in_base,
         const std::shared_ptr<CompositeInstruction> &in_toTest,
         InstructionIterator &out_Iter) {
        assert(getBaseLength(in_base) <= getBaseLength(in_toTest));
        bool result = true;
        InstructionIterator baseIt(in_base);
        InstructionIterator otherIter(in_toTest);
        while (baseIt.hasNext()) {
          auto nextInst = baseIt.next();
          if (nextInst->isEnabled() && !nextInst->isComposite()) {
            // Pop the other iter as well
            auto otherInstr = getNextInstruction(otherIter);
            if (!compareInst(nextInst, otherInstr)) {
              return false;
            }
            out_Iter = otherIter;
          }
        }
        return result;
      };

  const std::function<InstPtr(std::shared_ptr<CompositeInstruction>)>
      getLastInstr =
          [&](std::shared_ptr<CompositeInstruction> composite) -> InstPtr {
    auto instructions = composite->getInstructions();
    for (auto rIter = instructions.rbegin(); rIter != instructions.rend();
         ++rIter) {
      if ((*rIter)->isEnabled()) {
        if (!(*rIter)->isComposite()) {
          return (*rIter);
        } else {
          std::shared_ptr<CompositeInstruction> compositeCast =
              std::dynamic_pointer_cast<CompositeInstruction>(*rIter);
          return getLastInstr(compositeCast);
        }
        return (*rIter);
      }
    }
    return nullptr;
  };

  // Cache of the iterators during matching
  std::vector<InstructionIterator> matchedIters;
  while (getBaseLength(baseComposite) > 1) {
    bool allMatched = true;
    matchedIters.clear();
    for (const auto &compositeToTest : in_composites) {
      InstructionIterator compIter(compositeToTest);
      if (!isBaseOf(baseComposite, compositeToTest, compIter)) {
        // Remove the last instruction of base and re-check
        auto lastInst = getLastInstr(baseComposite);
        if (lastInst) {
          lastInst->disable();
          baseComposite->removeDisabled();
        } else {
          baseComposite->clear();
        }
        allMatched = false;
        break;
      }
      matchedIters.emplace_back(compIter);
    }

    if (allMatched) {
      assert(matchedIters.size() == in_composites.size());
      std::vector<std::shared_ptr<CompositeInstruction>> subCircuits;
      for (size_t i = 0; i < in_composites.size(); ++i) {
        auto comp = in_composites[i];
        auto iter = matchedIters[i];
        auto newComp =
            // IMPORTANT: the sub-circuit must keep the same name as the
            // composite since downstream mapping depends on this.
            gateRegistry->createComposite(comp->name());
        // Add the remaining instructions
        while (iter.hasNext()) {

          auto nextInst = iter.next();

          if (nextInst->isEnabled() && !nextInst->isComposite()) {
            newComp->addInstruction(nextInst->clone());
          }
        }
        subCircuits.emplace_back(newComp);
      }
      result.m_baseAnsatz = baseComposite;
      result.m_obsCircuits = subCircuits;
      return result;
    }
  }

  // These circuits have no relationship:
  baseComposite->clear();
  // Returns an empty base and all input composites as individual circuits.
  result.m_baseAnsatz = baseComposite;
  result.m_obsCircuits = in_composites;
  return result;
}
bool ObservedAnsatz::validate(
    const std::vector<std::shared_ptr<CompositeInstruction>> &in_composites)
    const {
  if (m_obsCircuits.size() != in_composites.size()) {
    return false;
  }
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");

  for (int i = 0; i < m_obsCircuits.size(); ++i) {
    auto obsCirc = m_obsCircuits[i];

    std::shared_ptr<xacc::CompositeInstruction> combinedComp =
        gateRegistry->createComposite("__RECOVERED__" + std::to_string(i));
    for (int idx = 0; idx < m_baseAnsatz->nInstructions(); ++idx) {
      combinedComp->addInstruction(m_baseAnsatz->getInstruction(idx)->clone());
    }
    for (int idx = 0; idx < obsCirc->nInstructions(); ++idx) {
      combinedComp->addInstruction(obsCirc->getInstruction(idx)->clone());
    }
    auto originalCirc = in_composites[i];
    // Compare two composites:
    InstructionIterator origIter(originalCirc);
    InstructionIterator otherIter(combinedComp);
    while (origIter.hasNext()) {
      auto nextInst = origIter.next();
      if (nextInst->isEnabled() && !nextInst->isComposite()) {
        // Pop the other iter as well
        auto otherInstr = getNextInstruction(otherIter);
        if (!compareInst(nextInst, otherInstr)) {
          return false;
        }
      }
    }
    assert(!origIter.hasNext());
    if (otherIter.hasNext()) {
      // The other composite contains extra instructions.
      return false;
    }
  }
  return true;
}
} // namespace quantum
} // namespace xacc