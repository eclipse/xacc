/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
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
#ifndef XACC_XASMLISTENER_H
#define XACC_XASMLISTENER_H

#include "IR.hpp"
#include "IRProvider.hpp"
#include "xasmBaseListener.h"
#include "expression_parsing_util.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"

using namespace xasm;

namespace xacc {

struct XasmGreaterThan {};
struct XasmLessThan {};
struct XasmLessThanOrEqual {};
struct XasmGreaterThanOrEqual {};

class XASMListener : public xasmBaseListener {
protected:
  std::shared_ptr<IRProvider> irProvider;
  std::shared_ptr<CompositeInstruction> function;
  std::shared_ptr<CompositeInstruction> for_function;
  std::shared_ptr<CompositeInstruction> if_stmt;
  std::string bufferName = "";
  bool inForLoop = false;
  bool inIfStmt = false;

  std::vector<std::string> functionBufferNames;
  std::string currentInstructionName;
  std::vector<std::size_t> currentBits;
  std::vector<std::string> currentBufferNames;
  std::vector<InstructionParameter> currentParameters;
  std::map<int, std::string> currentBitIdxExpressions;
  std::pair<std::string, int> measure_cReg;
  std::map<std::string, int> new_var_to_vector_idx;

  std::string currentCompositeName;
  HeterogeneousMap currentOptions;

  std::shared_ptr<ExpressionParsingUtil> parsingUtil;

  void for_stmt_update_inst_args(Instruction* inst);
  
  std::vector<std::size_t> for_stmt_update_bits(Instruction *inst,
                                                const std::string varName,
                                                const int value);
  std::vector<InstructionParameter>
  for_stmt_update_params(Instruction *inst, const std::string varName,
                         const int value);

  template <typename T>
  void createForInstructions(xasmParser::ForstmtContext *ctx,
                             std::vector<InstPtr> &instructions,
                             std::shared_ptr<CompositeInstruction> function) {
    xacc::warning("[XasmCompiler] createForInstructions called with invalid "
                  "template parameter. Skipping.");
    return;
  }

public:
  HeterogeneousMap runtimeOptions;

  XASMListener();

  std::vector<std::string> getBufferNames() {return functionBufferNames;}
  
  std::shared_ptr<CompositeInstruction> getFunction() { return function; }

  void enterXacckernel(xasmParser::XacckernelContext * /*ctx*/) override;
  void enterXacclambda(xasmParser::XacclambdaContext * /*ctx*/) override;

  void enterInstruction(xasmParser::InstructionContext * /*ctx*/) override;
  void enterBufferList(xasmParser::BufferListContext * /*ctx*/) override;
  void enterParamList(xasmParser::ParamListContext * /*ctx*/) override;
  void exitInstruction(xasmParser::InstructionContext * /*ctx*/) override;

  void enterComposite_generator(
      xasmParser::Composite_generatorContext * /*ctx*/) override;
  void enterOptionsType(xasmParser::OptionsTypeContext * /*ctx*/) override;
  void exitComposite_generator(
      xasmParser::Composite_generatorContext * /*ctx*/) override;

  void enterForstmt(xasmParser::ForstmtContext * /*ctx*/) override;
  void exitForstmt(xasmParser::ForstmtContext * /*ctx*/) override;

  void enterIfstmt(xasmParser::IfstmtContext * /*ctx*/) override;
  void exitIfstmt(xasmParser::IfstmtContext * /*ctx*/) override;
};

} // namespace xacc

#endif
