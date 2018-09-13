/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#ifndef XACC_IBM_DWQMILISTENER_H
#define XACC_IBM_DWQMILISTENER_H

#include <PyXACCIRBaseListener.h>
#include "IR.hpp"
#include "IRProvider.hpp"
#include <boost/algorithm/string.hpp>

using namespace pyxacc;

namespace xacc {
namespace quantum {

class DWorGateListener : public PyXACCIRBaseListener {
public:
  bool isDW = false;
  void enterGate(PyXACCIRParser::GateContext *ctx) override;
};

/**
 * The PyXACCListener implements the Antlr-generated
 * PyXACCIRBaseListener and maps the Antlr AST to XACC IR.
 */
class PyXACCListener : public PyXACCIRBaseListener {
protected:
  std::shared_ptr<Function> f;
  std::shared_ptr<IRProvider> provider;
  std::vector<std::string> functionVariableNames;
  bool useDW = false;

public:
  std::shared_ptr<Function> getKernel();

  PyXACCListener();
  PyXACCListener(bool useDw);

  virtual void
  enterXacckernel(PyXACCIRParser::XacckernelContext * /*ctx*/) override;

  virtual void enterUop(PyXACCIRParser::UopContext * /*ctx*/) override;
};

} // namespace quantum

} // namespace xacc

#endif
