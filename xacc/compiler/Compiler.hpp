/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef XACC_COMPILER_HPP_
#define XACC_COMPILER_HPP_

#include <memory>
#include "IR.hpp"
#include "Accelerator.hpp"

namespace xacc {

// Compilers provide an API for mapping xacc kernels to an instance
// of the intermediate representation.
class Compiler : public OptionsProvider, public Identifiable {

public:
  virtual std::shared_ptr<IR> compile(const std::string &src,
                                      std::shared_ptr<Accelerator> acc) = 0;
  virtual std::shared_ptr<IR> compile(const std::string &src) = 0;

  virtual const std::string
  translate(std::shared_ptr<CompositeInstruction> CompositeInstruction) = 0;

  virtual const std::shared_ptr<CompositeInstruction>
  compile(std::shared_ptr<CompositeInstruction> f,
          std::shared_ptr<Accelerator> acc) {
    XACCLogger::instance()->info(
        "Compiler::compile(CompositeInstruction, accelerator) not "
        "implemented. Returning given CompositeInstruction.");
    return f;
  }

  virtual OptionPairs getOptions() { return OptionPairs{}; }
  virtual bool
  handleOptions(const std::map<std::string, std::string> &arg_map) {
    return false;
  }

  virtual ~Compiler() {}
};

} // namespace xacc
#endif
