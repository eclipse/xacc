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

#include "Program.hpp"
#include "XACC.hpp"

namespace xacc {

std::vector<IRTransformation> getAcceleratorIndependentTransformations(
		AcceleratorType& accType) {
	std::vector<IRTransformation> transformations;
	return transformations;
}

std::vector<std::shared_ptr<Preprocessor>> getDefaultPreprocessors(
		AcceleratorType accType) {
	std::vector<std::shared_ptr<Preprocessor>> preprocessors;
	auto preprocessor = ServiceRegistry::instance()->getService<Preprocessor>(
			"kernel-replacement");
	if (accType == AcceleratorType::qpu_gate) {
		preprocessors.push_back(preprocessor);
	}

	return preprocessors;
}

void Program::build() {

	// Get reference to the runtime options
	auto runtimeOptions = RuntimeOptions::instance();

	// Get the compiler that has been requested.
	auto compilerToRun = (*runtimeOptions)["compiler"];

	// Create the appropriate compiler
	compiler = xacc::ServiceRegistry::instance()->getService<Compiler>(
			compilerToRun);

	// Make sure we got a valid
	if (!compiler) {
		xacc::error("Invalid Compiler requested in Program.build().\n");
	}

	// Before compiling, run preprocessors
	auto defaultPPs = xacc::getDefaultPreprocessors(accelerator->getType());
	for (auto preprocessor : defaultPPs) {
		src = preprocessor->process(src, compiler, accelerator);
	}

	// Execute any preprocessor clients have provided
	for (auto preprocessor : preprocessors) {
		src = preprocessor->process(src, compiler, accelerator);
	}

	XACCLogger::instance()->info("Executing " + compiler->getName() + " compiler.");

	// Execute the compilation
	xaccIR = compiler->compile(src, accelerator);

	// Validate the compilation
	if (!xaccIR) {
		XACCLogger::instance()->error("Bad source string or something.\n");
	}

	// Execute hardware dependent IR Transformations
	auto accTransforms = accelerator->getIRTransformations();
	for (auto t : accTransforms) {
		xaccIR = t->transform(xaccIR);
	}

	for (auto irp : irpreprocessors) {
		bufferPostprocessors.push_back(irp->process(*xaccIR));
	}

	// Write the IR to file if the user requests it
	if (runtimeOptions->exists("persist-ir")) {
		auto fileStr = (*runtimeOptions)["persist-ir"];
		std::ofstream ostr(fileStr);
		xaccIR->persist(ostr);
	}

	return;
}

}
