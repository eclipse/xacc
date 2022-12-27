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
#ifndef XACC_QUANTUM_GATE_API_HPP_
#define XACC_QUANTUM_GATE_API_HPP_
#include "Observable.hpp"

namespace xacc {
namespace quantum {

const std::vector<std::string> CHEM_OBS_PLUGINS = {"pyscf", "psi4"};

std::shared_ptr<Observable> getObservable();
std::shared_ptr<Observable> getObservable(const std::string type);
std::shared_ptr<Observable> getObservable(const std::string type,
                                          const std::string repr);
std::shared_ptr<Observable> getObservable(const std::string type,
                                          const HeterogeneousMap &options);
std::shared_ptr<Observable> getObservable(const std::string type,
                                          const HeterogeneousMap &&options);
} // namespace quantum
} // namespace xacc

#endif