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
#include "staq_swap_short.hpp"
#include "mapping/device.hpp"
#include "mapping/mapping/swap.hpp"
#include "mapping/layout/basic.hpp"
#include "xacc_service.hpp"
#include "xacc.hpp"
#include "staq_visitors.hpp"
#include "transformations/inline.hpp"

#include <regex>

// Need this to get around multiple definition error
// from upstream staq
namespace staq {
  namespace transformations {
    extern void desugar(staq::ast::ASTNode&);
  }
}

namespace xacc {
namespace quantum {

using namespace staq;
using namespace staq::ast;

void SwapShort::apply(std::shared_ptr<CompositeInstruction> program,
                      const std::shared_ptr<Accelerator> qpu,
                      const HeterogeneousMap &options) {

  if (!qpu) {
    xacc::warning("[SwapShort Placement] Provided QPU was null. Cannot run "
                  "shortest path swap placement.");
    return;
  }

  // First get total number of qubits on device
  std::set<int> qbitIdxs;
  auto connectivity = qpu->getConnectivity();
  for (auto &edge : connectivity) {
    qbitIdxs.insert(edge.first);
    qbitIdxs.insert(edge.second);
  }
  auto nQubits = *std::max_element(qbitIdxs.begin(), qbitIdxs.end()) + 1;

  // Create the adjacency matrix, init to all 0s first
  std::vector<std::vector<bool>> adj(nQubits);
  for (int i = 0; i < nQubits; i++) {
    adj[i] = std::vector<bool>(nQubits);
  }

  // Now set true where there are connections
  for (auto &edge : connectivity) {
    adj[edge.first][edge.second] = true;
    adj[edge.second][edge.first] = true;
  }

  // map to openqasm
  auto staq = xacc::getCompiler("staq");
  auto src = staq->translate(program);

    // std::cout << "HELLO WORLD:\n" << src << "\n";
  // parse that to get staq ast
  ast::ptr<ast::Program> prog;
  try {
    prog = parser::parse_string(src);
    transformations::desugar(*prog);
    transformations::Inliner::config c;
    // Make sure we treat map all control pauli 
    // ops and swaps as CNOT gates
    c.overrides.erase("cx");
    c.overrides.erase("cy");
    c.overrides.erase("cz");
    c.overrides.erase("swap");
    transformations::inline_ast(*prog, c);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << e.what();
    ss << "\nXACC Error in Staq Swap Short, here was the src:\n" << src << "\n";
    xacc::error(ss.str());
  }

  // std::cout << "HI:\n" << *prog << "\n";

  mapping::Device device(qpu->getSignature(), nQubits, adj);

  // map qreg_NAME -> q
  auto layout = mapping::compute_basic_layout(device, *prog);
  mapping::apply_layout(layout, device, *prog);

  mapping::map_onto_device(device, *prog);

  // map prog back to staq src string and
  // compile to ir
  std::stringstream ss;
  prog->pretty_print(ss);
  src = ss.str();

  // std::cout << "AGAIN\n" << src << "\n";
  auto ir = staq->compile(src);
  //   std::cout << "PASSED HERE\n";

  // reset the program and add optimized instructions
  program->clear();
  program->addInstructions(ir->getComposites()[0]->getInstructions());

  return;
}

} // namespace quantum
} // namespace xacc