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
#ifndef XACC_ALGORITHM_DDCL_STRATEGIES_JS_LOSS_HPP_
#define XACC_ALGORITHM_DDCL_STRATEGIES_JS_LOSS_HPP_

#include "ddcl.hpp"
#include "xacc.hpp"
#include <cassert>
#include <set>
#include "InstructionIterator.hpp"
namespace xacc {
namespace algorithm {

class JSLossStrategy : public LossStrategy {
protected:
  // Helper function
  double entropy(const std::vector<double> p, const std::vector<double> q) {
    double sum = 0.0;
    for (int i = 0; i < p.size(); i++) {
      if (std::fabs(p[i]) > 1e-12) {
        sum += p[i] * std::log(p[i] / q[i]);
      }
    }
    return sum;
  }

public:
  std::pair<double, std::vector<double>>
  compute(Counts &counts, const std::vector<double> &target, const HeterogeneousMap& = {}) override {
    int shots = 0;
    for (auto &x : counts) {
      shots += x.second;
    }

    // Compute the probability distribution
    std::vector<double> q(target.size()); // all zeros
    for (auto &x : counts) {
      int idx = std::stoi(x.first, nullptr, 2);
      q[idx] = (double)x.second / shots;
    }

    // get M=1/2(P+Q)
    std::vector<double> m(target.size());
    for (int i = 0; i < m.size(); i++) {
      m[i] = .5 * (target[i] + q[i]);
    }

    auto js = 0.5 * (entropy(target, m) + entropy(q, m));
    return std::make_pair(js, q);
  }

  bool isValidGradientStrategy(const std::string &gradientStrategy) override {
    // FIXME define what grad strategies this guy works with
    return true;
  }
  const std::string name() const override { return "js"; }
  const std::string description() const override { return ""; }
};

class JSParameterShiftGradientStrategy : public GradientStrategy {
protected:
  std::vector<double> currentParameterSet;

public:
  std::vector<Circuit>
  getCircuitExecutions(Circuit circuit, const std::vector<double> &x) override {
    currentParameterSet = x;
    std::set<std::size_t> uniqueBits = circuit->uniqueBits();
    std::vector<Circuit> grad_circuits;
    auto provider = xacc::getIRProvider("quantum");
    for (int i = 0; i < x.size(); i++) {
      auto xplus = x[i] + xacc::constants::pi / 2.;
      auto xminus = x[i] - xacc::constants::pi / 2.;
      std::vector<double> tmpx_plus = x, tmpx_minus = x;
      tmpx_plus[i] = xplus;
      tmpx_minus[i] = xminus;
      auto xplus_circuit = circuit->operator()(tmpx_plus);
      auto xminus_circuit = circuit->operator()(tmpx_minus);

      xplus_circuit->setName(circuit->name()+"param_"+std::to_string(i)+"_shift_plus");
      xminus_circuit->setName(circuit->name()+"param_"+std::to_string(i)+"_shift_minus");

      for (auto &ii : uniqueBits) {
        auto m = provider->createInstruction("Measure",
                                             std::vector<std::size_t>{ii});
        xplus_circuit->addInstruction(m);
      }
      for (auto &ii : uniqueBits) {
        auto m = provider->createInstruction("Measure",
                                             std::vector<std::size_t>{ii});
        xminus_circuit->addInstruction(m);
      }
      grad_circuits.push_back(xplus_circuit);
      grad_circuits.push_back(xminus_circuit);
    }

    return grad_circuits;
  }

  void compute(std::vector<double> &grad, std::vector<std::shared_ptr<AcceleratorBuffer>> results,
               const std::vector<double> &q_dist,
               const std::vector<double> &target_dist) override {
    assert(2 * grad.size() == results.size());

    // Get the number of shosts
    int shots = 0;
    for (auto &x : results[0]->getMeasurementCounts()) {
      shots += x.second;
    }

    // Create q+ and q- vectors
    int counter = 0;
    std::vector<std::vector<double>> qplus_theta, qminus_theta;
    for (int i = 0; i < results.size(); i += 2) {
      std::vector<double> qp(q_dist.size()), qm(q_dist.size());
      for (auto &x : results[i]->getMeasurementCounts()) {
        int idx = std::stoi(x.first, nullptr, 2);
        qp[idx] = (double)x.second / shots;
      }
      for (auto &x : results[i + 1]->getMeasurementCounts()) {
        int idx = std::stoi(x.first, nullptr, 2);
        qm[idx] = (double)x.second / shots;
      }

      std::vector<double> shiftedp = currentParameterSet;
      std::vector<double> shiftedm = currentParameterSet;
      auto xplus = currentParameterSet[counter] + xacc::constants::pi / 2.;
      auto xminus = currentParameterSet[counter] - xacc::constants::pi / 2.;

      shiftedp[counter] = xplus;
      shiftedm[counter] = xminus;

      results[i]->addExtraInfo("gradient-index", counter);
      results[i+1]->addExtraInfo("gradient-index", counter);

      results[i]->addExtraInfo("shift-direction", "plus");
      results[i+1]->addExtraInfo("shift-direction", "minus");

      results[i]->addExtraInfo("qdist", qp);
      results[i+1]->addExtraInfo("qdist", qm);

      results[i]->addExtraInfo("gradient-parameters", shiftedp);
      results[i+1]->addExtraInfo("gradient-parameters", shiftedm);

      qplus_theta.push_back(qp);
      qminus_theta.push_back(qm);

      counter++;
    }

    for (int i = 0; i < grad.size(); i++) {
      double sum = 0.0;
      for (int x = 0; x < q_dist.size(); x++) {
        if (std::fabs(q_dist[x]) > 1e-12) {
          sum += std::log(q_dist[x] / (0.5 * (target_dist[x] + q_dist[x]))) *
                 0.5 * (qplus_theta[i][x] - qminus_theta[i][x]);
        }
      }
      sum *= 0.5;
      grad[i] = sum;
    }

    return;
  }

  const std::string name() const override { return "js-parameter-shift"; }
  const std::string description() const override { return ""; }
};
} // namespace algorithm
} // namespace xacc
#endif
