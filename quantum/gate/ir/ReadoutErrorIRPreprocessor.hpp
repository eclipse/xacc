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
#include "IRProvider.hpp"

namespace xacc {
namespace quantum {

class ReadoutErrorIRPreprocessor : public IRPreprocessor {
public:
	virtual std::shared_ptr<AcceleratorBufferPostprocessor> process(IR& ir);

	/**
	 * Return the name of this instance.
	 *
	 * @return name The string name
	 */
	virtual const std::string name() const {
		return "readout-error-preprocessor";
	}

	/**
	 * Return the description of this instance
	 * @return description The description of this object.
	 */
	virtual const std::string description() const {
		return "";
	}

	/**
	 * Return a Boost options_description instance that
	 * describes the options available for this
	 * derived subclass.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		auto desc = std::make_shared<options_description>();
		desc->add_options()("no-readout-error-postprocess","Do not apply ReadoutErrorAcceleratorBufferPostprocessor.");
		return desc;
	}

	/**
	 * Given user-input command line options, perform
	 * some operation. Returns true if runtime should exit,
	 * false otherwise.
	 *
	 * @param map The mapping of options to values
	 * @return exit True if exit, false otherwise
	 */
	virtual bool handleOptions(variables_map& map) {
		return false;
	}


};

}
}

#endif
