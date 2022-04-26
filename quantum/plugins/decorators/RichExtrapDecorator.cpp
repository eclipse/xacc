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
#include "RichExtrapDecorator.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"
#include <cassert>
#include "util/poly_fit.hpp"
namespace xacc {
namespace quantum {
void RichExtrapDecorator::initialize(const HeterogeneousMap &params) {
  // Must be odd integers
  m_noiseScalings = {1, 3, 5};
  if (params.keyExists<std::vector<int>>("scale-factors")) {
    m_noiseScalings = params.get<std::vector<int>>("scale-factors");
    assert(m_noiseScalings.size() > 1);
    for (const auto &x : m_noiseScalings) {
      if (x % 2 != 1) {
        xacc::error("Noise scaling factors must be odd integers.");
      }
      if (x < 1) {
        xacc::error("Noise scaling factors must be positive.");
      }
    }
    std::set<int> noiseScalingsAsSet(m_noiseScalings.begin(), m_noiseScalings.end());
    if (noiseScalingsAsSet.size() != m_noiseScalings.size()) {
      xacc::error("Repeated values in noise scaling factors.");
    }
    std::sort(m_noiseScalings.begin(), m_noiseScalings.end());
    if (m_noiseScalings[0] != 1) {
      // If 1 (base circuit) was not present, adding it.
      m_noiseScalings.insert(m_noiseScalings.begin(), 1);
    }
  }
  // Only scale CNOT by default
  m_scalingGate = "CNOT";
  if (params.stringExists("scale-gate")) {
    m_scalingGate = params.getString("scale-gate");
  }
  m_config = params;
}

void RichExtrapDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {
  if (!decoratedAccelerator) {
    xacc::error("Null Decorated Accelerator Error");
  }
  
  // Single extrapolation mode (just scale and run)
  if (m_config.keyExists<int>("rich-extrap-r")) {
    // Get RO error probs
    auto r = m_config.get<int>("rich-extrap-r");
    buffer->addExtraInfo("rich-extrap-r", ExtraInfo(r));

    auto provider = xacc::getService<IRProvider>("quantum");

    auto f =
        provider->createComposite(function->name(), function->getVariables());

    InstructionIterator it(function);
    while (it.hasNext()) {
      auto nextInst = it.next();

      if (!nextInst->isComposite() && nextInst->isEnabled()) {

        if (nextInst->name() == "CNOT") {
          for (int i = 0; i < r; i++) {
            auto tmp = nextInst->clone();
            tmp->setBits(nextInst->bits());
            f->addInstruction(tmp);
          }
        } else {
          f->addInstruction(nextInst);
        }
      }
    }

    // std::cout << "HELLO: \n" << f->toString() << "\n";

    decoratedAccelerator->execute(buffer, f);
    return;
  }

  // Run with Zero noise extrapolation:
  std::vector<std::shared_ptr<CompositeInstruction>> foldedComps;
  auto provider = xacc::getService<IRProvider>("quantum");
  for (const auto &scale : m_noiseScalings) {
    auto f = provider->createComposite(function->name() + "_SCALE_" +
                                           std::to_string(scale),
                                       function->getVariables());
    InstructionIterator it(function);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (!nextInst->isComposite() && nextInst->isEnabled()) {
        if (nextInst->name() == m_scalingGate) {
          for (int i = 0; i < scale; ++i) {
            auto tmp = nextInst->clone();
            tmp->setBits(nextInst->bits());
            f->addInstruction(tmp);
          }
        } else {
          f->addInstruction(nextInst);
        }
      }
    }
    // std::cout << "HELLO: \n" << f->toString() << "\n";
    foldedComps.emplace_back(f);
  }

  auto tmp_buffer = xacc::qalloc(buffer->size());
  decoratedAccelerator->execute(tmp_buffer, foldedComps);
  // tmp_buffer->print();
  std::vector<double> exp_vals;
  for (auto &child : tmp_buffer->getChildren()) {
    const double expValZ = child->getExpectationValueZ();
    // std::cout << "Exp-val = " << expValZ << "\n";
    exp_vals.emplace_back(expValZ);
  }
  std::vector<double> x;
  for (const auto &scale : m_noiseScalings) {
    x.emplace_back(static_cast<double>(scale));
  }
  const auto fit_coeffs = xacc::poly_fit(x, exp_vals);
  const double zne_exp_val_z = fit_coeffs[0];
  // std::cout << "ZNE-val = " << zne_exp_val_z << "\n";
  // Report the raw count (first child buffer) along with the ZNE value for exp-val
  auto base_buffer = tmp_buffer->getChildren()[0];
  for (const auto &[bitStr, count] : base_buffer->getMeasurementCounts()) {
    buffer->appendMeasurement(bitStr, count);
  }
  buffer->addExtraInfo("rich-extrap-exp-vals", exp_vals);
  buffer->addExtraInfo("exp-val-z", zne_exp_val_z);
}

void RichExtrapDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;

  if (!decoratedAccelerator) {
    xacc::error("RichExtrap - Null Decorated Accelerator Error");
  }

  if (m_config.keyExists<int>("rich-extrap-r")) {
    // Get RO error probs
    auto r = m_config.get<int>("rich-extrap-r");
    buffer->addExtraInfo("rich-extrap-r", ExtraInfo(r));

    auto provider = xacc::getService<IRProvider>("quantum");

    std::vector<std::shared_ptr<CompositeInstruction>> newFuncs;

    for (auto &f : functions) {
      auto newF = provider->createComposite(f->name(), f->getVariables());

      InstructionIterator it(f);
      while (it.hasNext()) {
        auto nextInst = it.next();

        if (!nextInst->isComposite() && nextInst->isEnabled()) {

          if (nextInst->name() == "CNOT") {
            for (int i = 0; i < r; i++) {
              auto tmp = nextInst->clone();
              tmp->setBits(nextInst->bits());
              newF->addInstruction(tmp);
            }
          } else {
            newF->addInstruction(nextInst);
          }
        }
      }

      // xacc::info("HI: " + newF->toString("q"));

      newFuncs.push_back(newF);
    }

    decoratedAccelerator->execute(buffer, newFuncs);
    return;
  }

  // Full ZNE workflow
  for (auto &f : functions) {
    auto tmp_buffer = xacc::qalloc(buffer->size());
    tmp_buffer->setName(f->name());
    execute(tmp_buffer, f);
    buffer->appendChild(f->name(), tmp_buffer);
  }
}

} // namespace quantum
} // namespace xacc