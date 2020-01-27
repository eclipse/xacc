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
#include "rbm_classification.hpp"

#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include <memory>
#include <iomanip>
#include <cassert>
#include <random>
#include <chrono>

#include "InstructionIterator.hpp"

using namespace xacc;

namespace xacc {
namespace algorithm {
bool RBMClassification::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.keyExists<std::shared_ptr<CompositeInstruction>>("rbm")) {
    std::cout << "Ansatz was false\n";
    return false;
  }

  rbm = parameters.getPointerLike<CompositeInstruction>("rbm");

  auto train_file = parameters.getString("train-file");
  if (!xacc::fileExists(train_file)) {
      xacc::error("Invalid train-file, file does not exist:\n" + train_file);
  }
  training_data = load_csv<Eigen::MatrixXd>(train_file);
  modelExp = "dwave-mcmc";
  if (parameters.stringExists("model-exp-strategy")) {
    modelExp = parameters.getString("model-exp-strategy");
  }

  _parameters = parameters;
  return true;
}

const std::vector<std::string> RBMClassification::requiredParameters() const {
  return {"rbm", "model-exp-strategy"};
}

void RBMClassification::execute(
    const std::shared_ptr<AcceleratorBuffer> buffer) const {

  auto provider = xacc::getIRProvider("quantum");
  auto nv = rbm->getParameter(0).as<int>();
  auto nh = rbm->getParameter(1).as<int>();

  int batch_size = 1;
  int n_batches = training_data.rows() / batch_size;
  double l2reg = 0.0;
  double lr = .01;

  // Initialize the RBM
  Eigen::MatrixXd w = Eigen::MatrixXd::Random(nv, nh);
  Eigen::VectorXd bh = Eigen::VectorXd::Zero(nh);
  Eigen::VectorXd bv = Eigen::VectorXd::Zero(nv);

  //   std::cout << "Started:\n" << w << "\n";
  // Get the expectation strategies
  auto dataExpectations = xacc::getService<ExpectationStrategy>("data-exp");
  auto modelExpectations = xacc::getService<ExpectationStrategy>(modelExp);

  // Strategy:
  // loop through data in chunks of batch_size
  int epoch = 0;
  for (int batchIdx = 0; batchIdx < n_batches; batchIdx += batch_size) {
    xacc::debug("Starting Epoch " +std::to_string(epoch));
    epoch++;

    // batch should be batch_size x nv
    Eigen::MatrixXd batch = training_data.block(batchIdx, 0, batch_size, nv);

    // Create data exp w, v, and h, as well as model versions
    Eigen::MatrixXd tmp = Eigen::MatrixXd::Zero(1, 1);
    Eigen::MatrixXd dataexp_W, modexp_W;
    Eigen::VectorXd dataexp_v, dataexp_h, modexp_v, modexp_h;

    // Compute the data expectations
    std::tie(dataexp_W, dataexp_v, dataexp_h) =
        dataExpectations->compute(batch, w, bv, bh);

    // Compute the model expectations
    if (modelExp == "dwave" || modelExp == "dwave-mcmc") {
      std::tie(modexp_W, modexp_v, modexp_h) =
          modelExpectations->compute(tmp, w, bv, bh, _parameters);
    }

    // Get the deltas and update for the next iteration
    Eigen::MatrixXd wdelta = dataexp_W - modexp_W;
    Eigen::VectorXd v_delta = dataexp_v - modexp_v;
    Eigen::VectorXd h_delta = dataexp_h - modexp_h;
    w = (1. - l2reg) * w + lr * wdelta;
    bv = (1. - l2reg) * bv + lr * v_delta;
    bh = (1. - l2reg) * bh + lr * h_delta;
  }

  std::vector<double> wvec(w.data(), w.data() + w.size()),
      v_vec(bv.data(), bv.data() + bv.size()),
      h_vec(bh.data(), bh.data() + bh.size());
  buffer->addExtraInfo("w", wvec);
  buffer->addExtraInfo("bv", v_vec);
  buffer->addExtraInfo("bh", h_vec);
}

} // namespace algorithm
} // namespace xacc
