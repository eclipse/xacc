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

  void exitKernelcall(XACCLangParser::KernelcallContext *ctx) override;

  virtual void enterUop(XACCLangParser::UopContext * /*ctx*/) override;

  virtual void
  enterAllbitsOp(XACCLangParser::AllbitsOpContext * /*ctx*/) override;
};


} // namespace xacc

#endif
