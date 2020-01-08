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

#include "ast/decl.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include "InstructionIterator.hpp"
#include "AcceleratorBuffer.hpp"

#include "parser/parser.hpp"
#include "ast/traversal.hpp"
#include "transformations/desugar.hpp"
#include "transformations/inline.hpp"
#include "transformations/oracle_synthesizer.hpp"
#include "optimization/simplify.hpp"

using namespace staq::ast;

namespace xacc {
namespace internal_staq {
static const std::map<std::string, std::string> staq_to_xacc{
    // "u3", "u2",   "u1", "ccx", cu1, cu3
    {"cx", "CX"}, {"id", "I"},    {"x", "X"},   {"y", "Y"},
    {"z", "Z"},   {"h", "H"},     {"s", "S"},   {"sdg", "Sdg"},
    {"t", "T"},   {"tdg", "Tdg"}, {"rx", "Rx"}, {"ry", "Ry"},
    {"rz", "Rz"}, {"cz", "CZ"},   {"cy", "CY"}, {"swap", "Swap"},
    {"ch", "CH"}, {"crz", "CRZ"}};

class CountQregs : public staq::ast::Traverse {
public:
  std::vector<std::string> qregs;
  void visit(staq::ast::RegisterDecl &d) override {
    if (d.is_quantum()) {
      qregs.push_back(d.id());
    }
  }
};

class CountAncillas : public staq::ast::Traverse {
public:
  std::map<std::string, int> ancillas;
  void visit(staq::ast::AncillaDecl &d) override {
    ancillas.insert({d.id(), d.size()});
  }
};

class StaqToXasm : public staq::ast::Visitor {
public:
  std::stringstream ss;
  void visit(VarAccess &) override {}
  // Expressions
  void visit(BExpr &) override {}
  void visit(UExpr &) override {}
  void visit(PiExpr &) override {}
  void visit(IntExpr &) override {}
  void visit(RealExpr &r) override {}
  void visit(VarExpr &v) override {}
  void visit(ResetStmt &) override {}
  void visit(IfStmt &) override {}
  void visit(BarrierGate &) override {}
  void visit(GateDecl &) override {}
  void visit(OracleDecl &) override {}
  void visit(RegisterDecl &) override {}
  void visit(AncillaDecl &) override {}
  void visit(Program &prog) override {
    // Program body
    prog.foreach_stmt([this](auto &stmt) { stmt.accept(*this); });
  }
  void visit(MeasureStmt &m) override {
    ss << "Measure(" << m.q_arg().var() << "[" << m.q_arg().offset().value()
       << "]);\n";
  }
  void visit(UGate &u) override {
    ss << "U(" << u.arg().var() << "[" << u.arg().offset().value() << "], " << 0
       << ", " << 0 << ", " << 0 << ");\n";
  }
  void visit(CNOTGate &cx) override {
    ss << "CX(" << cx.ctrl().var() << "[" << cx.ctrl().offset().value() << "],"
       << cx.tgt().var() << "[" << cx.tgt().offset().value() << "]);\n";
  }
  //   void visit(BarrierGate&) = 0;
  void visit(DeclaredGate &g) override {

    auto xacc_name = staq_to_xacc.at(g.name());
    ss << xacc_name << "(" << g.qarg(0).var() << "["
       << g.qarg(0).offset().value() << "]";
    for (int i = 1; i < g.num_qargs(); i++) {
      ss << ", " << g.qarg(i).var() << "[" << g.qarg(i).offset().value() << "]";
    }

    if (g.num_cargs() > 0) {
      ss << ", " << g.carg(0).constant_eval().value();
      for (int i = 1; i < g.num_cargs(); i++) {
        ss << ", " << g.carg(i).constant_eval().value() << "\n";
      }
    }

    ss << ");\n";
  }
};

} // namespace internal_staq

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
    for (auto &l : lines) {
      xacc::trim(l);
      tmp += l + "\n";
      if (l.find("include") != std::string::npos) {
        for (auto &b : bufferNames) {
          auto size = xacc::getBuffer(b)->size();
          tmp += "qreg " + b + "[" + std::to_string(size) + "];\n";
        }
      }
    }
    _src = tmp;
  }

//   std::cout << "SRC:\n" << _src << "\n";

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

//   std::cout << "HELLO:\n";
//   prog->pretty_print(std::cout);
//   exit(0);
  internal_staq::StaqToXasm translate;
  translate.visit(*prog);

//   std::cout << "XASM:\n" << translate.ss.str() << "\n";

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
        while(true) {
            name = "tmp"+std::to_string(counter);
            if (!xacc::hasCompiled(name)) {
                break;
            }
            counter++;
        }
    }
    xx << "__qpu__ void "<<name<<"(";
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
  return "";
}
} // namespace xacc
