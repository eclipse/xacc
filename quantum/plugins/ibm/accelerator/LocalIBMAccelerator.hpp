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
#ifndef QUANTUM_GATE_ACCELERATORS_LOCALIBMACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_LOCALIBMACCELERATOR_HPP_

#include "Accelerator.hpp"
#include "InstructionIterator.hpp"
#include "OpenQasmVisitor.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

class LocalIBMAccelerator : public Accelerator {
public:
  void updateConfiguration(const HeterogeneousMap &config) override {
      if (config.keyExists<int>("shots")) {
          shots = config.get<int>("shots");
      }

      if (config.keyExists<double>("u-p-depol")) {
          udepol = config.get<double>("u-p-depol");
          u_depol_noise = true;
      }

      if (config.keyExists<double>("cx-p-depol")) {
          cxdepol = config.get<double>("cx-p-depol");
          cx_depol_noise = true;
      }
  }

  const std::vector<std::string> configurationKeys() override {
      return {"shots", "u-p-depol","cx-p-depol"};
  }

  void initialize(const HeterogeneousMap& params = {}) override {
      updateConfiguration(params);
  }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<xacc::CompositeInstruction> kernel) override;

  void
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<CompositeInstruction>> functions) override;

  std::vector<std::shared_ptr<IRTransformation>> getIRTransformations() override;

  OptionPairs getOptions() override {
    OptionPairs desc {{"local-ibm-ro-error",
                        "Provide the p(1|0), p(0|1)."},{
                        "u-p-depol", ""},{
                        "cx-p-depol", ""}};
    return desc;
  }

  const std::string name() const override { return "local-ibm"; }
  const std::string description() const override { return ""; }
  const std::string getSignature() override {return name()+":";}

  LocalIBMAccelerator() : Accelerator() {}
  virtual ~LocalIBMAccelerator() {}

protected:
  bool u_depol_noise = false;
  bool cx_depol_noise = false;
  bool readout_errors = false;
  int shots = 1024;
  double udepol = 0.0;
  double cxdepol = 0.0;
  std::map<int, std::vector<int>> measurementSupports;
};

} // namespace quantum
} // namespace xacc

#endif
