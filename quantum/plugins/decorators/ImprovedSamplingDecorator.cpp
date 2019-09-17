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
#include "ImprovedSamplingDecorator.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"
#include <numeric>

namespace xacc {
namespace quantum {
void ImprovedSamplingDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  if (!decoratedAccelerator) {
    xacc::error("Cannot run the ImprovedSamplingDecorator without a delegate "
                "Accelerator.");
  }

  if (!xacc::optionExists("sampler-n-execs")) {
    xacc::error("Cannot find sampler-n-execs option. Skipping "
                "ImprovedSamplingDecorator.");
    return;
  }

  // Get the number of extra runs to execute
  auto nRuns = std::stoi(xacc::getOption("sampler-n-execs"));

  // Execute the first one.
  decoratedAccelerator->execute(buffer, function);
  auto counts = buffer->getMeasurementCounts();

  for (int i = 1; i < nRuns; i++) {
    auto cloned = buffer->clone();
    cloned->clearMeasurements();
    decoratedAccelerator->execute(cloned, function);

    auto clonedCounts = cloned->getMeasurementCounts();

    // Aggregate results...
    counts = std::accumulate(counts.begin(), counts.end(), clonedCounts,
                             [](std::map<std::string, int> &m,
                                const std::pair<std::string, int> &p) {
                               return (m[p.first] += p.second, m);
                             });

    // Set on the return buffer;
    buffer->setMeasurements(counts);
  }

  return;
}

void
ImprovedSamplingDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;

  if (!decoratedAccelerator) {
    xacc::error("Cannot run the ImprovedSamplingDecorator without a delegate "
                "Accelerator.");
  }

  if (!xacc::optionExists("sampler-n-execs")) {
    xacc::error("Cannot find sampler-n-execs option. Skipping "
                "ImprovedSamplingDecorator.");
  }

  auto nRuns = std::stoi(xacc::getOption("sampler-n-execs"));

  auto tmpBuffer = xacc::qalloc(buffer->size());
  decoratedAccelerator->execute(tmpBuffer, functions);
  buffers = tmpBuffer->getChildren();

  std::map<std::string, std::map<std::string, int>> childrenCounts;
  for (auto &b : buffers) {
    childrenCounts.insert({b->name(), b->getMeasurementCounts()});
  }

  for (int i = 1; i < nRuns; i++) {

    auto tmpBuffer = xacc::qalloc(buffer->size());
    decoratedAccelerator->execute(tmpBuffer, functions);
    buffers = tmpBuffer->getChildren();

    for (auto &b : buffers) {
      auto newCounts = b->getMeasurementCounts();
      childrenCounts[b->name()] =
          std::accumulate(childrenCounts[b->name()].begin(),
                          childrenCounts[b->name()].end(), newCounts,
                          [](std::map<std::string, int> &m,
                             const std::pair<std::string, int> &p) {
                            return (m[p.first] += p.second, m);
                          });

      b->setMeasurements(childrenCounts[b->name()]);
      buffer->appendChild(b->name(), b);
    }
  }

  return;
}

} // namespace quantum
} // namespace xacc