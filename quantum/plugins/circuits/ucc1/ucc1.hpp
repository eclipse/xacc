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
#ifndef XACC_GENERATORS_UCC1_HPP_
#define XACC_GENERATORS_UCC1_HPP_

#include "Circuit.hpp"

namespace xacc {
namespace circuits {
class UCC1 : public xacc::quantum::Circuit {
public:
  UCC1() : Circuit("ucc1") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(UCC1);
};
} // namespace circuits
} // namespace xacc
#endif