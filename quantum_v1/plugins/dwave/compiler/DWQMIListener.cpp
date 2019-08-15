/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial implementation - Alex McCaskey
 *
 **********************************************************************************/
#include "DWQMIListener.hpp"
#include "DWIR.hpp"
#include "DWFunction.hpp"
#include "DWQMI.hpp"
#include "Embedding.hpp"
#include "EmbeddingAlgorithm.hpp"
#include "exprtk.hpp"
#include <iostream>

#include "XACC.hpp"
#include "xacc_service.hpp"

using namespace dwqmi;

namespace xacc {
namespace quantum {
DWQMIListener::DWQMIListener(
    std::shared_ptr<xacc::IR> ir,
    std::shared_ptr<xacc::Graph> hardwareGraph,
    std::shared_ptr<AcceleratorBuffer> aqcBuffer)
    : ir(ir), hardwareGraph(hardwareGraph), buffer(aqcBuffer) {}

void DWQMIListener::enterXacckernel(
    dwqmi::DWQMIParser::XacckernelContext *ctx) {
  std::vector<InstructionParameter> params;
  maxBitIdx = 0;
  functionVarNames.clear();
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    functionVarNames.push_back(
        ctx->typedparam(static_cast<size_t>(i))->id()->getText());
    params.push_back(InstructionParameter(functionVarNames.back()));
  }
  curFunc = std::make_shared<DWFunction>(ctx->kernelname->getText(), params);
  functions.insert({curFunc->name(), curFunc});
}

void DWQMIListener::exitXacckernel(dwqmi::DWQMIParser::XacckernelContext *ctx) {
  // Now we have a qubits set. If we used 0-N qubits,
  // then this set size will be N, but if we skipped some
  // bits, then it will be < N. Get the maximum int in this set
  // so we can see if any were skipped.
  maxBitIdx++;

  // Create a graph representation of the problem
//   auto problemGraph = std::make_shared<DWGraph>(maxBitIdx);
 auto problemGraph = xacc::getService<Graph>("boost-ugraph");
        for (int i = 0; i < maxBitIdx;i++) {
            std::map<std::string,InstructionParameter> m{{"bias",1.0}};
            problemGraph->addVertex(m);
        }
  for (auto inst : curFunc->getInstructions()) {
    if (inst->name() == "dw-qmi") {
      auto qbit1 = inst->bits()[0];
      auto qbit2 = inst->bits()[1];
      if (qbit1 != qbit2) {
        problemGraph->addEdge(qbit1, qbit2, 1.0);
      }
    }
  }

  // Embed the problem
  Embedding embedding;
  std::string embAlgoStr = "cmr";
  if (xacc::optionExists("dwave-load-embedding")) {
    std::ifstream ifs(xacc::getOption("dwave-load-embedding"));
    embedding.load(ifs);
  } else {
    auto algoStr = xacc::optionExists("dwave-embedding")
                       ? xacc::getOption("dwave-embedding")
                       : embAlgoStr;
    auto embeddingAlgorithm = xacc::getService<EmbeddingAlgorithm>(algoStr);

    // Compute the minor graph embedding
    embedding = embeddingAlgorithm->embed(problemGraph, hardwareGraph);

    if (xacc::optionExists("dwave-persist-embedding")) {
      auto fileName = xacc::getOption("dwave-persist-embedding");
      std::ofstream ofs(fileName);
      embedding.persist(ofs);
    }
  }

  // Add the embedding to the AcceleratorBuffer
  buffer->addExtraInfo("embedding", ExtraInfo(embedding));

  ir->addKernel(curFunc);
};

void DWQMIListener::exitKernelcall(DWQMIParser::KernelcallContext *ctx) {
  std::string gateName = ctx->kernelname->getText();
  if (functions.count(gateName)) {
    curFunc->addInstruction(functions[gateName]);
  } else {
    xacc::error("Tried calling an undefined kernel.");
  }
}

void DWQMIListener::enterAnnealdecl(DWQMIParser::AnnealdeclContext *ctx) {
  if (!foundAnneal) {

    std::vector<InstructionParameter> params;
    auto taStr = ctx->ta()->getText();
    auto tpStr = ctx->tp()->getText();
    auto tqStr = ctx->tq()->getText();

    auto is_double = [](const std::string &s) -> bool {
      try {
        std::stod(s);
      } catch (std::exception &e) {
        return false;
      }
      return true;
    };

    auto taparam = is_double(taStr) ? InstructionParameter(std::stod(taStr))
                                    : InstructionParameter(taStr);
    auto tpparam = is_double(tpStr) ? InstructionParameter(std::stod(tpStr))
                                    : InstructionParameter(tpStr);
    auto tqparam = is_double(tqStr) ? InstructionParameter(std::stod(tqStr))
                                    : InstructionParameter(tqStr);

    if (taparam.which() == 3 &&
        !contains(functionVarNames, mpark::get<std::string>(taparam)))
      xacc::error(mpark::get<std::string>(taparam) +
                  " is an invalid kernel parameter (does not exist in kernel "
                  "arg list)");

    if (tpparam.which() == 3 &&
        !contains(functionVarNames, mpark::get<std::string>(tpparam)))
      xacc::error(mpark::get<std::string>(tpparam) +
                  " is an invalid kernel parameter (does not exist in kernel "
                  "arg list)");

    if (tqparam.which() == 3 &&
        !contains(functionVarNames, mpark::get<std::string>(tqparam)))
      xacc::error(mpark::get<std::string>(tqparam) +
                  " is an invalid kernel parameter (does not exist in kernel "
                  "arg list)");

    std::string direction = "forward";
    if (ctx->direction() != nullptr) {
      std::cout << ctx->direction()->getText() << "\n";
      if (ctx->direction()->forward() == nullptr) {
        direction = "reverse";
      }
    }

    auto anneal = std::make_shared<Anneal>(taparam, tpparam, tqparam,
                                           InstructionParameter(direction));
    curFunc->addInstruction(anneal);
    foundAnneal = true;
  } else {
    xacc::error("Error - You can only provide one anneal instruction.");
  }
  return;
}

void DWQMIListener::enterInst(dwqmi::DWQMIParser::InstContext *ctx) {

  int bit1, bit2;

  auto is_double = [](const std::string &s) -> bool {
    try {
      std::stod(s);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  };

  try {
    bit1 = std::stoi(ctx->INT(0)->getText());
    bit2 = std::stoi(ctx->INT(1)->getText());
  } catch (std::exception &e) {
    xacc::error("Invalid qubit indices: " + ctx->getText());
  }

  if (bit1 > maxBitIdx)
    maxBitIdx = bit1;
  if (bit2 > maxBitIdx)
    maxBitIdx = bit2;

  std::string valStr;
  if (ctx->real() != nullptr) {
    valStr = ctx->real()->getText();
  } else if (ctx->INT(2) != nullptr) {
    valStr = ctx->INT(2)->getText();
  } else {
    valStr = ctx->id()->getText();
  }
  auto param = is_double(valStr) ? InstructionParameter(std::stod(valStr))
                                 : InstructionParameter(valStr);

  if (param.which() == 3 &&
      !contains(functionVarNames, mpark::get<std::string>(param)))
    xacc::error(
        mpark::get<std::string>(param) +
        " is an invalid kernel parameter (does not exist in kernel arg list)");

  auto instruction =
      std::make_shared<DWQMI>(std::stoi(ctx->INT(0)->getText()),
                              std::stoi(ctx->INT(1)->getText()), param);

  curFunc->addInstruction(instruction);
  return;
}

} // namespace quantum
} // namespace xacc
