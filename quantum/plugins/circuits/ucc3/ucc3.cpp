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
#include "ucc3.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"
#include <regex>

using namespace xacc;

namespace xacc {
namespace circuits {
const std::vector<std::string> UCC3::requiredKeys() {
  return {};
}
bool UCC3::expand(const HeterogeneousMap &parameters) {

//   std::string paramLetter = "x";
//   if (parameters.stringExists("param_id")) {
//     paramLetter = parameters.getString("param_id");
//   }
//   addVariable(paramLetter + "0");
//   addVariable(paramLetter + "1");
//   addVariable(paramLetter + "2");

  if (nVariables() != 3) {
      return false;
  }
  
  auto provider = xacc::getService<IRProvider>("quantum");

  auto rx0 = provider->createInstruction("Rx", {0}, {xacc::constants::pi / 2.});
  auto h0 = provider->createInstruction("H", {1});
  auto cn0 = provider->createInstruction("CX", {0, 1});
  auto rz0 = provider->createInstruction("Rz", {1}, {getVariables()[0]});
  auto rx1 = provider->createInstruction("Rx", {2}, {xacc::constants::pi / 2.});
  auto h1 = provider->createInstruction("H", {3});
  auto cn1 = provider->createInstruction("CX", {2, 3});
  auto rz1 = provider->createInstruction("Rz", {3}, {getVariables()[1]});
  auto cn2 = provider->createInstruction("CX", {2, 3});
  auto rx2 =
      provider->createInstruction("Rx", {2}, {xacc::constants::pi / -2.});
  auto h2 = provider->createInstruction("H", {2});

  auto cn3 = provider->createInstruction("CX", {1, 2});
  auto cn4 = provider->createInstruction("CX", {2, 3});
  auto rz2 = provider->createInstruction("Rz", {3}, {getVariables()[2]});
  auto cn5 = provider->createInstruction("CX", {2, 3});
  auto cn6 = provider->createInstruction("CX", {1, 2});
  auto cn7 = provider->createInstruction("CX", {0, 1});
  auto rx3 =
      provider->createInstruction("Rx", {0}, {xacc::constants::pi / -2.});
  auto h4 = provider->createInstruction("H", {1});
  auto h5 = provider->createInstruction("H", {2});
  auto h6 = provider->createInstruction("H", {3});

  addInstruction(rx0);
  addInstruction(h0);
  addInstruction(cn0);
  addInstruction(rz0);
  addInstruction(rx1);
  addInstruction(h1);
  addInstruction(cn1);
  addInstruction(rz1);
  addInstruction(cn2);
  addInstruction(rx2);
  addInstruction(h2);
  addInstruction(cn3);
  addInstruction(cn4);
  addInstruction(rz2);
  addInstruction(cn5);
  addInstruction(cn6);
  addInstruction(cn7);
  addInstruction(rx3);
  addInstruction(h4);
  addInstruction(h5);
  addInstruction(h6);

  return true;
}

} // namespace circuits
} // namespace xacc