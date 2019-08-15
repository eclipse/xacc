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
#ifndef XACC_IR_OBSERVABLE_HPP_
#define XACC_IR_OBSERVABLE_HPP_
#include "Function.hpp"
#include "Utils.hpp"

namespace xacc {

class Observable : public Identifiable {
public:
  virtual std::vector<std::shared_ptr<Function>>
  observe(std::shared_ptr<Function> function) = 0;
  virtual const std::string toString() = 0;
  virtual void fromString(const std::string str) = 0;
  virtual const int nBits() = 0;
  virtual void fromOptions(std::map<std::string, InstructionParameter>&& options) {
      XACCLogger::instance()->error("Observable.fromOptions not implemented.");
      return;
  }
  virtual void fromOptions(std::map<std::string, InstructionParameter>& options) {
      XACCLogger::instance()->error("Observable.fromOptions not implemented.");
      return;
  }
};

} // namespace xacc
#endif