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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_QALLOC_HPP_
#define XACC_QALLOC_HPP_

#include <map>
#include <string>

namespace xacc {
class AcceleratorBuffer;
namespace internal_compiler {

class qreg {
protected:
  AcceleratorBuffer *buffer;

public:
  qreg(const int n);
  int operator[](const int &i);
  AcceleratorBuffer *results();
  std::map<std::string, int> counts();
  double exp_val_z();
  void reset();
};

} // namespace internal_compiler
} // namespace xacc

xacc::internal_compiler::qreg qalloc(const int n) {
  return xacc::internal_compiler::qreg(n);
}

#endif