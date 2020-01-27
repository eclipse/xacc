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
#ifndef XACC_ALGORITHM_RBM_CLASSIFICATION_HPP_
#define XACC_ALGORITHM_RBM_CLASSIFICATION_HPP_

#include "Algorithm.hpp"
#include <vector>
#include <Eigen/Dense>
#include <fstream>

namespace xacc {
namespace algorithm {

class ExpectationStrategy : public Identifiable {
public:
  virtual std::tuple<Eigen::MatrixXd, Eigen::VectorXd, Eigen::VectorXd>
  compute(Eigen::MatrixXd &features, Eigen::MatrixXd &w, Eigen::VectorXd &v,
          Eigen::VectorXd &h, HeterogeneousMap options = {}) = 0;
};

class RBMClassification : public Algorithm {
protected:
  CompositeInstruction* rbm;
  Eigen::MatrixXd training_data;
  std::string modelExp;

  HeterogeneousMap _parameters;

  template <typename M> M load_csv(const std::string &path) {
    std::ifstream indata;
    indata.open(path);
    std::string line;
    std::vector<double> values;
    int rows = 0;
    while (std::getline(indata, line)) {
      if (line[0] != '#') {
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ',')) {
          values.push_back(std::stod(cell));
        }
        ++rows;
      }
    }
    return Eigen::Map<
        const Eigen::Matrix<typename M::Scalar, M::RowsAtCompileTime,
                            M::ColsAtCompileTime, Eigen::RowMajor>>(
        values.data(), rows, values.size() / rows);
  }

public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  const std::string name() const override { return "rbm-classification"; }
  const std::string description() const override { return ""; }
  DEFINE_ALGORITHM_CLONE(RBMClassification)
};
} // namespace algorithm
} // namespace xacc
#endif
