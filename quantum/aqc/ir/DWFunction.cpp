
#include "DWFunction.hpp"
#include "xacc_service.hpp"

#include "IRGenerator.hpp"
#include "IRProvider.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

namespace xacc {
namespace quantum {

std::shared_ptr<Graph> DWFunction::toGraph() {
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
  for (int i = 0; i < maxBit + 1; i++) {
    std::map<std::string, InstructionParameter> props{{"bias", 0.0}};
    graph->addVertex(props); //
  }

  for (int i = 0; i < nInstructions(); ++i) {
    auto inst = getInstruction(i);
    auto bits = inst->bits();
    if (bits[0] == bits[1]) {
      auto p = inst->getParameter(0);
      graph->setVertexProperty(bits[0], "bias", p);
    } else {
      graph->addEdge(bits[0], bits[1], inst->getParameter(0).as<double>());
    }
  }

  return graph;
}

void DWFunction::persist(std::ostream &outStream) {
  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  writer.StartObject(); // start root object
  writer.String("function");
  writer.String(name());

  writer.String("instructions");
  writer.StartArray();

  for (auto &i : getInstructions()) {
    writer.StartObject();
    writer.String("instruction");
    writer.String(i->name().c_str());
    writer.String("enabled");
    writer.Bool(i->isEnabled());
    writer.String("qubits");
    writer.StartArray();
    for (auto qi : i->bits()) {
      writer.Int(qi);
    }
    writer.EndArray();

    writer.String("parameters");
    writer.StartArray();
    for (auto p : i->getParameters()) {
      switch (p.which()) {
      case 0:
        writer.Int(p.as<int>());
        break;
      case 1:
        writer.Double(p.as<double>());
        break;
      case 2:
        writer.String(p.as<std::string>());
        break;
      default:
        writer.String(p.toString());
      }
    }
    writer.EndArray();

    // writer.String("options");
    // writer.StartObject();
    // for (auto &kv : i->getOptions()) {
    //   writer.Key(kv.first);
    //   auto p = kv.second;
    //   switch (p.which()) {
    //   case 0:
    //     writer.Int(p.as<int>());
    //     break;
    //   case 1:
    //     writer.Double(p.as<double>());
    //     break;
    //   case 2:
    //     writer.String(p.as<std::string>());
    //     break;
    //   default:
    //     writer.String(p.toString());
    //   }
    // }
    // writer.EndObject();

    writer.EndObject();
  }

  // End Instructions
  writer.EndArray();

  // End Function
  writer.EndObject();

  outStream << buffer.GetString();

}

void DWFunction::load(std::istream &inStream) {

  std::vector<std::string> irGeneratorNames;
  auto irgens = xacc::getRegisteredIds<xacc::IRGenerator>();
  for (auto &irg : irgens) {
    irGeneratorNames.push_back(irg);
  }

  auto provider = xacc::getService<IRProvider>("quantum");
  std::string json(std::istreambuf_iterator<char>(inStream), {});
//   std::cout << "JSON: " << json << "\n";

  Document doc;
  doc.Parse(json);

//   auto &kernel = doc["kernels"].GetArray()[0];
  this->_name = doc["function"].GetString();
  auto instructionsArray = doc["instructions"].GetArray();

  for (int i = 0; i < instructionsArray.Size(); i++) {
    auto &inst = instructionsArray[i];
    auto gname = inst["instruction"].GetString();
    if (std::string(gname) == "dw-qmi") {
        gname = "qmi";
    }
    bool isAnIRG = false;
    if (std::find(irGeneratorNames.begin(), irGeneratorNames.end(), gname) != irGeneratorNames.end()) {
        // this is an IRG
        isAnIRG = true;
    }

    std::vector<int> qbits;
    auto bitsArray = inst["qubits"].GetArray();
    for (int k = 0; k < bitsArray.Size(); k++) {
      qbits.push_back(bitsArray[k].GetInt());
    }

    std::vector<InstructionParameter> local_parameters;
    auto &paramsArray = inst["parameters"];
    for (int k = 0; k < paramsArray.Size(); k++) {
      auto &value = paramsArray[k];
      if (value.IsInt()) {
        local_parameters.push_back(InstructionParameter(value.GetInt()));
      } else if (value.IsDouble()) {
        local_parameters.push_back(InstructionParameter(value.GetDouble()));
      } else {
        local_parameters.push_back(InstructionParameter(value.GetString()));
      }
    }

    std::shared_ptr<Instruction> instToAdd;
    if (!isAnIRG) {
     instToAdd =
        provider->createInstruction(gname, qbits, local_parameters);
    } else {
        instToAdd = xacc::getService<IRGenerator>(gname);
    }

    // auto &optionsObj = inst["options"];
    // for (auto itr = optionsObj.MemberBegin(); itr != optionsObj.MemberEnd();
    //      ++itr) {
    //   auto &value = optionsObj[itr->name.GetString()];

    //   if (value.IsInt()) {
    //     instToAdd->setOption(itr->name.GetString(),
    //                          InstructionParameter(value.GetInt()));
    //   } else if (value.IsDouble()) {
    //     instToAdd->setOption(itr->name.GetString(),
    //                          InstructionParameter(value.GetDouble()));
    //   } else {
    //     instToAdd->setOption(itr->name.GetString(),
    //                          InstructionParameter(value.GetString()));
    //   }
    // }
    if (!inst["enabled"].GetBool()) {
      instToAdd->disable();
    }

    addInstruction(instToAdd);
  }
}

} // namespace quantum
} // namespace xacc
