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

namespace xacc {
class AcceleratorBuffer;
class Observable;
namespace internal_compiler {
using qubit = std::pair<std::string, std::size_t>;

class qreg {
protected:
  AcceleratorBuffer *buffer;

public:
  qreg() = default;
  qreg(const int n);
  qreg (const qreg& other);
  qubit operator[](const std::size_t i);
  qreg& operator=(const qreg& q);
  AcceleratorBuffer *results();
  std::map<std::string, int> counts();
  double exp_val_z();
  void reset();
  int size();
  void addChild(qreg& q);
  void setName(const char *name);
  void setNameAndStore(const char *name);
  std::string name();
  void store();
  void print();
  double weighted_sum(Observable* obs);
};

} // namespace internal_compiler
} // namespace xacc

xacc::internal_compiler::qreg qalloc(const int n) {
  return xacc::internal_compiler::qreg(n);
}

// __qpu__ indicates this functions is for the QCOR Clang Syntax Handler
// and annotated with quantum for the LLVM IR CodeGen
#define __qpu__ [[clang::syntax(qcor)]] __attribute__((annotate("quantum")))

#endif