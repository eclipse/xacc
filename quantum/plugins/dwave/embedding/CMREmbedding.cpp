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
#include <cassert>

#include "CMREmbedding.hpp"
#include "find_embedding.hpp"

namespace xacc {
namespace cmr {

xacc::quantum::Embedding
CMREmbedding::embed(std::shared_ptr<xacc::Graph> problem,
                    std::shared_ptr<xacc::Graph> hardware,
                    std::map<std::string, std::string> params) {


class XACCInteractions : public find_embedding::LocalInteraction {
public:
  bool _canceled = false;
  void cancel() { _canceled = true; }

private:
  virtual void displayOutputImpl(const std::string &mess) const {
    xacc::info(mess);
  }
  virtual bool cancelledImpl() const { return _canceled; }
};

  // Local Declarations
  std::vector<int> pa, pb, ha, hb;
  std::vector<std::vector<int>> chains;
  int counter = 0;
  xacc::quantum::Embedding embedding;

  // Get the number of nodes on both graphs
  auto probN = problem->order();
  auto hardN = hardware->order();

  // map problem and hardware to input_graph structure
  for (int i = 0; i < probN; i++) {
    for (int j = 0; j < probN; j++) {
      if (i < j && problem->edgeExists(i, j)) {
        pa.push_back(i);
        pb.push_back(j);
      }
    }
  }

  for (int i = 0; i < hardN; i++) {
    for (int j = 0; j < hardN; j++) {
      if (i < j && hardware->edgeExists(i, j)) {
        ha.push_back(i);
        hb.push_back(j);
      }
    }
  }

  ::graph::input_graph prob(probN, pa, pb);
  ::graph::input_graph hard(hardN, ha, hb);
  find_embedding::optional_parameters _params;
  _params.localInteractionPtr =
      std::make_shared<XACCInteractions>(); //.reset(new XACCInteractions());

  if (find_embedding::findEmbedding(prob, hard, _params, chains)) {
    for (auto chain : chains) {
      embedding.insert(std::make_pair(counter, chain));
      counter++;
    }
  } else {
    xacc::error("Couldn't find embedding.");
  }

  return embedding;
}

} // namespace cmr
} // namespace xacc
