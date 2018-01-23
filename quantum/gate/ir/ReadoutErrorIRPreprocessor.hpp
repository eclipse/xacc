/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#ifndef QUANTUM_GATE_IR_READOUTERRORIRPREPROCESSOR_HPP_
#define QUANTUM_GATE_IR_READOUTERRORIRPREPROCESSOR_HPP_

#include "IRPreprocessor.hpp"

namespace xacc {
namespace quantum {

class ReadoutErrorIRPreprocessor : public IRPreprocessor {
public:
	virtual std::shared_ptr<AcceleratorBufferPostprocessor> process(IR& ir);
};

}
}

#endif
