/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *   Daniel Claudino - MPI native implementation
 *******************************************************************************/
#ifndef XACC_HPC_VIRT_DECORATOR_HPP_
#define XACC_HPC_VIRT_DECORATOR_HPP_

#include "mpi.h"
#include "xacc.hpp"
#include "MPIProxy.hpp"
#include "AcceleratorDecorator.hpp"

namespace xacc {

namespace quantum {

class HPCVirtDecorator : public AcceleratorDecorator {
protected:

  int n_virtual_qpus = 1;
  // The MPI communicator for each QPU
  std::shared_ptr<ProcessGroup> qpuComm;

public:
  void initialize(const HeterogeneousMap &params = {}) override;

  void updateConfiguration(const HeterogeneousMap &config) override;

  const std::vector<std::string> configurationKeys() override { return {}; }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override;

  const std::string name() const override { return "hpc-virtualization"; }
  const std::string description() const override { return ""; }

  ~HPCVirtDecorator() override { }

private:
  template <typename T>
  std::vector<std::vector<T>> split_vector(const std::vector<T> &vec,
                                           size_t n) {
    std::vector<std::vector<T>> outVec;

    size_t length = vec.size() / n;
    size_t remain = vec.size() % n;

    size_t begin = 0;
    size_t end = 0;

    for (size_t i = 0; i < std::min(n, vec.size()); ++i) {
      end += (remain > 0) ? (length + !!(remain--)) : length;

      outVec.push_back(std::vector<T>(vec.begin() + begin, vec.begin() + end));

      begin = end;
    }

    return outVec;
  }
};

} // namespace quantum
} // namespace xacc
#endif