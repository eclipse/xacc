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
#ifndef XACC_QUANTUM_IR_PULSE_SCHEDULER_HPP_
#define XACC_QUANTUM_IR_PULSE_SCHEDULER_HPP_

#include "Scheduler.hpp"

namespace xacc {
namespace quantum {
class PulseScheduler : public Scheduler {
public:
  void schedule(std::shared_ptr<CompositeInstruction> program) override;
  const std::string name() const override {
		return "pulse";
	}

	const std::string description() const override{
		return "";
	}

};
}
}
#endif