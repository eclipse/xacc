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
 *   Austin Adams - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_ACCELERATORS_IONTRAPPASSESCOMMON_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPPASSESCOMMON_HPP_

#include "CompositeInstruction.hpp"

namespace xacc {
namespace quantum {

typedef std::function<void(std::vector<InstPtr>, std::vector<InstPtr>)> IonTrapLogTransformCallback;

void iontrapFlattenComposite(std::shared_ptr<CompositeInstruction> composite);

}
}

#endif
