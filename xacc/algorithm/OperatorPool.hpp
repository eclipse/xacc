/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/

#ifndef XACC_OPERATOR_POOL_HPP_
#define XACC_OPERATOR_POOL_HPP_

#include "Identifiable.hpp"
#include <memory>

namespace xacc {

class OperatorPool : public Identifiable {

public:

  virtual bool optionalParameters(const HeterogeneousMap parameters) = 0;

  virtual std::vector<std::shared_ptr<Observable>>
  generate(const int &nQubits) = 0;

  virtual std::string operatorString(const int index) = 0;

  virtual std::shared_ptr<CompositeInstruction>
   getOperatorInstructions(const int opIdx, const int varIdx) const = 0;
};

}

#endif