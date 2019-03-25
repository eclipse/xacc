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
 *******************************************************************************/
#ifndef XACC_IR_GRAPHPROVIDER_HPP_
#define XACC_IR_GRAPHPROVIDER_HPP_
#include "Graph.hpp"

namespace xacc {

/**
 *
 */
template <typename VertexType, GraphType type = Undirected>
class GraphProvider {

public:
  virtual Graph<VertexType, type> toGraph() = 0;
  // virtual void fromGraph(Graph<VertexType>& graph) = 0;
  // virtual void fromGraph(std::istream& input) = 0;
};

} // namespace xacc
#endif