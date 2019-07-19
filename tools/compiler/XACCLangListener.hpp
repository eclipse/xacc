/***********************************************************************************
 *
 * Contributors:
 *   Initial implementation - Amrut Nadgir
 *
 **********************************************************************************/
#ifndef XACC_LANG_LISTENER_H
#define XACC_LANG_LISTENER_H

#include "IR.hpp"
#include "IRProvider.hpp"
#include "XACCLangBaseListener.h"

using namespace xacclang;

namespace xacc {


class XACCLangListener : public XACCLangBaseListener {
  std::shared_ptr<IR> ir;
  std::shared_ptr<IRProvider> gateRegistry;
  std::map<std::string, std::shared_ptr<Function>> functions;
  std::shared_ptr<Function> curFunc;

public:
  explicit XACCLangListener(std::shared_ptr<IR>);

  void enterXacckernel(XACCLangParser::XacckernelContext *ctx) override;

  void exitXacckernel(XACCLangParser::XacckernelContext *ctx) override;

  void exitU(XACCLangParser::UContext *ctx) override;

  void exitCX(XACCLangParser::CXContext *ctx) override;

  void exitUserDefGate(XACCLangParser::UserDefGateContext *ctx) override;

  void exitMeasure(XACCLangParser::MeasureContext *ctx) override;

  void exitKernelcall(XACCLangParser::KernelcallContext *ctx) override;
};


} // namespace xacc

#endif
