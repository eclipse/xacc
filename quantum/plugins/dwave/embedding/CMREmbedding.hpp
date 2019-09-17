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
#ifndef EMBEDDING_CMREMBEDDING_HPP_
#define EMBEDDING_CMREMBEDDING_HPP_

#include "xacc.hpp"
#include "EmbeddingAlgorithm.hpp"
#include <cassert>

namespace xacc {
namespace cmr {

class CMREmbedding : public xacc::quantum::EmbeddingAlgorithm {
public:
  virtual xacc::quantum::Embedding
  embed(std::shared_ptr<xacc::Graph> problem,
        std::shared_ptr<xacc::Graph> hardware,
        std::map<std::string, std::string> params =
            std::map<std::string, std::string>());

  virtual const std::string name() const { return "cmr"; }

  virtual const std::string description() const {
    return "An EmbeddingAlgorithm wrapping the D-Wave MinorMiner.";
  }
};

} // namespace cmr
} // namespace xacc

#endif
