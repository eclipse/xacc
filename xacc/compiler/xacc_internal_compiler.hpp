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
#ifndef XACC_XACC_INTERNAL_COMPILER_HPP_
#define XACC_XACC_INTERNAL_COMPILER_HPP_

namespace xacc {
class CompositeInstruction;
class AcceleratorBuffer;
namespace internal_compiler {

template <typename T> struct empty_delete {
  empty_delete() {}
  void operator()(T *const) const {}
};

enum OptLevel { DEFAULT, LEVEL1, LEVEL2, LEVEL3 };

void compiler_InitializeXACC();

// Map kernel source string representing a single
// kernel function to a single CompositeInstruction (src to IR)
CompositeInstruction *compile(const char *compiler_name,
                                    const char *kernel_src,
                                    const char *qpu_name);

// Run quantum compilation routines on IR
void optimize(CompositeInstruction *program, const char *qpu_name,
              const OptLevel opt = DEFAULT);

// Execute on the specified QPU, persisting results to
// the provided buffer.
void execute(AcceleratorBuffer *buffer, const char *qpu_name,
             CompositeInstruction *program);

} // namespace internal_compiler
} // namespace xacc

#endif
