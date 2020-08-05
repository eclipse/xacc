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
#include "staq_rotation_folding.hpp"
#include "optimization/simplify.hpp"
#include "xacc_service.hpp"
#include "xacc.hpp"
#include "staq_visitors.hpp"
#include "optimization/rotation_folding.hpp"

namespace xacc {
namespace quantum {

using namespace staq;
using namespace staq::ast;

void RotationFolding::apply(std::shared_ptr<CompositeInstruction> program,
                            const std::shared_ptr<Accelerator> accelerator,
                            const HeterogeneousMap &options) {

  // map to openqasm
  auto staq = xacc::getCompiler("staq");
  auto src = staq->translate(program);

  // parse that to get staq ast
  auto prog = parser::parse_string(src);

  // fold rotations
  optimization::fold_rotations(*prog);
  optimization::simplify(*prog);

  // map prog back to staq src string and
  // compile to ir
  std::stringstream ss;
  // Staq has a *bug* whereby it cannot parse floating point parameters in
  // scientific notation form, yet happily 'pretty_print' parameters in that
  // form => cannot recompile itself.
  // Hence, we need to set the fixed format here to work around that.
  ss << std::fixed << std::setprecision(16);
  prog->pretty_print(ss);
  auto ir = staq->compile(ss.str());

  // reset the program and add optimized instructions
  program->clear();
  program->addInstructions(ir->getComposites()[0]->getInstructions());

  return;
}

} // namespace quantum
} // namespace xacc