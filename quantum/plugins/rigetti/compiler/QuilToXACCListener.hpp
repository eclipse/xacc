
#ifndef XACC_RIGETTI_QUILTOXACCLISTENER_H
#define XACC_RIGETTI_QUILTOXACCLISTENER_H

#include "QuilBaseListener.h"
#include "IR.hpp"
#include "IRProvider.hpp"

using namespace quil;

namespace xacc {
namespace quantum {
class QuilToXACCListener : public QuilBaseListener {
protected:
  std::shared_ptr<IRProvider> gateRegistry;
  std::shared_ptr<CompositeInstruction> function;

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
