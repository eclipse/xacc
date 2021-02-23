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
#ifndef QUANTUM_GATE_COMPILER_STAQ_VISITORS_HPP_
#define QUANTUM_GATE_COMPILER_STAQ_VISITORS_HPP_

#include "ast/decl.hpp"
#include "parser/parser.hpp"
#include "ast/traversal.hpp"
#include <map>
#include <iomanip>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "AllGateVisitor.hpp"

using namespace staq::ast;

namespace xacc {
namespace internal_staq {
static const std::map<std::string, std::string> staq_to_xacc{
    // "u3", "u2",   "u1", "ccx", cu1, cu3
    {"cx", "CX"}, {"id", "I"},    {"x", "X"},       {"y", "Y"},
    {"z", "Z"},   {"h", "H"},     {"s", "S"},       {"sdg", "Sdg"},
    {"t", "T"},   {"tdg", "Tdg"}, {"rx", "Rx"},     {"ry", "Ry"},
    {"rz", "Rz"}, {"cz", "CZ"},   {"cy", "CY"},     {"swap", "Swap"},
    {"ch", "CH"}, {"crz", "CRZ"}, {"cu1", "CPhase"}};

using IRConstructor = std::function<InstPtr(DeclaredGate&)>;
using StaqIRCtorMap = std::unordered_map<std::string, IRConstructor>;
static const StaqIRCtorMap staq_to_xacc_ir_ctor{
    {"x",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::X>(gate.qarg(0).offset().value());
     }},
    {"y",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Y>(gate.qarg(0).offset().value());
     }},
    {"z",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Z>(gate.qarg(0).offset().value());
     }},
    {"h",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Hadamard>(
           gate.qarg(0).offset().value());
     }},
    {"s",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::S>(gate.qarg(0).offset().value());
     }},
    {"sdg",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Sdg>(
           gate.qarg(0).offset().value());
     }},
    {"t",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::T>(gate.qarg(0).offset().value());
     }},
    {"tdg",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Tdg>(
           gate.qarg(0).offset().value());
     }},
    {"rx",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Rx>(
           gate.qarg(0).offset().value(), gate.carg(0).constant_eval().value());
     }},
    {"ry",
     [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Ry>(
           gate.qarg(0).offset().value(), gate.carg(0).constant_eval().value());
     }},
    {"rz", [](DeclaredGate &gate) {
       return std::make_shared<xacc::quantum::Rz>(
           gate.qarg(0).offset().value(), gate.carg(0).constant_eval().value());
     }}};

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
  void visit(ResetStmt &reset) override {
    ss << "Reset(" << reset.arg().var() << "["
       << reset.arg().offset().value() << "]);\n";
  }
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
    ss << "U(" << u.arg().var() << "[" << u.arg().offset().value() << "], "
       // This is used internally for source-source translation,
       // hence we don't want to lose any precision.
       << std::fixed << std::setprecision(16) << u.theta().constant_eval().value() << ", "
       << u.phi().constant_eval().value() << ", "
       << u.lambda().constant_eval().value() << ");\n";
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
      ss << std::fixed << std::setprecision(16) << ", " << g.carg(0).constant_eval().value();
      for (int i = 1; i < g.num_cargs(); i++) {
        ss << ", " << g.carg(i).constant_eval().value() << "\n";
      }
    }

    ss << ");\n";
  }
};

// Staq AST to XACC IR
class StaqToIr : public staq::ast::Visitor {
public:
  StaqToIr(const std::string &in_kernelName, const std::string &in_regName)
      : m_kernelName(in_kernelName), m_regName(in_regName), m_provider(xacc::getIRProvider("quantum")) {}
  void visit(VarAccess &) override {}
  // Expressions
  void visit(BExpr &) override {}
  void visit(UExpr &) override {}
  void visit(PiExpr &) override {}
  void visit(IntExpr &) override {}
  void visit(RealExpr &r) override {}
  void visit(VarExpr &v) override {}
  void visit(ResetStmt &reset) override {
    m_runtimeInsts.emplace_back(
        std::make_shared<xacc::quantum::Reset>(reset.arg().offset().value()));
  }
  void visit(IfStmt &) override {}
  void visit(BarrierGate &) override {}
  void visit(GateDecl &) override {}
  void visit(OracleDecl &) override {}
  void visit(RegisterDecl &) override {}
  void visit(AncillaDecl &) override {}
  void visit(Program &prog) override {
    // Program body
    m_runtimeInsts.clear();
    m_runtimeInsts.reserve(prog.body().size());
    prog.foreach_stmt([this](auto &stmt) { stmt.accept(*this); });
  }

  void visit(MeasureStmt &m) override {
    m_runtimeInsts.emplace_back(
        std::make_shared<xacc::quantum::Measure>(m.q_arg().offset().value()));
  }

  void visit(UGate &u) override {
    m_runtimeInsts.emplace_back(std::make_shared<xacc::quantum::U>(
        u.arg().offset().value(), u.theta().constant_eval().value(),
        u.phi().constant_eval().value(), u.lambda().constant_eval().value()));
  }

  void visit(CNOTGate &cx) override {
    m_runtimeInsts.emplace_back(std::make_shared<xacc::quantum::CNOT>(
        cx.ctrl().offset().value(), cx.tgt().offset().value()));
  }

  void visit(DeclaredGate &g) override {
    // Handle common gates:
    auto funcIter = staq_to_xacc_ir_ctor.find(g.name());
    if (funcIter != staq_to_xacc_ir_ctor.end()) {
      const auto &ctorFunc = funcIter->second;
      m_runtimeInsts.emplace_back(ctorFunc(g));
    } else {
      auto xacc_name = staq_to_xacc.at(g.name());
      // Otherwise, just do generic construction
      std::vector<std::size_t> gate_bits;
      std::vector<InstructionParameter> gate_params;
      for (int i = 0; i < g.num_qargs(); i++) {
        gate_bits.emplace_back(g.qarg(i).offset().value());
      }
      for (int i = 0; i < g.num_cargs(); i++) {
        gate_params.emplace_back(g.carg(i).constant_eval().value());
      }

      m_runtimeInsts.emplace_back(
          m_provider->createInstruction(xacc_name, gate_bits, gate_params));
    }
  }

  std::shared_ptr<IR> getIr() {
    auto composite =
        xacc::getService<IRProvider>("quantum")->createComposite(m_kernelName);
    composite->setBufferNames({m_regName});
    for (auto &gate: m_runtimeInsts) {
      std::vector regNames(gate->bits().size(), m_regName);
      gate->setBufferNames(regNames);
    }
    // Since the instructions were *compiled* by staq (valid AST),
    // hence, we skip all validation.
    composite->addInstructions(std::move(m_runtimeInsts), false);
    auto ir = xacc::getService<IRProvider>("quantum")->createIR();
    ir->addComposite(composite);
    return ir;
  }

private:
  std::vector<InstPtr> m_runtimeInsts;
  std::shared_ptr<IRProvider> m_provider;
  std::string m_kernelName;
  std::string m_regName;
};

using namespace xacc::quantum;

class XACCToStaqOpenQasm : public AllGateVisitor {

public:
  std::stringstream ss;
  std::map<std::string, std::string> cregNames;

  XACCToStaqOpenQasm(std::map<std::string, int> bufNamesToSize);
  void visit(Hadamard &h) override;
  void visit(CNOT &cnot) override;
  void visit(Rz &rz) override;
  void visit(Ry &ry) override;
  void visit(Rx &rx) override;
  void visit(X &x) override;
  void visit(Y &y) override;
  void visit(Z &z) override;
  void visit(CY &cy) override;
  void visit(CZ &cz) override;
  void visit(Swap &s) override;
  void visit(CRZ &crz) override;
  void visit(CH &ch) override;
  void visit(S &s) override;
  void visit(Sdg &sdg) override;
  void visit(T &t) override;
  void visit(Tdg &tdg) override;
  void visit(CPhase &cphase) override;
  void visit(Measure &measure) override;
  void visit(Identity &i) override;
  void visit(U &u) override;
  void visit(IfStmt &ifStmt) override;
};

} // namespace internal_staq
} // namespace xacc

#endif
