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
 *   Sarah Powers - based off initial implementation in QCS written by Alex McCaskey
 *******************************************************************************/
#ifndef QPLACEMENT_MAPTOPHYSICAL_HPP_
#define QPLACEMENT_MAPTOPHYSICAL_HPP_

#include "xacc.hpp"
#include "IRTransformation.hpp"
#include "InstructionIterator.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

  // MapToPhysical is an IRTransformation that maps the logical program IR qubit indices to the physical qubits available on the given lattice
  class MapToPhysical : public xacc::IRTransformation {
  protected:
    std::vector<std::pair<int,int>> _edges;
  public:
    MapToPhysical() {}
    //MapToPhysical(std::vector<std::pair<int,int>> &edges) : _edges(edges) {}
    void apply(std::shared_ptr<CompositeInstruction> program,
               const std::shared_ptr<Accelerator> acc,
               const HeterogeneousMap &optioins = {}) override;
    const IRTransformationType type() const override {return IRTransformationType::Placement;}
    //std::shared_ptr<IR> transform(std::shared_ptr<IR> ir, std::shared_ptr<Accelerator> acc) override;
    const std::string name() const override { return "minor-graph-embedding-placement"; }
    const std::string description() const override { return ""; }
  };

} // namespace quantum
} // namespace xacc

#endif
