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

#include <vector>
#include <memory>

namespace xacc {
class CompositeInstruction;
class AcceleratorBuffer;
class Accelerator;

namespace internal_compiler {
extern std::shared_ptr<Accelerator> qpu;
extern std::shared_ptr<CompositeInstruction> lastCompiled;
extern bool __execute;

enum OptLevel { DEFAULT, LEVEL1, LEVEL2, LEVEL3 };

void compiler_InitializeXACC(const char *qpu_backend = "qpp",
                             const std::vector<std::string> cmd_line_args = {});
void compiler_InitializeXACC(const char *qpu_backend, int shots);

void setAccelerator(const char *qpu_backend);
void setAccelerator(const char *qpu_backend, int shots);
std::shared_ptr<Accelerator> get_qpu();

void __set_verbose(bool verbose);

// Map kernel source string representing a single
// kernel function to a single CompositeInstruction (src to IR)
std::shared_ptr<CompositeInstruction> compile(const char *compiler_name,
                                              const char *kernel_src);

std::shared_ptr<CompositeInstruction> getLastCompiled();
std::shared_ptr<CompositeInstruction> getCompiled(const char *kernel_name);

// Run quantum compilation routines on IR
void optimize(std::shared_ptr<CompositeInstruction> program,
              const OptLevel opt = DEFAULT);

// Execute on the specified QPU, persisting results to
// the provided buffer.
void execute(AcceleratorBuffer *buffer,
             std::vector<std::shared_ptr<CompositeInstruction>> programs);
void execute(AcceleratorBuffer *buffer,
             std::shared_ptr<CompositeInstruction> program,
             double *parameters = nullptr);
void execute(AcceleratorBuffer **buffers, const int nBuffers,
             std::shared_ptr<CompositeInstruction> program,
             double *parameters = nullptr);
} // namespace internal_compiler
} // namespace xacc

#endif
