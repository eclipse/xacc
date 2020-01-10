/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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
#include "staq_compiler.hpp"

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "InstructionIterator.hpp"
#include "AcceleratorBuffer.hpp"

#include "staq_visitors.hpp"

#include "transformations/desugar.hpp"
#include "transformations/inline.hpp"
#include "transformations/oracle_synthesizer.hpp"
#include "optimization/simplify.hpp"

using namespace staq::ast;

namespace xacc {

StaqCompiler::StaqCompiler() {}

std::shared_ptr<IR> StaqCompiler::compile(const std::string &src,
                                          std::shared_ptr<Accelerator> acc) {
  // IF src contains typical xacc quantum kernel prototype, then take
  // buffers names and add qreg declarations before parsing
  // IF it does not, then assume qreg decls already declared.
  auto xasm = xacc::getCompiler("xasm");
  std::string prototype;

  std::string _src = src;
  bool isXaccKernel = false;
  if (src.find("__qpu__") != std::string::npos) {
    prototype = _src.substr(0, _src.find_first_of("{")) + "{}";
    auto bufferNames = xasm->getKernelBufferNames(prototype);

    isXaccKernel = true;

    std::string tmp = "";
    auto first = _src.find_first_of("{");
    auto last = _src.find_last_of("}");
    auto sub = _src.substr(first + 1, last - first - 1);
    auto lines = xacc::split(sub, '\n');
    bool addedNames = false;
    for (auto &l : lines) {
      xacc::trim(l);
      tmp += l + "\n";
      if (l.find("include") != std::string::npos) {
        for (auto &b : bufferNames) {
          auto size = xacc::getBuffer(b)->size();
          tmp += "qreg " + b + "[" + std::to_string(size) + "];\n";
        }
        addedNames = true;
      }
    }
    _src = tmp;
    if (!addedNames) {
      for (auto &b : bufferNames) {
        auto size = xacc::getBuffer(b)->size();
        _src = "qreg " + b + "[" + std::to_string(size) + "];\n" + _src;
      }
    }
  }

  // we allow users to leave out OPENQASM 2.0 and include qelib.inc
  // If they did we need to add it for them
  std::string tmp = "";
  auto lines = xacc::split(_src, '\n');
  bool foundOpenQasm = false, foundInclude = false;
  for (auto &l : lines) {
    if (l.find("OPENQASM") != std::string::npos) {
      foundOpenQasm = true;
    }
    if (l.find("include") != std::string::npos) {
      foundInclude = true;
    }
  }

  if (!foundInclude) {
    _src = "include \"qelib1.inc\";\n" + _src;
  }
  if (!foundOpenQasm) {
    _src = "OPENQASM 2.0;\n" + _src;
  }

//   std::cout << " HELLO:\n" << _src << "\n";
  using namespace staq;
  auto prog = parser::parse_string(_src);
  transformations::desugar(*prog);
  transformations::synthesize_oracles(*prog);

  optimization::simplify(*prog);

  // at this point we have to find out if we have any ancilla
  // registers
  internal_staq::CountAncillas ancillas;
  dynamic_cast<ast::Traverse &>(ancillas).visit(*prog);

  // add ancilla registers to xacc
  for (auto &kv : ancillas.ancillas) {
    if (!xacc::hasBuffer(kv.first)) {
      auto q = xacc::qalloc(kv.second);
      q->setName(kv.first);
      xacc::storeBuffer(q);
    } else {
      xacc::getBuffer(kv.first)->resetBuffer();
    }
  }

  transformations::inline_ast(
      *prog, {false, transformations::default_overrides, "anc"});

  // Visit Program to find out how many qreg there are and
  // use that to build up openqasm xacc function prototype

  internal_staq::StaqToXasm translate;
  translate.visit(*prog);

  std::string kernel;
  if (isXaccKernel) {
    if (!ancillas.ancillas.empty()) {
      kernel = prototype.substr(0, prototype.find_first_of(")"));
      for (auto &kv : ancillas.ancillas) {
        kernel += ", qreg " + kv.first;
      }
      kernel += ") {\n" + translate.ss.str() + "}";
    } else {
      kernel = prototype.substr(0, prototype.length() - 1) + "\n" +
               translate.ss.str() + "}";
    }
  } else {
    internal_staq::CountQregs c;
    dynamic_cast<ast::Traverse &>(c).visit(*prog);

    std::stringstream xx;
    std::string name = "tmp";
    if (xacc::hasCompiled(name)) {
      int counter = 0;
      while (true) {
        name = "tmp" + std::to_string(counter);
        if (!xacc::hasCompiled(name)) {
          break;
        }
        counter++;
      }
    }
    xx << "__qpu__ void " << name << "(";
    xx << "qreg " << c.qregs[0];
    for (int i = 1; i < c.qregs.size(); i++) {
      xx << ", qreg " << c.qregs[i];
    }
    for (auto &kv : ancillas.ancillas) {
      xx << ", qreg " << kv.first;
    }
    xx << ") {\n" << translate.ss.str() << "}";
    kernel = xx.str();
  }
  //   std::cout << "\n\nFinal:\n" << kernel << "\n";
  return xasm->compile(kernel, acc);
}

std::shared_ptr<IR> StaqCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string
StaqCompiler::translate(std::shared_ptr<xacc::CompositeInstruction> function) {
  std::map<std::string,int> bufNamesToSize;
  InstructionIterator iter(function);
  // First search buffer names and see if we have
  while(iter.hasNext()) {
    auto &next = *iter.next();
    if (next.isEnabled()) {
        for (int i = 0; i < next.nRequiredBits(); i++) {
            auto bufName = next.getBufferName(i);
            int size = next.bits()[i]+1;
            if (bufNamesToSize.count(bufName)) {
                if (bufNamesToSize[bufName] < size) {
                    bufNamesToSize[bufName] = size;
                }
            } else {
                bufNamesToSize.insert({bufName, size});
            }
        }
    }
  }

  auto translate = std::make_shared<internal_staq::XACCToStaqOpenQasm>(bufNamesToSize);
  InstructionIterator iter2(function);
  while(iter2.hasNext()) {
    auto &next = *iter2.next();
    if (next.isEnabled()) {
        next.accept(translate);
    }
  }

  return translate->ss.str();
}
} // namespace xacc
