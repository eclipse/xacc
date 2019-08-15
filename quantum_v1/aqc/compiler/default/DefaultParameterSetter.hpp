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
#ifndef QUANTUM_AQC_COMPILER_DefaultParameterSetter_HPP_
#define QUANTUM_AQC_COMPILER_DefaultParameterSetter_HPP_

#include "ParameterSetter.hpp"
#include "OptionsProvider.hpp"

namespace xacc {
namespace quantum {

/**
 */
class DefaultParameterSetter : public ParameterSetter, public OptionsProvider {

public:
  /**
   * The Constructor
   */
  DefaultParameterSetter() {}

  /**
   * The Destructor
   */
  virtual ~DefaultParameterSetter() {}

  std::list<std::shared_ptr<DWQMI>>
  setParameters(std::shared_ptr<Graph> problemGraph,
                std::shared_ptr<Graph> hardwareGraph,
                Embedding embedding) override;

  const std::string name() const override { return "default"; }

  const std::string description() const override {
    return "This ParameterSetter sets Ising parameters as in the JADE "
           "publication.";
  }

  OptionPairs getOptions() override {
    OptionPairs desc {{"chain-strength",
                        ""}};
    return desc;
  }

};

} // namespace quantum

} // namespace xacc

#endif
