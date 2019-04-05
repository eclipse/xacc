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
#include "TrivialEmbeddingAlgorithm.hpp"

namespace xacc {
namespace quantum {

Embedding
TrivialEmbeddingAlgorithm::embed(std::shared_ptr<Graph> problem,
                                 std::shared_ptr<Graph> hdware,
                                 std::map<std::string, std::string> params) {

  Embedding xaccEmbedding;
  bool failHard = true;
  if (params.count("failhard")) {
    failHard = params["failhard"] == "false" ? false : true;
  }

  for (int i = 0; i < problem->order(); i++) {
    for (int j = 0; j < problem->order(); j++) {
      if (i < j && i != j &&
          (problem->edgeExists(i, j) && !hdware->edgeExists(i, j))) {
        if (failHard) {
          XACCLogger::instance()->error(
              "Trivial Embedding not possible, there is no hardware edge "
              "corresponding to (" +
              std::to_string(i) + ", " + std::to_string(j) + ") problem edge.");
        } else {
          XACCLogger::instance()->info(
              "This embedding failed, but user requested to not fail hard. "
              "Returning empty embedding.");
          xaccEmbedding.clear();
          return xaccEmbedding;
        }
      }
    }
    xaccEmbedding.insert(std::make_pair(i, std::vector<int>{i}));
  }

  return xaccEmbedding;
}

} // namespace quantum
} // namespace xacc
