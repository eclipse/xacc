/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#include "JsonVisitor.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace xacc {

namespace quantum {

using Writer = PrettyWriter<StringBuffer>;

template <class W, class B>
JsonVisitor<W, B>::JsonVisitor(std::shared_ptr<xacc::CompositeInstruction> f)
    : buffer(std::make_shared<B>()),
      writer(std::make_shared<W>(*buffer.get())), functions{f} {}

template <class W, class B>
JsonVisitor<W, B>::JsonVisitor(std::vector<std::shared_ptr<xacc::CompositeInstruction>> fs)
    : buffer(std::make_shared<B>()), writer(std::make_shared<W>(*buffer.get())),
      functions(fs) {}

template <class W, class B> std::string JsonVisitor<W, B>::write() {
  writer->StartObject();

  writer->String("circuits");
  writer->StartArray();
  for (auto f : functions) {
    // This is a Function, start it as an Object
    writer->StartObject();

    writer->String("circuit");
    writer->String(f->name());

    // All functions have instructions, start
    // that array here.
    writer->String("instructions");
    writer->StartArray();

    topLevelInstructionIterator =
        std::make_shared<xacc::InstructionIterator>(f);
    while (topLevelInstructionIterator->hasNext()) {
      // Get the next node in the tree
      auto nextInst = topLevelInstructionIterator->next();
      nextInst->accept(this);
    }

    // End Instructions
    writer->EndArray();

    writer->String("variables");
    writer->StartArray();
    for (auto& v : f->getVariables()) {
        writer->String(v);
    }
    writer->EndArray();

    writer->String("coefficient");
    writer->Double(std::real(f->getCoefficient()));
    writer->String("accelerator_signature");
    writer->String(f->accelerator_signature());
    // End Function
    writer->EndObject();
  }
  writer->EndArray();
  writer->EndObject();
  return buffer->GetString();
}

template<class W, class B> void JsonVisitor<W,B>::visit(Circuit& inst) {
  if (!inst.hasChildren()) {
  writer->StartObject();
  writer->String("gate");
  writer->String(inst.name().c_str());
  writer->String("enabled");
  writer->Bool(inst.isEnabled());
  writer->String("composite");
  writer->Bool(true);
  writer->String("qubits");
  writer->StartArray();
//   for (auto qi : inst.bits()) {
//     writer->Int(qi);
//   }
  writer->EndArray();

  writer->String("parameters");
  writer->StartArray();
  writer->EndArray();
  writer->String("variables");
  for (auto v : inst.getVariables()) {
      writer->String(v);
  }
  writer->StartArray();

  writer->EndArray();
    writer->EndObject();
  }
}

template <class W, class B> void JsonVisitor<W, B>::visit(Rz &rz) {
  baseGateInst(dynamic_cast<Gate &>(rz));
}

template <class W, class B> void JsonVisitor<W, B>::visit(Rx &rx) {
  baseGateInst(dynamic_cast<Gate &>(rx));
}

template <class W, class B> void JsonVisitor<W, B>::visit(Ry &ry) {
  baseGateInst(dynamic_cast<Gate &>(ry));
}

template <class W, class B> void JsonVisitor<W, B>::visit(CPhase &cp) {
  baseGateInst(dynamic_cast<Gate &>(cp));
}

template <class W, class B> void JsonVisitor<W, B>::visit(Measure &cn) {
  baseGateInst(dynamic_cast<Gate &>(cn));
}

template <class W, class B> void JsonVisitor<W, B>::visit(U &u) {
  baseGateInst(dynamic_cast<Gate &>(u));
}

// template<class W, class B>
// void JsonVisitor<W, B>::visit(IRGenerator& inst) {
//       writer->StartObject();
//   writer->String("gate");
//   writer->String(inst.name().c_str());
//   writer->String("enabled");
//   writer->Bool(inst.isEnabled());
//   writer->String("qubits");
//   writer->StartArray();
//   for (auto qi : inst.bits()) {
//     writer->Int(qi);
//   }
//   writer->EndArray();

//   writer->String("parameters");
//   writer->StartArray();
//   for (auto p : inst.getParameters()) {
//     switch (p.which()) {
//     case 0:
//       writer->Int(p.as<int>());
//       break;
//     case 1:
//       writer->Double(p.as<double>());
//       break;
//     case 2:
//       writer->String(p.as<std::string>());
//       break;
//     default:
//       writer->String(p.toString());
//     }
//   }
//   writer->EndArray();

//   writer->String("options");
//   writer->StartObject();
//   for (auto& kv : inst.getOptions()) {
//       writer->Key(kv.first);
//       auto p = kv.second;
//       switch (p.which()) {
//     case 0:
//       writer->Int(p.as<int>());
//       break;
//     case 1:
//       writer->Double(p.as<double>());
//       break;
//     case 2:
//       writer->String(p.as<std::string>());
//       break;
//     default:
//       writer->String(p.toString());
//     }
//   }
//   writer->EndObject();

//   writer->EndObject();
// }


template <class W, class B>
void JsonVisitor<W, B>::baseGateInst(Gate &inst, bool endObject) {
  writer->StartObject();
  writer->String("gate");
  writer->String(inst.name().c_str());
  writer->String("enabled");
  writer->Bool(inst.isEnabled());
  writer->String("composite");
  writer->Bool(false);
  writer->String("qubits");
  writer->StartArray();
  for (auto qi : inst.bits()) {
    writer->Int(qi);
  }
  writer->EndArray();

  writer->String("parameters");
  writer->StartArray();
  for (auto p : inst.getParameters()) {
    switch (p.which()) {
    case 0:
      writer->Int(p.as<int>());
      break;
    case 1:
      writer->Double(p.as<double>());
      break;
    case 2:
      writer->String(p.as<std::string>());
      break;
    default:
      writer->String(p.toString());
    }
  }
  writer->EndArray();

  if (endObject) {
    writer->EndObject();
  }
}
template class JsonVisitor<Writer, StringBuffer>;

} // namespace quantum
} // namespace xacc
