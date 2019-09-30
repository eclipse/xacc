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
  double compute(Counts &counts, const std::vector<double>& target) override {
    int shots = 0;
    for (auto &x : counts) {
      shots += x.second;
    }

    // Compute the probability distribution
    std::vector<double> q(target.size()); // all zeros
    for (auto &x : counts) {
      int idx = std::stoi(x.first, nullptr, 2);
      q[idx] = (double) x.second / shots;
    }
    // get M=1/2(P+Q)
    std::vector<double> m(target.size());
    for (int i = 0; i < m.size(); i++)
      m[i] = .5 * (target[i] + q[i]);

    // for (auto& kv : counts) {
    //     std::cout << kv.first <<", " << kv.second << "\n";
    // }
    auto js = 0.5 * (entropy(target, m) + entropy(q, m));
    // xacc::info("JSDiv: " + std::to_string(js));
    return js;
  }
  const std::string name() const override { return "js"; }
  const std::string description() const override { return ""; }
};

}
}
#endif