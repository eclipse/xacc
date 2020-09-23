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
#ifndef XACC_QUANTUM_GRAPH_HPP_
#define XACC_QUANTUM_GRAPH_HPP_

#include "Identifiable.hpp"
#include "Instruction.hpp"
#include "Cloneable.hpp"

namespace xacc {

class Graph : public Identifiable, public Cloneable<Graph> {
public:
  virtual std::shared_ptr<Graph> clone() = 0;
  virtual std::shared_ptr<Graph> gen_random_graph(int nbNodes,
                                                  double edgeProb) const = 0;
  virtual void addEdge(const int srcIndex, const int tgtIndex,
                       const double edgeWeight) = 0;
  virtual void addEdge(const int srcIndex, const int tgtIndex) = 0;
  virtual void removeEdge(const int srcIndex, const int tgtIndex) = 0;

  virtual void addVertex() = 0;
  virtual void addVertex(HeterogeneousMap& properties) = 0;
  virtual void addVertex(HeterogeneousMap&& properties) = 0;

  virtual void setVertexProperties(const int index, HeterogeneousMap& properties) = 0;
  virtual void setVertexProperties(const int index, HeterogeneousMap&& properties) = 0;

  virtual HeterogeneousMap& getVertexProperties(const int index) = 0;

  virtual void setEdgeWeight(const int srcIndex, const int tgtIndex,
                     const double weight) = 0;
  virtual double getEdgeWeight(const int srcIndex, const int tgtIndex) = 0;
  virtual bool edgeExists(const int srcIndex, const int tgtIndex) = 0;

  virtual int degree(const int index) = 0;
  virtual int diameter()  = 0;
  // n edges
  virtual int size() = 0;
  // n vertices
  virtual int order() = 0;

  virtual const int depth() = 0;

  virtual std::vector<int> getNeighborList(const int index)  = 0;

  virtual void write(std::ostream &stream) = 0;
  virtual void read(std::istream &stream) = 0;

  virtual void computeShortestPath(int startIndex, std::vector<double> &distances,
                           std::vector<int> &paths) = 0;
};

}
#endif