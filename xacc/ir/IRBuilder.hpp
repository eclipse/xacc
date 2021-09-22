/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#pragma once

#include "IRProvider.hpp"

// Enumerate XACC Instruction Nodes
// -------- NOTE -------
// If you want your new XACC Instruction to be supported by
// the IRBuilder, then you have to add it here, categorized by 1,2 qubits
// ---------------------
#define XACC_SINGLE_QUBIT_NODE_LIST_WITH_CODE(code)                            \
  code(h) code(mz) code(rz) code(rx) code(ry) code(x) code(y) code(z) code(s)  \
      code(t) code(tdg) code(u) code(u1) code(rphi) code(reset)

#define XACC_TWO_QUBIT_NODE_LIST_WITH_CODE(code)                               \
  code(cnot) code(cphase) code(swap) code(iswap) code(fsim) code(cz) code(cy)  \
      code(crz) code(ch) code(xx) code(xy) code(rzz)

namespace xacc {

// Define QubitType to be our usual pair qreg_name, idx
using QubitType = std::pair<std::string, std::size_t>;

// The IRBuilder provides a high-level interface for the 
// construction of quantum gate model XACC IR. It exposes a 
// construction method for every gate instruction in XACC. Programmers 
// can use these methods to construct the IR CompositeInstruction instance 
// and call to_ir() to extract that CompositeInstruction.
class IRBuilder {
private:
  // Map lowercase gate method names to their corresponding XACC Instruction names
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

  // Variadic to vector helper functions...
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
  // The default constructor
  IRBuilder();
  // The constructor, takes the name of the CompositeInstruction
  IRBuilder(const std::string name);

  // Return the CompositeInstruction
  auto to_ir() { return program; }

// Generate at compile time all one-qubit instruction methods
// Takes qubit indices as input and potential parameters
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

// Same, takes QubitType instead of indices
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

// Generate at compile time all two-qubit instruction methods
// Takes qubit indices as input and potential parameters
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

// Same, takes QubitType instead of indices
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
