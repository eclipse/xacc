/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "IRBuilder.hpp"
#include "xacc_service.hpp"
#include <iostream>
#include "xacc.hpp"

namespace xacc {
IRBuilder::IRBuilder() {
  provider = xacc::getIRProvider("quantum");
  program = provider->createComposite("__internal__builder__xacc__program");
  __init__inst_map();
}

IRBuilder::IRBuilder(const std::string name) {
  provider = xacc::getIRProvider("quantum");
  program = provider->createComposite(name);
  __init__inst_map();
}

void IRBuilder::__init__inst_map() {
  auto insts = xacc::getServices<xacc::Instruction>();
  for (auto inst : insts) {
    if (!inst->isComposite()) {

      std::string data = inst->name();
      std::transform(data.begin(), data.end(), data.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      if (data == "measure") {
        data = "mz";
      }
      method_name_to_xacc_name.insert({data, inst->name()});
    }
  }
}

} // namespace xacc