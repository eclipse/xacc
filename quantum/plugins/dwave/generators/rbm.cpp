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
#include "rbm.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include "DWQMI.hpp"

using namespace xacc;
namespace xacc {
namespace dwave {

const std::vector<std::string> RBM::requiredKeys() { return {"nh", "nv"}; }

bool RBM::expand(const xacc::HeterogeneousMap &runtimeOptions) {
  if (!runtimeOptions.keyExists<int>("nh") &&
      !runtimeOptions.keyExists<int>("nv")) {
    return false;
  }

  int n_visible = runtimeOptions.get<int>("nv");
  int n_hidden = runtimeOptions.get<int>("nh");

  for (int i = 0; i < n_visible; i++) {
    std::string paramName = "v" + std::to_string(i);
    addVariable(paramName);
    xacc::InstructionParameter visParam(paramName);
    auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, i, visParam);
    addInstruction(qmi);
  }

  for (int i = n_visible; i < n_visible + n_hidden; i++) {
    std::string paramName = "h" + std::to_string(i);
    addVariable(paramName);
    xacc::InstructionParameter hidParam(paramName);
    auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, i, hidParam);
    addInstruction(qmi);
  }
  for (int i = 0; i < n_visible; i++) {
    for (int j = n_visible; j < n_visible + n_hidden; j++) {
      std::string paramName = "w" + std::to_string(i) + std::to_string(j);
      addVariable(paramName);
      xacc::InstructionParameter wParam(paramName);
      auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, j, wParam);
      addInstruction(qmi);
    }
  }

  parameters.emplace_back(n_visible);
  parameters.emplace_back(n_hidden);

  return true;
} // namespace instructions

} // namespace dwave
} // namespace xacc