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
#ifndef QUANTUM_AQC_COMPILER_PARAMETERSETTER_HPP_
#define QUANTUM_AQC_COMPILER_PARAMETERSETTER_HPP_

#include "Graph.hpp"
#include "DWQMI.hpp"
#include "Embedding.hpp"
#include "Identifiable.hpp"

#include <list>

namespace xacc {
namespace quantum {

class ParameterSetter : public Identifiable {

public:
  virtual std::list<std::shared_ptr<DWQMI>>
  setParameters(std::shared_ptr<Graph> problemGraph,
                std::shared_ptr<Graph> hardwareGraph,
                Embedding embedding) = 0;

  virtual ~ParameterSetter() {}
};

} // namespace quantum
} // namespace xacc

#endif
