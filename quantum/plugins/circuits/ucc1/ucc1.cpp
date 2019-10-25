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
#include "ucc1.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"
#include <regex>

using namespace xacc;

namespace xacc {
namespace circuits {
const std::vector<std::string> UCC1::requiredKeys() { return {}; }
bool UCC1::expand(const HeterogeneousMap &parameters) {

//   std::string paramLetter = "x";
//   if (parameters.stringExists("param_id")) {
//     paramLetter = parameters.getString("param_id");
//   }

//   addVariable(paramLetter + "0");


  if (nVariables() != 1) {
      return false;
  }

  auto provider = xacc::getService<IRProvider>("quantum");
  auto rx1 = provider->createInstruction("Rx", {0}, {xacc::constants::pi / 2.});
  auto h1 = provider->createInstruction("H", {1});
  auto h2 = provider->createInstruction("H", {2});
  auto h3 = provider->createInstruction("H", {3});
  auto cn1 = provider->createInstruction("CX", {0, 1});
  auto cn2 = provider->createInstruction("CX", {1, 2});
  auto cn3 = provider->createInstruction("CX", {2, 3});
  auto rz = provider->createInstruction("Rz", {3}, {getVariables()[0]});
  auto cn4 = provider->createInstruction("CX", {2, 3});
  auto cn5 = provider->createInstruction("CX", {1, 2});
  auto cn6 = provider->createInstruction("CX", {0, 1});

  auto rx2 =
      provider->createInstruction("Rx", {0}, {xacc::constants::pi / -2.});
  auto h4 = provider->createInstruction("H", {1});
  auto h5 = provider->createInstruction("H", {2});
  auto h6 = provider->createInstruction("H", {3});

  addInstruction(rx1);
  addInstruction(h1);
  addInstruction(h2);
  addInstruction(h3);
  addInstruction(cn1);
  addInstruction(cn2);
  addInstruction(cn3);
  addInstruction(rz);
  addInstruction(cn4);
  addInstruction(cn5);
  addInstruction(cn6);
  addInstruction(rx2);
  addInstruction(h4);
  addInstruction(h5);
  addInstruction(h6);

  return true;
}

} // namespace circuits
} // namespace xacc