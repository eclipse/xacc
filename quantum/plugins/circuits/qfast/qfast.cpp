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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/

#include "qfast.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace circuits {
QFAST::QFAST():
    xacc::quantum::Circuit("QFAST")
{}

bool QFAST::expand(const xacc::HeterogeneousMap& runtimeOptions) 
{
    // TODO
    return true;
}

const std::vector<std::string> QFAST::requiredKeys()
{
   // TODO
   return {};
}
}
}