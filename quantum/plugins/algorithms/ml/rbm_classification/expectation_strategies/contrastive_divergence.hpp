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
#ifndef XACC_ALGORITHM_RBM_CLASSIFICATION_CD_HPP_
#define XACC_ALGORITHM_RBM_CLASSIFICATION_CD_HPP_

#include "rbm_classification.hpp"

namespace xacc {
namespace algorithm {

class ContrastiveDivergenceExpectationStrategy : public ExpectationStrategy {
protected:

  Eigen::VectorXd sample(Eigen::VectorXd& probs) {
      Eigen::VectorXd tmp = probs + Eigen::VectorXd::Random(probs.size());
      Eigen::VectorXd tmp2 = tmp.array().floor();
      return tmp2;
  }
public:
const std::string name() const override { return "cd"; }
  const std::string description() const override { return ""; }
  std::tuple<Eigen::MatrixXd, Eigen::VectorXd, Eigen::VectorXd>
  compute(Eigen::MatrixXd &features, Eigen::MatrixXd &w, Eigen::VectorXd &v,
          Eigen::VectorXd &h, HeterogeneousMap options = {}) override {

    Eigen::VectorXd exp_h = options.get<Eigen::VectorXd>("dataexp_h");
    Eigen::VectorXd h_sample = sample(exp_h);

    Eigen::MatrixXd tmp = h_sample * w.transpose();
     for (int col = 0; col < tmp.cols(); col++) {
      for (int row = 0; row < tmp.rows(); row++) {
        tmp(row, col) += v(col);
      }
    }

    // tmp.unaryExpr([](double& x) {return 1./(1.+std::exp(-x));});
    
    // Eigen::MatrixXd h_probs = tmp.array().exp();//unaryExpr(s);
    // h_probs has to be bs x n_v

    Eigen::MatrixXd w_expectation;

    Eigen::VectorXd v_expectation = Eigen::VectorXd::Zero(v.size());
    Eigen::VectorXd h_expectation = Eigen::VectorXd::Zero(v.size());

    // for (int i = 0; i < features.rows(); i++) {
    //   v_expectation(i) = features.row(i).sum() / features.row(i).size();
    //   h_expectation(i) = h_probs.row(i).sum() / h_probs.row(i).size();
    // }

    return std::make_tuple(w_expectation, v_expectation, h_expectation);
  }
};
} // namespace algorithm
} // namespace xacc
#endif