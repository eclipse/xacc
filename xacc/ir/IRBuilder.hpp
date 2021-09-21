#pragma once

#include "IRProvider.hpp"

// Enumerate XACC Instruction Nodes
// If you want your new XACC Instruction to be supported by
// the IRBuilder, then you have to add it here, categorized by 1,2 qubits
#define XACC_SINGLE_QUBIT_NODE_LIST_WITH_CODE(code)                            \
  code(h) code(mz) code(rz) code(rx) code(ry) code(x) code(y) code(z) code(s)  \
      code(t) code(tdg) code(u) code(u1) code(rphi) code(reset)

#define XACC_TWO_QUBIT_NODE_LIST_WITH_CODE(code)                               \
  code(cnot) code(cphase) code(swap) code(iswap) code(fsim) code(cz) code(cy)  \
      code(crz) code(ch) code(xx) code(xy) code(rzz)

namespace xacc {
using QubitType = std::pair<std::string, std::size_t>;

class IRBuilder {
private:
  std::map<std::string, std::string> method_name_to_xacc_name;
  // Build instructions with the Provider
  std::shared_ptr<xacc::IRProvider> provider;

  // Composite storing the instructions
  std::shared_ptr<xacc::CompositeInstruction> program;

  // Build up method_name_to_xacc_name
  void __init__inst_map();

  // Temp holder for InstructionParameters
  // facilitates variadic params to vector conversion
  std::vector<InstructionParameter> m_tmp_holder;

  // Variadic to vector...
  void internal_push_back() {}
  void internal_push_back(InstructionParameter p) {
    m_tmp_holder.push_back(p);
    if (m_tmp_holder.back().isVariable()) {
      program->addVariable(m_tmp_holder.back().toString());
    }
  }
  template <typename First, typename... T>
  void internal_push_back(First f, T... t) {
    m_tmp_holder.push_back(f);
    if (m_tmp_holder.back().isVariable()) {
      program->addVariable(m_tmp_holder.back().toString());
    }
    internal_push_back(t...);
  }

public:
  IRBuilder();
  IRBuilder(const std::string name);
  auto to_ir() { return program; }

// Generate at compile time all one-qubit instruction method names
#define FUNC_SIG_GEN(TYPENAME) void TYPENAME
#define DEFINE_VISIT_ONE_QUBIT(type)                                           \
  template <typename... Params>                                                \
  FUNC_SIG_GEN(type)                                                           \
  (std::size_t qubit, Params... params) {                                      \
    internal_push_back(params...);                                             \
    program->addInstruction(provider->createInstruction(                       \
        method_name_to_xacc_name[#type], std::vector<std::size_t>{qubit},      \
        m_tmp_holder));                                                        \
    m_tmp_holder.clear();                                                      \
  }
  XACC_SINGLE_QUBIT_NODE_LIST_WITH_CODE(DEFINE_VISIT_ONE_QUBIT)
#undef DEFINE_VISIT_ONE_QUBIT

#define DEFINE_VISIT_ONE_QUBIT_TYPE(type)                                      \
  template <typename... Params>                                                \
  FUNC_SIG_GEN(type)                                                           \
  (QubitType qubit, Params... params) {                                        \
    internal_push_back(params...);                                             \
    program->addInstruction(provider->createInstruction(                       \
        method_name_to_xacc_name[#type], std::vector<QubitType>{qubit},        \
        m_tmp_holder));                                                        \
    m_tmp_holder.clear();                                                      \
  }
  XACC_SINGLE_QUBIT_NODE_LIST_WITH_CODE(DEFINE_VISIT_ONE_QUBIT_TYPE)
#undef DEFINE_VISIT_ONE_QUBIT_TYPE

#define DEFINE_VISIT_TWO_QUBIT(type)                                           \
  template <typename... Params>                                                \
  FUNC_SIG_GEN(type)                                                           \
  (std::size_t ctrl, std::size_t tgt, Params... params) {                      \
    internal_push_back(params...);                                             \
    program->addInstruction(provider->createInstruction(                       \
        method_name_to_xacc_name[#type], std::vector<std::size_t>{ctrl, tgt},  \
        m_tmp_holder));                                                        \
    m_tmp_holder.clear();                                                      \
  }
  XACC_TWO_QUBIT_NODE_LIST_WITH_CODE(DEFINE_VISIT_TWO_QUBIT)
#undef DEFINE_VISIT_TWO_QUBIT

#define DEFINE_VISIT_TWO_QUBIT_TYPE(type)                                      \
  template <typename... Params>                                                \
  FUNC_SIG_GEN(type)                                                           \
  (QubitType ctrl, QubitType tgt, Params... params) {                          \
    internal_push_back(params...);                                             \
    program->addInstruction(provider->createInstruction(                       \
        method_name_to_xacc_name[#type], std::vector<QubitType>{ctrl, tgt},    \
        m_tmp_holder));                                                        \
    m_tmp_holder.clear();                                                      \
  }
  XACC_TWO_QUBIT_NODE_LIST_WITH_CODE(DEFINE_VISIT_TWO_QUBIT_TYPE)
#undef DEFINE_VISIT_TWO_QUBIT_TYPE
};
} // namespace xacc
