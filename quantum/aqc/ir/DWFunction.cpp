
#include "DWFunction.hpp"
#include "xacc_service.hpp"


namespace xacc {
namespace quantum {

  std::shared_ptr<Graph> DWFunction::toGraph()  {
    int maxBit = 0;
    for (int i = 0; i < nInstructions(); ++i) {
      auto inst = getInstruction(i);
      auto bits = inst->bits();
      if (bits[0] > maxBit) {
        maxBit = bits[0];
      }
      if (bits[1] > maxBit) {
        maxBit = bits[1];
      }
    }

    auto graph = xacc::getService<Graph>("boost-ugraph");
    for (int i = 0; i < maxBit+1; i++) {
     std::map<std::string,InstructionParameter> props{{"bias",0.0}};
        graph->addVertex(props);//
    }

    for (int i = 0; i < nInstructions(); ++i) {
      auto inst = getInstruction(i);
      auto bits = inst->bits();
      if (bits[0] == bits[1]) {
        auto p = inst->getParameter(0);
        graph->setVertexProperty(bits[0], "bias", p);
      } else {
        graph->addEdge(bits[0], bits[1],
                      inst->getParameter(0).as<double>());
      }
    }

    return graph;
  }


} // namespace quantum
} // namespace xacc
