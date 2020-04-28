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
#ifndef XACC_RIGETTI_QUILTOXACCLISTENER_H
#define XACC_RIGETTI_QUILTOXACCLISTENER_H

#include "QuilBaseListener.h"
#include "IR.hpp"
#include "IRProvider.hpp"
#include "expression_parsing_util.hpp"

using namespace quil;

namespace xacc {
namespace quantum {
class QuilToXACCListener : public QuilBaseListener {
protected:
  std::shared_ptr<IRProvider> gateRegistry;
  std::shared_ptr<CompositeInstruction> function;
  std::shared_ptr<ExpressionParsingUtil> parsingUtil;

public:
  QuilToXACCListener();
  std::shared_ptr<CompositeInstruction> getFunction() {return function;}
  void enterXacckernel(QuilParser::XacckernelContext *ctx) override;

  void exitGate(quil::QuilParser::GateContext *ctx) override;

  void exitKernelcall(
    quil::QuilParser::KernelcallContext *ctx) override;

  void exitMeasure(quil::QuilParser::MeasureContext *ctx) override;

};
} // namespace quantum
} // namespace xacc

#endif
