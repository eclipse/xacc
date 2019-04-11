/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include "DWQMICompiler.hpp"
#include "AcceleratorBuffer.hpp"
#include "RuntimeOptions.hpp"

#include "DWQMILexer.h"
#include "DWQMIListener.hpp"

using namespace antlr4;
using namespace dwqmi;

namespace xacc {

namespace quantum {

std::shared_ptr<IR> DWQMICompiler::compile(const std::string &src,
                                           std::shared_ptr<Accelerator> acc) {

  auto hardwareconnections = acc->getAcceleratorConnectivity();
        std::set<int> nUniqueBits;
        for (auto& edge : hardwareconnections) {
            nUniqueBits.insert(edge.first);
            nUniqueBits.insert(edge.second);
        }

        int nBits = *std::max_element(nUniqueBits.begin(), nUniqueBits.end()) + 1;

   auto hardwareGraph = xacc::getService<Graph>("boost-ugraph");
        for (int i = 0; i < nBits; i++) {
            std::map<std::string,InstructionParameter> m{{"bias",1.0}};
            hardwareGraph->addVertex(m);
        }
        for (auto& edge : hardwareconnections) {
            hardwareGraph->addEdge(edge.first, edge.second);
        }

  ANTLRInputStream input(src);
  DWQMILexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  DWQMIParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new DWQMIErrorListener());

  auto ir(std::make_shared<DWIR>());

  // Here we assume, there is just one allocation
  // of qubits for the D-Wave -- all of them.
  auto bufName = acc->getAllocatedBufferNames()[0];
  auto buffer = acc->getBuffer(bufName);
//   std::shared_ptr<AQCAcceleratorBuffer> aqcBuffer =
//       std::dynamic_pointer_cast<AQCAcceleratorBuffer>(buffer);
//   if (!aqcBuffer) {
//     xacc::error("Invalid AcceleratorBuffer passed to DW QMI Compiler. Must be "
//                 "an AQCAcceleratorBuffer.");
//   }

  tree::ParseTree *tree = parser.xaccsrc();
  DWQMIListener listener(ir, hardwareGraph, buffer);
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  return ir;
}

std::shared_ptr<IR> DWQMICompiler::compile(const std::string &src) {
  xacc::error("Cannot compile D-Wave program without "
              "information about Accelerator connectivity.");
  return std::make_shared<DWIR>();
}

const std::string DWQMICompiler::translate(const std::string &bufferVariable,
                                           std::shared_ptr<Function> function) {

  if (!std::dynamic_pointer_cast<DWFunction>(function)) {
    xacc::error("Cannot translate non-DW Kernels to DW-QMI.");
  }

  xacc::info("Translating provided IR Function to dwave-qmi.");

  std::string src = "__qpu__ " + function->name() + "(AcceleratorBuffer b";

  for (auto p : function->getParameters())
    src += ", double " + p.toString();

  src += ") {\n";

  for (int i = 0; i < function->nInstructions(); ++i) {
    auto inst = function->getInstruction(i);
    auto bits = inst->bits();
    auto param = inst->getParameter(0);
    std::stringstream s;
    s << "   " << bits[0] << " " << bits[1] << " "
      << param.toString() << ";\n";
    src += s.str();
  }
  src += "}";
  return src;
}

} // namespace quantum

} // namespace xacc
