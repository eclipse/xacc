#ifndef XACC_QUANTUM_GRAPH_HPP_
#define XACC_QUANTUM_GRAPH_HPP_

#include "Identifiable.hpp"
#include "InstructionParameter.hpp"
#include "Cloneable.hpp"

namespace xacc {

class Graph : public Identifiable, public Cloneable<Graph> {
public:

  virtual std::shared_ptr<Graph> clone() = 0;

  virtual void addEdge(const int srcIndex, const int tgtIndex,
               const double edgeWeight) = 0;
  virtual void addEdge(const int srcIndex, const int tgtIndex) = 0;
  virtual void removeEdge(const int srcIndex, const int tgtIndex) = 0;

  virtual void addVertex() = 0;
  virtual void addVertex(std::map<std::string, InstructionParameter>& properties) = 0;
  virtual void addVertex(std::map<std::string, InstructionParameter>&& properties) = 0;

  virtual void setVertexProperties(const int index, std::map<std::string, InstructionParameter>& properties) = 0;
  virtual void setVertexProperties(const int index, std::map<std::string, InstructionParameter>&& properties) = 0;
  virtual void setVertexProperty(const int index, const std::string prop, InstructionParameter& p) = 0;
  virtual void setVertexProperty(const int index, const std::string prop, InstructionParameter&& p) = 0;

  virtual std::map<std::string, InstructionParameter> getVertexProperties(const int index) = 0;
  virtual InstructionParameter&
  getVertexProperty(const int index, const std::string property) = 0;

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