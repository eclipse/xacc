/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_GATE_ALLGATEVISITOR_HPP_
#define QUANTUM_GATE_ALLGATEVISITOR_HPP_

#include "InstructionIterator.hpp"
#include "Hadamard.hpp"
#include "CNOT.hpp"
#include "X.hpp"
#include "Y.hpp"
#include "Z.hpp"
#include "ConditionalFunction.hpp"
#include "Rz.hpp"
#include "Rx.hpp"
#include "Ry.hpp"
#include "CPhase.hpp"
#include "Swap.hpp"
#include "Measure.hpp"
#include "Identity.hpp"
#include "CZ.hpp"

namespace xacc {
namespace quantum {

/**
 * FIXME write this
 */
class AllGateVisitor:
		public BaseInstructionVisitor,
		public InstructionVisitor<GateFunction>,
		public InstructionVisitor<Hadamard>,
		public InstructionVisitor<CNOT>,
		public InstructionVisitor<Rz>,
		public InstructionVisitor<Rx>,
		public InstructionVisitor<Ry>,
		public InstructionVisitor<ConditionalFunction>,
		public InstructionVisitor<X>,
		public InstructionVisitor<Y>,
		public InstructionVisitor<Z>,
		public InstructionVisitor<CPhase>,
		public InstructionVisitor<Swap>,
		public InstructionVisitor<Measure>,
		public InstructionVisitor<Identity>,
		public InstructionVisitor<CZ> {

};
}
}
#endif
