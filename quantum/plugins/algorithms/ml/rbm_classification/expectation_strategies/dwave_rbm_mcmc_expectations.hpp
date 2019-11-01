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
#ifndef XACC_ALGORITHM_RBM_CLASSIFICATION_DWAVE_MCMCEXP_HPP_
#define XACC_ALGORITHM_RBM_CLASSIFICATION_DWAVE_MCMCEXP_HPP_

#include "rbm_classification.hpp"

namespace xacc {
namespace algorithm {

class DWaveRBM_MCMCDataExpectationStrategy : public ExpectationStrategy {
protected:
  //   def sample_arr(probs, a=-1, b=1):
  //     rand_sample = np.random.random(probs.shape)
  //     A = np.ones(probs.shape)*b
  //     A[rand_sample > probs] = a
  //     return A

  Eigen::VectorXi sampleArray(Eigen::VectorXd &probs) {

    Eigen::VectorXd random = Eigen::VectorXd::Random(probs.size());
    Eigen::VectorXi A = Eigen::VectorXi::Ones(probs.size());

    for (int i = 0; i < probs.size(); i++) {
      A(i) = random(i) > probs(i) ? -1 : A(i);
    }

    return A;
  }

  Eigen::VectorXi sampleHGivenV(Eigen::VectorXi &v, Eigen::VectorXd &b,
                                Eigen::MatrixXd &w) {
    Eigen::VectorXd tmp = 2 * (b + w * v.cast<double>());
    for (int i = 0; i < tmp.size(); i++) {
      tmp(i) = 1. / (1. + std::exp(-tmp(i)));
    }
    Eigen::VectorXd h_probs = tmp;
    return sampleArray(h_probs);
  }
  Eigen::VectorXi sampleVGivenH(Eigen::VectorXi &h, Eigen::VectorXd &a,
                                Eigen::MatrixXd &w) {
    Eigen::VectorXd tmp = 2 * (a + w * h.cast<double>());
    for (int i = 0; i < tmp.size(); i++) {
      tmp(i) = 1. / (1. + std::exp(-tmp(i)));
    }
    Eigen::VectorXd v_probs = tmp;
    return sampleArray(v_probs);
  }

public:
  const std::string name() const override { return "dwave-mcmc"; }
  const std::string description() const override { return ""; }

  std::tuple<Eigen::MatrixXd, Eigen::VectorXd, Eigen::VectorXd>
  compute(Eigen::MatrixXd &features, Eigen::MatrixXd &w, Eigen::VectorXd &v,
          Eigen::VectorXd &h, HeterogeneousMap options = {}) override {

    int nSamples = options.keyExists<int>("n-samples")
                       ? options.get<int>("n-samples")
                       : 10;

    Eigen::VectorXi curr_v = Eigen::VectorXi::Ones(v.size());
    Eigen::VectorXd tmp = Eigen::VectorXd::Random(v.size());
    for (int i = 0; i < tmp.size(); i++) {
      curr_v(i) = tmp(i) > .5 ? curr_v(i) : -1;
    }
    std::vector<Eigen::VectorXi> vs, hs;
    for (int i = 0; i < nSamples; i++) {
      vs.push_back(curr_v);
      Eigen::VectorXi curr_h = sampleHGivenV(curr_v, h, w);
      hs.push_back(curr_h);
      curr_v = sampleVGivenH(curr_h, v, w);
    }

    Eigen::MatrixXi visibles = Eigen::MatrixXi::Zero(nSamples, v.size());
    for (int i = 0; i < nSamples; i++) {
      visibles.row(i) = vs[i];
    }
    Eigen::MatrixXi hidden = Eigen::MatrixXi::Zero(nSamples, h.size());
    for (int i = 0; i < nSamples; i++) {
      hidden.row(i) = hs[i];
    }

    // sum v
    Eigen::VectorXd sum_v = Eigen::VectorXd::Zero(v.size());
    for (int i = 0; i < v.size(); i++) {
      sum_v(i) = visibles.col(i).sum();
    }

    // sum h
    Eigen::VectorXd sum_h = Eigen::VectorXd::Zero(h.size());
    for (int i = 0; i < h.size(); i++) {
      sum_h(i) = hidden.col(i).sum();
    }

    // sum v * h
    Eigen::MatrixXd sum_vh = (visibles.transpose() * hidden).cast<double>();

    // exp w = sumvh / nsamples
    Eigen::MatrixXd expW = sum_vh / nSamples;
    Eigen::VectorXd expv = sum_v / nSamples;
    Eigen::VectorXd exph = sum_h / nSamples;
    // exp v = sumv / nsamples
    // exp h = sumh / nsamples

    return std::make_tuple(expW, expv, exph);
  }
};
} // namespace algorithm
} // namespace xacc
#endif