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

/**
 * FIXME write this
 */
template <class W, class B>
JsonVisitor<W, B>::JsonVisitor(std::shared_ptr<xacc::Function> f)
    : buffer(std::make_shared<B>()),
      writer(std::make_shared<W>(*buffer.get())), functions{f} {}

template <class W, class B>
JsonVisitor<W, B>::JsonVisitor(std::vector<std::shared_ptr<xacc::Function>> fs)
    : buffer(std::make_shared<B>()), writer(std::make_shared<W>(*buffer.get())),
      functions(fs) {}

template <class W, class B> std::string JsonVisitor<W, B>::write() {
  writer->StartObject();

  writer->String("kernels");
  writer->StartArray();
  for (auto f : functions) {
    // This is a Function, start it as an Object
    writer->StartObject();

    writer->String("function");
    writer->String(f->name());

    if (f->hasBeenBitMapped()) {
        writer->String("bitmap");
        writer->StartArray();
        for (auto& b : f->getBitMap()) {
            writer->Int(b);
        }
        writer->EndArray();
    }
    
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

    // End Function
    writer->EndObject();
  }
  writer->EndArray();
  writer->EndObject();
  return buffer->GetString();
}

template <class W, class B> void JsonVisitor<W, B>::visit(Rz &rz) {
  baseGateInst(dynamic_cast<GateInstruction &>(rz));
//   writer->String("angle");
//   auto p = rz.getParameter(0);
//   switch (p.which()) {
//   case 0:
//     writer->Int(p.as<int>());
//     break;
//   case 1:
//     writer->Double(p.as<double>());
//     break;
//   case 2:
//     writer->String(p.as<std::string>());
//     break;
//   default:
//     xacc::error("Invalid InstructionParameter: " + p.toString());
//   }
//   writer->EndObject();
}

template <class W, class B> void JsonVisitor<W, B>::visit(Rx &rx) {
  baseGateInst(dynamic_cast<GateInstruction &>(rx));
//   writer->String("angle");
//   auto p = rx.getParameter(0);
//   switch (p.which()) {
//   case 0:
//     writer->Int(p.as<int>());
//     break;
//   case 1:
//     writer->Double(p.as<double>());
//     break;
//   case 2:
//     writer->String(p.as<std::string>());
//     break;
//   default:
//     xacc::error("Invalid InstructionParameter: " + p.toString());
//   }
//   writer->EndObject();
}

template <class W, class B> void JsonVisitor<W, B>::visit(Ry &ry) {
  baseGateInst(dynamic_cast<GateInstruction &>(ry));
//   writer->String("angle");
//   auto p = ry.getParameter(0);

//   switch (p.which()) {
//   case 0:
//     writer->Int(p.as<int>());
//     break;
//   case 1:
//     writer->Double(p.as<double>());
//     break;
//   case 2:
//     writer->String(p.as<std::string>());
//     break;
//   default:
//     xacc::error("Invalid InstructionParameter: " + p.toString());
//   }
//   writer->EndObject();
}

template <class W, class B> void JsonVisitor<W, B>::visit(CPhase &cp) {
  baseGateInst(dynamic_cast<GateInstruction &>(cp));
//   writer->String("angle");
//   auto p = cp.getParameter(0);

//   switch (p.which()) {
//   case 0:
//     writer->Int(p.as<int>());
//     break;
//   case 1:
//     writer->Double(p.as<double>());
//     break;
//   case 2:
//     writer->String(p.as<std::string>());
//     break;
//   default:
//     xacc::error("Invalid InstructionParameter: " + p.toString());
//   }
//   writer->EndObject();
}

template <class W, class B>
void JsonVisitor<W, B>::visit(ConditionalFunction &cn) {
  writer->StartObject();
  writer->String("conditional_function");
  writer->String(cn.name());

  writer->String("conditional_qubit");
  writer->Int(cn.getConditionalQubit());
  writer->String("instructions");
  writer->StartArray();

  auto cnAsPtr = std::make_shared<ConditionalFunction>(cn);
  int nInsts = cnAsPtr->nInstructions();
  xacc::InstructionIterator it(cnAsPtr);
  it.next();
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    nextInst->accept(this);
  }

  // End Instructions
  writer->EndArray();
  writer->EndObject();

  // Move the Top Level Iterator past these instructions that were in
  // the conditional function
  for (int i = 0; i < nInsts; i++)
    topLevelInstructionIterator->next();
}

template <class W, class B> void JsonVisitor<W, B>::visit(Measure &cn) {
  baseGateInst(dynamic_cast<GateInstruction &>(cn));
//   writer->String("classicalBitIdx");
//   writer->Int(cn.getClassicalBitIndex());
//   writer->EndObject();
}

template <class W, class B> void JsonVisitor<W, B>::visit(U &u) {
  baseGateInst(dynamic_cast<GateInstruction &>(u));
//   writer->String("theta");
//   auto p = u.getParameter(0);

//   switch (p.which()) {
//   case 0:
//     writer->Int(p.as<int>());
//     break;
//   case 1:
//     writer->Double(p.as<double>());
//     break;
//   case 2:
//     writer->String(p.as<std::string>());
//     break;
//   default:
//     xacc::error("Invalid InstructionParameter: " + p.toString());
//   }
//   writer->String("phi");
//   auto p2 = u.getParameter(1);

//   switch (p.which()) {
//   case 0:
//     writer->Int(p.as<int>());
//     break;
//   case 1:
//     writer->Double(p.as<double>());
//     break;
//   case 2:
//     writer->String(p.as<std::string>());
//     break;
//   default:
//     xacc::error("Invalid InstructionParameter: " + p.toString());
//   }

//   writer->String("lambda");
//   auto p3 = u.getParameter(2);

//   switch (p.which()) {
//   case 0:
//     writer->Int(p.as<int>());
//     break;
//   case 1:
//     writer->Double(p.as<double>());
//     break;
//   case 2:
//     writer->String(p.as<std::string>());
//     break;
//   default:
//     xacc::error("Invalid InstructionParameter: " + p.toString());
//   }
//   writer->EndObject();
}

template<class W, class B>
void JsonVisitor<W, B>::visit(IRGenerator& inst) {
      writer->StartObject();
  writer->String("gate");
  writer->String(inst.name().c_str());
  writer->String("enabled");
  writer->Bool(inst.isEnabled());
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

  writer->String("options");
  writer->StartObject();
  for (auto& kv : inst.getOptions()) {
      writer->Key(kv.first);
      auto p = kv.second;
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
  writer->EndObject();

  writer->EndObject();
}


template <class W, class B>
void JsonVisitor<W, B>::baseGateInst(GateInstruction &inst, bool endObject) {
  writer->StartObject();
  writer->String("gate");
  writer->String(inst.name().c_str());
  writer->String("enabled");
  writer->Bool(inst.isEnabled());
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

  writer->String("options");
  writer->StartObject();
  for (auto& kv : inst.getOptions()) {
      writer->Key(kv.first);
      auto p = kv.second;
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
  writer->EndObject();

  if (endObject) {
    writer->EndObject();
  }
}
template class JsonVisitor<Writer, StringBuffer>;

} // namespace quantum
} // namespace xacc
