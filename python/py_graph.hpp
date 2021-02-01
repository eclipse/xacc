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
#include <memory>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include "Graph.hpp"

namespace py = pybind11;
using namespace xacc;

class PyGraph : public xacc::Graph {
public:
  /* Inherit the constructors */
  using Graph::Graph;

  virtual const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Graph, name);
  }
  
  virtual const std::string description() const override { return ""; }

  virtual std::shared_ptr<Graph> clone() override {
    PYBIND11_OVERLOAD_PURE(std::shared_ptr<Graph>, xacc::Graph, clone);
  }

  virtual std::shared_ptr<Graph>
  gen_random_graph(int nbNodes, double edgeProb) const override {
    PYBIND11_OVERLOAD_PURE(std::shared_ptr<Graph>, xacc::Graph,
                           gen_random_graph, nbNodes, edgeProb);
  }

  virtual void addEdge(const int srcIndex, const int tgtIndex,
                       const double edgeWeight) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, addEdge, srcIndex, tgtIndex,
                           edgeWeight);
  }
  virtual void addEdge(const int srcIndex, const int tgtIndex) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, addEdge, srcIndex, tgtIndex);
  }
  virtual void removeEdge(const int srcIndex, const int tgtIndex) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, removeEdge, srcIndex, tgtIndex);
  }

  virtual void addVertex() override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, addVertex);
  }
  virtual void addVertex(HeterogeneousMap &properties) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, addVertex, properties);
  }
  virtual void addVertex(HeterogeneousMap &&properties) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, addVertex, properties);
  }

  virtual void setVertexProperties(const int index,
                                   HeterogeneousMap &properties) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, setVertexProperties, index,
                           properties);
  }
  virtual void setVertexProperties(const int index,
                                   HeterogeneousMap &&properties) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, setVertexProperties, index,
                           properties);
  }

  virtual HeterogeneousMap &getVertexProperties(const int index) override {
    PYBIND11_OVERLOAD_PURE(HeterogeneousMap &, xacc::Graph, getVertexProperties,
                           index);
  }

  virtual void setEdgeWeight(const int srcIndex, const int tgtIndex,
                             const double weight) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, setEdgeWeight, srcIndex, tgtIndex,
                           weight);
  }
  virtual double getEdgeWeight(const int srcIndex,
                               const int tgtIndex) override {
    PYBIND11_OVERLOAD_PURE(double, xacc::Graph, getEdgeWeight, srcIndex,
                           tgtIndex);
  }
  virtual bool edgeExists(const int srcIndex, const int tgtIndex) override {
    PYBIND11_OVERLOAD_PURE(bool, xacc::Graph, edgeExists, srcIndex, tgtIndex);
  }

  virtual int degree(const int index) override {
    PYBIND11_OVERLOAD_PURE(int, xacc::Graph, degree, index);
  }
  virtual int diameter() override {
    PYBIND11_OVERLOAD_PURE(int, xacc::Graph, diameter);
  }
  // n edges
  virtual int size() override {
    PYBIND11_OVERLOAD_PURE(int, xacc::Graph, size);
  }
  // n vertices
  virtual int order() override {
    PYBIND11_OVERLOAD_PURE(int, xacc::Graph, order);
  }

  virtual const int depth() override {
    PYBIND11_OVERLOAD_PURE(const int, xacc::Graph, depth);
  }

  virtual std::vector<int> getNeighborList(const int index) override {
    PYBIND11_OVERLOAD_PURE(std::vector<int>, xacc::Graph, getNeighborList,
                           index);
  }

  virtual void write(std::ostream &stream) override {}
  virtual void read(std::istream &stream) override {}
  virtual void computeShortestPath(int startIndex,
                                   std::vector<double> &distances,
                                   std::vector<int> &paths) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Graph, computeShortestPath, startIndex,
                           distances, paths);
  }
};

void bind_graph(py::module &m);
