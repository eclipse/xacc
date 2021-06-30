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
#include "ddcl.hpp"

#include "xacc.hpp"
#include "xacc_service.hpp"

#include <memory>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <regex>
#include <fstream>

#include "InstructionIterator.hpp"

using namespace xacc;

namespace xacc {
namespace algorithm {
bool DDCL::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.keyExists<std::shared_ptr<CompositeInstruction>>("ansatz")) {
    std::cout << "Ansatz was false\n";
    return false;
  } else if (!parameters.keyExists<std::shared_ptr<Accelerator>>(
                 "accelerator")) {
    std::cout << "Acc was false\n";
    return false;
  } else if (!parameters.keyExists<std::vector<double>>("target_dist")) {
    return false;
  } else if (!parameters.stringExists("loss")) {
    return false;
  }

  if (parameters.pointerLikeExists<Optimizer>("optimizer")) {
    optimizer = parameters.get<std::shared_ptr<Optimizer>>("optimizer");
  }
  kernel = parameters.get<std::shared_ptr<CompositeInstruction>>("ansatz");
  accelerator = parameters.get<std::shared_ptr<Accelerator>>("accelerator");
  target_dist = parameters.get<std::vector<double>>("target_dist");
  loss = parameters.getString("loss");
  gradient = parameters.stringExists("gradient")
                 ? parameters.getString("gradient")
                 : "";

  if (parameters.keyExists<bool>("persist-buffer")) {
    persistBuffer = parameters.get<bool>("persist-buffer");
  }

  _parameters = parameters;
  return true;
}

const std::vector<std::string> DDCL::requiredParameters() const {
  return {"target_dist", "loss",   "gradient",      "optimizer",
          "accelerator", "ansatz", "persist-buffer"};
}

void DDCL::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  if (!optimizer) {
    xacc::error("DDCL: No Optimizer provided.");
  }
  auto provider = xacc::getIRProvider("quantum");

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.
  OptFunction f(
      [&, this](const std::vector<double> &x, std::vector<double> &dx) {
        // Evaluate and add measurements to all qubits
        auto evaled = kernel->operator()(x);

        std::set<std::size_t> uniqueBits = kernel->uniqueBits();
        for (auto b : uniqueBits) {
          auto m = provider->createInstruction("Measure",
                                               std::vector<std::size_t>{b});
          evaled->addInstruction(m);
        }

        // Start the list of circuits to execute
        std::vector<Circuit> circuits{evaled};

        // Get the user specified loss and gradient strategies
        auto gradientStrategy =
            dx.empty() ? std::make_shared<NullGradientStrategy>()
                       : xacc::getService<GradientStrategy>(gradient);
        auto lossStrategy = xacc::getService<LossStrategy>(loss);

        // Only compute gradients if this is
        // a gradient based optimizer
        if (!dx.empty()) {
          // Request the circuits to be executed in support of
          // computing the gradient, add it to the circuits list
          auto gradientCircuits =
              gradientStrategy->getCircuitExecutions(kernel, x);
          for (auto &g : gradientCircuits) {
            circuits.push_back(g);
          }

          assert(circuits.size() == 2 * x.size() + 1);
        } else {
          assert(circuits.size() == 1);
        }

        // Execute!
        auto tmpBuffer = xacc::qalloc(buffer->size());
        accelerator->execute(tmpBuffer, circuits);
        auto buffers = tmpBuffer->getChildren();

        for (auto b : buffers) {
          b->addExtraInfo("parameters", x);
          buffer->appendChild(b->name(), b);
        }

        // The first child buffer is for the loss function
        std::shared_ptr<AcceleratorBuffer> lossBufferPtr;
        Counts counts;
        if (!buffers.empty()) {
          counts = buffers[0]->getMeasurementCounts();
          lossBufferPtr = buffers[0];
        } else {
          counts = tmpBuffer->getMeasurementCounts();
          lossBufferPtr = tmpBuffer;
        }

        // Compute and return the loss, this gives us the
        // distribution of the loss circuit too
        auto loss_and_qdist =
            lossStrategy->compute(counts, target_dist, _parameters);
        auto loss = loss_and_qdist.first;
        auto qdist = loss_and_qdist.second;

        lossBufferPtr->addExtraInfo("qdist", qdist);
        lossBufferPtr->addExtraInfo("loss", loss);

        // Only compute gradients if this is
        // a gradient based optimizer
        if (!dx.empty()) {
          // The rest of the buffers are for the gradients
          std::vector<std::shared_ptr<AcceleratorBuffer>> gradResults;
          for (int i = 1; i < buffers.size(); i++) {
            gradResults.push_back(buffers[i]);
          }

          // Compute the gradient with the results
          gradientStrategy->compute(dx, gradResults, qdist, target_dist);

          lossBufferPtr->addExtraInfo("gradient", dx);
        }
        std::stringstream ss;
        ss << x << ") = " << std::setprecision(12) << loss;
        xacc::info("Loss(" + ss.str());
        return loss;
      },
      kernel->nVariables());

  auto result = optimizer->optimize(f);

  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));

  if (persistBuffer) {
    char filename[40];
    struct tm *timenow;
    time_t now = time(NULL);
    timenow = localtime(&now);
    strftime(filename, sizeof(filename), "_%Y-%m-%d_%Hh%Mm%Ss", timenow);
    std::string time_str =
        ".ddcl_" + kernel->name() + std::string(filename) + ".ab";
    std::ofstream out(time_str);
    buffer->print(out);
    out.close();
  }
  return;
}

std::vector<double>
DDCL::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
              const std::vector<double> &x) {
  auto provider = xacc::getIRProvider("quantum");

  // Evaluate and add measurements to all qubits
  std::shared_ptr<CompositeInstruction> evaled;
  if (x.empty()) {
    evaled = kernel;
  } else {
    evaled = kernel->operator()(x);
  }
  std::set<std::size_t> uniqueBits;
  InstructionIterator iter(evaled);
  while (iter.hasNext()) {
    auto next = iter.next();
    if (!next->isComposite()) {
      for (auto &b : next->bits()) {
        uniqueBits.insert(b);
      }
    }
  }
  for (auto b : uniqueBits) {
    auto m =
        provider->createInstruction("Measure", std::vector<std::size_t>{b});
    evaled->addInstruction(m);
  }

  // Start the list of circuits to execute
  std::vector<Circuit> circuits{evaled};

  auto lossStrategy = xacc::getService<LossStrategy>(loss);

  // Execute!
  auto tmpBuffer = xacc::qalloc(buffer->size());
  accelerator->execute(tmpBuffer, circuits);
  auto buffers = tmpBuffer->getChildren();

  // The first child buffer is for the loss function
  Counts counts;
  if (!buffers.empty()) {
    counts = buffers[0]->getMeasurementCounts();
  } else {
    counts = tmpBuffer->getMeasurementCounts();
  }
  // Compute and return the loss, this gives us the
  // distribution of the loss circuit too
  auto loss_and_qdist = lossStrategy->compute(counts, target_dist);
  auto loss = loss_and_qdist.first;
  auto qdist = loss_and_qdist.second;
  for (auto b : buffers) {
    b->addExtraInfo("parameters", x);
    b->addExtraInfo("q_dist", qdist);
    buffer->appendChild(b->name(), b);
  }

  return {loss};
}
} // namespace algorithm
} // namespace xacc
