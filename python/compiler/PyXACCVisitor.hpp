#ifndef PYTHON_PYXACCVISITOR_HPP_
#define PYTHON_PYXACCVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include "IRGenerator.hpp"

namespace xacc {
namespace quantum {

/**
 */
class PyXACCVisitor : public AllGateVisitor, public InstructionVisitor<IRGenerator> {
protected:
  /**
   */
  std::string pyxaccStr;

public:
  PyXACCVisitor(std::vector<InstructionParameter> functionParams) {
    if (!functionParams.empty()) {
      pyxaccStr += "@qpu()\ndef foo(buffer," + functionParams[0].toString();
      for (int i = 1; i < functionParams.size(); i++) {
        pyxaccStr += "," + functionParams[i].toString();
      }
      pyxaccStr += "):\n";
    } else {
      pyxaccStr += "@qpu()\ndef foo(buffer):\n";
    }
  }
  PyXACCVisitor() { pyxaccStr += "@qpu()\ndef foo(buffer):\n"; }

  void visit(IRGenerator& irg) {
      pyxaccStr += "   " + irg.name() + "()\n";
  }
  /**
   * Visit hadamard gates
   */
  void visit(Hadamard &h) {
    pyxaccStr += "   H(" + std::to_string(h.bits()[0]) + ")\n";
  }

  void visit(Identity &i) {
    pyxaccStr += "   I(" + std::to_string(i.bits()[0]) + ")\n";
  }

  void visit(CZ &cz) {
    pyxaccStr += "   CZ(" + std::to_string(cz.bits()[0]) + ", " +
                 std::to_string(cz.bits()[1]) + ")\n";
  }

  /**
   * Visit CNOT gates
   */
  void visit(CNOT &cn) {
    pyxaccStr += "   CNOT(" + std::to_string(cn.bits()[0]) + ", " +
                 std::to_string(cn.bits()[1]) + ")\n";
  }
  /**
   * Visit X gates
   */
  void visit(X &x) {
    pyxaccStr += "   X(" + std::to_string(x.bits()[0]) + ")\n";
  }

  /**
   *
   */
  void visit(Y &y) {
    pyxaccStr += "   Y(" + std::to_string(y.bits()[0]) + ")\n";
  }

  /**
   * Visit Z gates
   */
  void visit(Z &z) {
    pyxaccStr += "   Z(" + std::to_string(z.bits()[0]) + ")\n";
  }

  /**
   * Visit Measurement gates
   */
  void visit(Measure &m) {
    int classicalBitIdx = m.getClassicalBitIndex();
    pyxaccStr += "   Measure(" + std::to_string(m.bits()[0]) + ", " +
                 std::to_string(classicalBitIdx) + ")\n";
  }

  /**
   * Visit Conditional functions
   */
  void visit(ConditionalFunction &c) {
    // nothing
  }

  void visit(Rx &rx) {
    auto angleStr = rx.getParameter(0).toString();
    pyxaccStr +=
        "   Rx(" + angleStr + ", " + std::to_string(rx.bits()[0]) + ")\n";
  }

  void visit(Ry &ry) {
    auto angleStr = ry.getParameter(0).toString();
    pyxaccStr +=
        "   Ry(" + angleStr + ", " + std::to_string(ry.bits()[0]) + ")\n";
  }

  void visit(Rz &rz) {
    auto angleStr = rz.getParameter(0).toString();
    pyxaccStr +=
        "   Rz(" + angleStr + ", " + std::to_string(rz.bits()[0]) + ")\n";
  }

  void visit(CPhase &cp) {
    auto angleStr = cp.getParameter(0).toString();
    pyxaccStr += "   CPhase(" + angleStr + ", " + std::to_string(cp.bits()[0]) +
                 ", " + std::to_string(cp.bits()[1]) + ")\n";
  }

  void visit(Swap &s) {
    pyxaccStr += "   SWAP(" + std::to_string(s.bits()[0]) + ", " +
                 std::to_string(s.bits()[1]) + ")\n";
  }

  void visit(U &u) {
    auto t = u.getParameter(0).toString();
    auto p = u.getParameter(1).toString();
    auto l = u.getParameter(2).toString();

    // if (u.getParameter(0).which() != 3 && u.getParameter(1).which() != 3 &&
    // u.getParameter(2).which() != 3) {
    //     // We have concrete values here
    //     auto td = boost::get<double>(u.getParameter(0));
    //     auto pd = boost::get<double>(u.getParameter(1));
    //     auto ld = boost::get<double>(u.getParameter(2));

    //     if (std::fabs())

    // }

    std::stringstream s;
    s << "   Rz(" << t << ", " << u.bits()[0] << ")\n";
    s << "   Ry(" << p << ", " << u.bits()[0] << ")\n";
    s << "   Rz(" << l << ", " << u.bits()[0] << ")\n";

    pyxaccStr += s.str();
  }

  void visit(GateFunction &f) { return; }
  /**
   * Return the quil string
   */
  std::string getPyXACCString() { return pyxaccStr; }

  /**
   * The destructor
   */
  virtual ~PyXACCVisitor() {}
};

} // namespace quantum
} // namespace xacc

#endif
