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
#include "CommonGates.hpp"
#include "Instruction.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include "DWQMI.hpp"
#include "CommonGates.hpp"

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

void RBMAsCircuitType::applyRuntimeArguments() {

  // We expect args like this
  // rbm(v, x, nv, nh);

  std::string variable_name = arguments[0]->name;

  std::vector<double> x_vals;
  if (arguments[0]->type.find("std::vector<double>") != std::string::npos) {
    x_vals = arguments[0]->runtimeValue.get<std::vector<double>>(
        INTERNAL_ARGUMENT_VALUE_KEY);
  } else {
    xacc::error("Has to be a vector of double");
  }

  auto n_visible = arguments[1]->runtimeValue.get<int>(INTERNAL_ARGUMENT_VALUE_KEY);
  auto n_hidden = arguments[2]->runtimeValue.get<int>(INTERNAL_ARGUMENT_VALUE_KEY);

  std::cout << "HELLO WORLD: " << x_vals << ", " << n_visible << ", " << n_hidden << "\n";

  if (nInstructions() > 0) {
      clear();
      parameters.clear();
  }

  for (std::size_t i = 0; i < n_visible; i++) {
    // std::string paramName = "v" + std::to_string(i);
    // addVariable(paramName);
    auto qmi = std::make_shared<xacc::quantum::AnnealingInstruction>(i, x_vals[i]);
    addInstruction(qmi);
  }

  for (std::size_t i = n_visible; i < n_visible + n_hidden; i++) {
    // std::string paramName = "h" + std::to_string(i);
    // addVariable(paramName);
    // xacc::InstructionParameter hidParam(paramName);
    auto qmi = std::make_shared<xacc::quantum::AnnealingInstruction>(i, x_vals[i]);
    addInstruction(qmi);
  }

  int counter = n_visible+n_hidden;
  for (std::size_t i = 0; i < n_visible; i++) {
    for (std::size_t j = n_visible; j < n_visible + n_hidden; j++) {
    //   std::string paramName = "w" + std::to_string(i) + std::to_string(j);
    //   addVariable(paramName);
    //   xacc::InstructionParameter wParam(paramName);
      auto qmi = std::make_shared<xacc::quantum::AnnealingInstruction>(i, j, x_vals[counter]);
      addInstruction(qmi);
      counter++;
    }
  }


  parameters.emplace_back(n_visible);
  parameters.emplace_back(n_hidden);
}

} // namespace dwave
} // namespace xacc