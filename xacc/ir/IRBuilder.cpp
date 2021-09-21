

#include "IRBuilder.hpp"
#include "xacc_service.hpp"
#include <iostream>
#include "xacc.hpp"

namespace xacc {
IRBuilder::IRBuilder() {
  provider = xacc::getIRProvider("quantum");
  program = provider->createComposite("__internal__builder__xacc__program");
  __init__inst_map();
}

IRBuilder::IRBuilder(const std::string name) {
  provider = xacc::getIRProvider("quantum");
  program = provider->createComposite(name);
  __init__inst_map();
}

void IRBuilder::__init__inst_map() {
  auto insts = xacc::getServices<xacc::Instruction>();
  for (auto inst : insts) {
    if (!inst->isComposite()) {

      std::string data = inst->name();
      std::transform(data.begin(), data.end(), data.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      if (data == "measure") {
        data = "mz";
      }
      method_name_to_xacc_name.insert({data, inst->name()});
    }
  }
}

} // namespace xacc