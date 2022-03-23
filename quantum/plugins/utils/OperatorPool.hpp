/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#ifndef XACC_ALGORITHM_OPERATOR_POOL_HPP_
#define XACC_ALGORITHM_OPERATOR_POOL_HPP_
#include "Identifiable.hpp"
#include "PauliOperator.hpp"
#include "FermionOperator.hpp"
#include <memory>

namespace xacc {
namespace quantum {

class OperatorPool : public Identifiable {

public:
  // These are methods for custom pool
  virtual void setOperators(const FermionOperator op) {
    XACCLogger::instance()->error(
        "OperatorPool::setOperators(FermionOperator) not implemented for " +
        name());
    exit(0);
    return;
  }

  virtual void setOperators(const PauliOperator op) {
    XACCLogger::instance()->error(
        "OperatorPool::setOperators(PauliOperator) not implemented for " +
        name());
    exit(0);
    return;
  }

  virtual void setOperators(const std::shared_ptr<Observable> op) {
    XACCLogger::instance()->error("OperatorPool::setOperators(shared_ptr<"
                                  "Observable>) not implemented for " +
                                  name());
    exit(0);
    return;
  }

  virtual void setOperators(const std::vector<FermionOperator> op) {
    XACCLogger::instance()->error("OperatorPool::setOperators(vector<"
                                  "FermionOperator>) not implemented for " +
                                  name());
    exit(0);
    return;
  }

  virtual void setOperators(const std::vector<PauliOperator> op) {
    XACCLogger::instance()->error("OperatorPool::setOperators(vector<"
                                  "PauliOperator>) not implemented for " +
                                  name());
    exit(0);
    return;
  }
  // end of custom pool methods

  virtual bool optionalParameters(const HeterogeneousMap parameters) = 0;

  virtual bool isNumberOfParticlesRequired() const { return false; };

  virtual std::vector<std::shared_ptr<Observable>>
  generate(const int &nQubits) = 0;

  virtual std::string operatorString(const int index) = 0;

  virtual std::shared_ptr<CompositeInstruction>
  getOperatorInstructions(const int opIdx, const int varIdx) const = 0;

  virtual double getNormalizationConstant(const int opIdx) const {
    XACCLogger::instance()->error("OperatorPool::getOperatorNorm(int)"
                                  "not implemented for " +
                                  name());
    exit(0);
    return {};
  }

  virtual std::vector<std::shared_ptr<Observable>>
  getExcitationOperators(const int &nQubits) {
    XACCLogger::instance()->error("OperatorPool::getExcitationOperators(int)"
                                  "not implemented for " +
                                  name());
    exit(0);
    return {};
  }
};

} // namespace quantum
} // namespace xacc

#endif