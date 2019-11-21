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
#ifndef XACC_ALGORITHM_DDCL_HPP_
#define XACC_ALGORITHM_DDCL_HPP_

#include "Algorithm.hpp"
#include <vector>

namespace xacc {
namespace algorithm {
using Circuit = std::shared_ptr<CompositeInstruction>;
using Counts = std::map<std::string, int>;

class LossStrategy : public Identifiable {
public:
  virtual std::pair<double, std::vector<double>>
  compute(Counts &counts, const std::vector<double> &target, const HeterogeneousMap& options = {}) = 0;

  virtual bool isValidGradientStrategy(const std::string &gradientStrategy) = 0;
};

class GradientStrategy : public Identifiable {
public:
  // Generate circuits to run that will be used to
  // compute gradients. Need the un-evaluated, original ansatz
  // and the current iterate's parameters
  virtual std::vector<Circuit>
  getCircuitExecutions(Circuit circuit, const std::vector<double> &x) = 0;
  virtual void compute(std::vector<double> &grad, std::vector<std::shared_ptr<AcceleratorBuffer>> results,
                       const std::vector<double> &q_dist,
                       const std::vector<double> &target_dist) = 0;
};

class NullGradientStrategy : public GradientStrategy {
public:
  std::vector<Circuit>
  getCircuitExecutions(Circuit circuit, const std::vector<double> &x) override {
    return {};
  }
  void compute(std::vector<double> &grad, std::vector<std::shared_ptr<AcceleratorBuffer>> results,
               const std::vector<double> &q_dist,
               const std::vector<double> &target_dist) override {
    return;
  }
  const std::string name() const override { return "null-gs"; }
  const std::string description() const override { return ""; }
};

class DDCL : public Algorithm {
protected:
  std::shared_ptr<Optimizer> optimizer;
  std::shared_ptr<CompositeInstruction> kernel;
  std::shared_ptr<Accelerator> accelerator;
  std::vector<double> initial_params;
  std::vector<double> target_dist;
  std::string gradient;
  std::string loss;
  bool persistBuffer = false;

  HeterogeneousMap _parameters;

public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  std::vector<double> execute(const std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<double> &parameters) override;
  const std::string name() const override { return "ddcl"; }
  const std::string description() const override { return ""; }

  DEFINE_ALGORITHM_CLONE(DDCL)

};
} // namespace algorithm
} // namespace xacc
#endif