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
#ifndef XACC_PYXASMLISTENER_H
#define XACC_PYXASMLISTENER_H

#include "IR.hpp"
#include "IRProvider.hpp"
#include "pyxasmBaseListener.h"

using namespace pyxasm;

namespace xacc {

class PyXASMListener : public pyxasmBaseListener {
protected:
  std::shared_ptr<IRProvider> irProvider;
  std::shared_ptr<CompositeInstruction> function;
  std::string bufferName = "";
public:
  PyXASMListener();

  std::shared_ptr<CompositeInstruction> getFunction() {return function;}

  virtual void enterXacckernel(pyxasmParser::XacckernelContext * /*ctx*/) override;
  virtual void enterInstruction(pyxasmParser::InstructionContext * /*ctx*/) override;

};


} // namespace xacc

#endif
