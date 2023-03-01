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
#include "xacc_quantum_gate_api.hpp"
#include "PauliOperator.hpp"
#include "FermionOperator.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include <xacc_service.hpp>

namespace xacc {
namespace quantum {

std::shared_ptr<Observable> getObservable(const std::string type,
                                          const std::string representation) {
  if (type == "pauli") {
    return representation.empty()
               ? std::make_shared<PauliOperator>()
               : std::make_shared<PauliOperator>(representation);
  } else if (type == "fermion") {
    return representation.empty()
               ? std::make_shared<FermionOperator>()
               : std::make_shared<FermionOperator>(representation);
  } else if (xacc::hasService<Observable>(type)) {
    auto obs = xacc::getService<Observable>(type);
    return obs;
  } else {
    if (xacc::hasContributedService<Observable>(type)) {
      auto obs = xacc::getContributedService<Observable>(type);
      return obs;
    }
    xacc::error("[xacc::getObservable()] Invalid observable type: " + type);
    return std::make_shared<PauliOperator>();
  }
}

std::shared_ptr<Observable> getObservable() {
  return getObservable("pauli", std::string(""));
}
std::shared_ptr<Observable> getObservable(const std::string type) {
  return getObservable(type, std::string(""));
}

std::shared_ptr<Observable> getObservable(const std::string type,
                                          const HeterogeneousMap &options) {
  auto observable = getObservable(type);
  observable->fromOptions(options);
  if(xacc::container::contains(CHEM_OBS_PLUGINS, type)) {
    observable = std::make_shared<FermionOperator>(observable->toString());
  }
  return observable;
}
std::shared_ptr<Observable> getObservable(const std::string type,
                                          const HeterogeneousMap &&options) {
  return getObservable(type, options);
}

} // namespace quantum
} // namespace xacc