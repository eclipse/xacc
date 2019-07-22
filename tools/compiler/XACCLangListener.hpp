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
protected:
  std::shared_ptr<IR> ir;
  std::shared_ptr<IRProvider> gateRegistry;
  std::map<std::string, std::shared_ptr<Function>> functions;
  std::shared_ptr<Function> curFunc;

public:
  XACCLangListener(std::shared_ptr<IR>);

  std::shared_ptr<Function> getFunction() {return curFunc;}

  void enterXacckernel(XACCLangParser::XacckernelContext *ctx) override;

  void exitXacckernel(XACCLangParser::XacckernelContext *ctx) override;

  virtual void enterUop(XACCLangParser::UopContext * /*ctx*/) override;

};


} // namespace xacc

#endif
