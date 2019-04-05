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
#ifndef QUANTUM_AQC_COMPILER_TRIVIALEMBEDDINGALGORITHM_HPP_
#define QUANTUM_AQC_COMPILER_TRIVIALEMBEDDINGALGORITHM_HPP_

#include "EmbeddingAlgorithm.hpp"

namespace xacc {
namespace quantum {

/**
 * The EmbeddingAlgorithm class provides an interface
 * for minor graph embedding algorithms.
 *
 */
class TrivialEmbeddingAlgorithm : public EmbeddingAlgorithm {

public:
  /**
   * The Constructor
   */
  TrivialEmbeddingAlgorithm() {}

  /**
   * The Destructor
   */
  virtual ~TrivialEmbeddingAlgorithm() {}

  /**
   * Implementations of EmbeddingAlgorithm implement this method to
   * provide a valid minor graph embedding of the given problem
   * graph into the given hardware graph.
   *
   * @param problem The problem graph to be embedded into the hardware graph
   * @param hardware The hardware graph.
   * @param params Any key-value string parameters to influence the algorithm.
   * @return embedding A mapping of problem vertex indices to the list of
   * hardware vertices they map to
   */
  virtual Embedding embed(std::shared_ptr<Graph> problem,
                          std::shared_ptr<Graph> hardware,
                          std::map<std::string, std::string> params =
                              std::map<std::string, std::string>());

  virtual const std::string name() const { return "trivial"; }

  virtual const std::string description() const {
    return "This Embedding Algorithm trivially maps a problem vertex "
           "to a hardware vertex.";
  }
};

} // namespace quantum

} // namespace xacc

#endif
