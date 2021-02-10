/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef XACC_ACCELERATOR_HPP_
#define XACC_ACCELERATOR_HPP_

#include "AcceleratorBuffer.hpp"
#include "CompositeInstruction.hpp"
#include "OptionsProvider.hpp"
#include "Identifiable.hpp"
#include "Utils.hpp"
#include "Observable.hpp"
#include "heterogeneous.hpp"
#include <complex>

namespace xacc {

// Common typedefs and keys for ExecutionInfo:
// Accelerator sub-types should use these keys and types
// for ExecutionInformation HetMap for consistency.
namespace ExecutionInfo {
// Wavefunction
const std::string WaveFuncKey = "wave-function";
// Note: we use shared_ptr to prevent copies of these potentially large vectors.
using WaveFuncType = std::vector<std::complex<double>>;
using WaveFuncPtrType = std::shared_ptr<WaveFuncType>;

// Density matrix
const std::string DmKey = "density-matrix";
// Row-by-row matrix
using DensityMatrixType = std::vector<WaveFuncType>;
using DensityMatrixPtrType = std::shared_ptr<DensityMatrixType>;
} // namespace ExecutionInfo

// The Accelerator is the primary interface connecting programmers/clients
// with an available post-Moore's law co-processor (or accelerator, think GPU).
// Accelerators primarily expose execution functionality, which takes
// one or many CompositeInstructions and an AcceleratorBuffer, and affects
// execution on the specific corresponding backend and persists results
// to the provided buffer.
//
// Accelerators can be initialized with a HeterogeneousMap, enabling
// runtime configuration of the Accelerator backend (number of shots,
// specific backend name, etc.)
//
// Accelerators can expose hardware-specific IRTransformations, thereby
// ensuring that incoming CompositeInstructions are amenable for
// backend execution.
//
// Accelerators expose bit connectivity, and one and two bit instruction
// error rates.
class Accelerator : public Identifiable {

public:
  enum BitOrder {LSB, MSB};

  virtual void initialize(const HeterogeneousMap &params = {}) = 0;
  virtual void updateConfiguration(const HeterogeneousMap &config) = 0;
  virtual void updateConfiguration(const HeterogeneousMap &&config) {
    updateConfiguration(config);
  }
  virtual const std::vector<std::string> configurationKeys() = 0;

  virtual HeterogeneousMap getProperties() { return HeterogeneousMap(); }

  // Return this Accelerator signature, example might be
  // ibm:ibmq_20_tokyo (should always be ACCELERATOR:BACKEND)
  // For decorators this should be
  // DECORATOR,DECORATOR,...,DECORATOR,ibm:ibmq_20_tokyo
  virtual const std::string getSignature() { return name() + ":"; }

  virtual void
  contributeInstructions(const std::string &custom_json_config = "") {}

  // Return the name of an IRTransformation of type Placement that is
  // preferred for this Accelerator
  virtual const std::string defaultPlacementTransformation() {return "swap-shortest-path";}

  virtual BitOrder getBitOrder() {
      return BitOrder::MSB;
  }
  
  // Execute a single program. All results persisted to the buffer
  virtual void
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::shared_ptr<CompositeInstruction> CompositeInstruction) = 0;

  // Execute a vector of programs. A new buffer
  // is expected to be appended as a child of the provided buffer.
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::vector<std::shared_ptr<CompositeInstruction>>
                           CompositeInstructions) = 0;

  virtual void cancel(){};

  virtual std::vector<std::pair<int, int>> getConnectivity() {
    return std::vector<std::pair<int, int>>{};
  }

  virtual const std::vector<std::complex<double>>
  getAcceleratorState(std::shared_ptr<CompositeInstruction> program) {
    return std::vector<std::complex<double>>{};
  }

  virtual bool isRemote() { return false; }
  // Gate-by-gate application (with a persistent underlying quantum state)
  virtual void apply(std::shared_ptr<AcceleratorBuffer> buffer,
                     std::shared_ptr<Instruction> inst) {
    throw std::logic_error("Accelerator '" + name() +
                           "' doesn't support single gate application.");
  }

  // Custom execution-related information (specific to each Acc implementation)
  virtual HeterogeneousMap getExecutionInfo() const { return {}; }

  // Retrieve a particular execution-related information.
  template <typename T> T getExecutionInfo(const std::string &key) {
    if (!getExecutionInfo().keyExists<T>(key)) {
      XACCLogger::instance()->error(
          "getExecutionInfo() error - Invalid information key (" + key + ").");
    } else {
      return getExecutionInfo().get<T>(key);
    }
    return T();
  }

  virtual ~Accelerator() {}
};

template Accelerator* HeterogeneousMap::getPointerLike<Accelerator>(const std::string key) const;
} // namespace xacc
#endif
