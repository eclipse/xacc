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
#include <set>
#include "JsonVisitor.hpp"
#include "GateIR.hpp"
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
using namespace rapidjson;

namespace xacc {
namespace quantum {

std::string GateIR::toAssemblyString(const std::string &kernelName,
                                     const std::string &accBufferVarName) {
  std::string retStr = "";
  auto kernel = getKernel(kernelName);

  std::set<int> qubitsUsed;
  InstructionIterator it(kernel);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();

    // If enabled, invoke the accept
    // method which kicks off the visitor
    // to execute the appropriate lambda.
    if (nextInst->isEnabled() && !nextInst->isComposite()) {

      for (auto qi : nextInst->bits()) {
        qubitsUsed.insert(qi);
      }
    }
  }

  for (auto qi : qubitsUsed) {
    retStr += "qubit " + accBufferVarName + std::to_string(qi) + "\n";
  }
  for (auto f : kernels) {
    retStr += f->toString(accBufferVarName);
  }
  return retStr;
}

void GateIR::persist(std::ostream &outStream) {

  JsonVisitor<PrettyWriter<StringBuffer>,StringBuffer> visitor(kernels);
  outStream << visitor.write();

  return;
}

// FOR IR
void GateIR::load(std::istream &inStream) {
  // read(inStream);
}

} // namespace quantum
} // namespace xacc
