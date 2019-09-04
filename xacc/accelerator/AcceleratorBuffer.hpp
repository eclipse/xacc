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
#ifndef XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_
#define XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_

#include <string>
#include <sstream>
#include <iostream>
#include "Utils.hpp"
#include "heterogeneous.hpp"

namespace xacc {

class AcceleratorBuffer;

using AcceleratorBufferChildPair =
    std::pair<std::string, std::shared_ptr<AcceleratorBuffer>>;
using ExtraInfo = xacc::Variant<int, double, std::string, std::vector<int>,
                                 std::vector<double>, std::vector<std::string>,
                                 std::map<int, std::vector<int>>,
                                 std::vector<std::pair<double, double>>,
                                 std::map<int, int>>;

using AddPredicate = std::function<bool(ExtraInfo &)>;

class CheckEqualVisitor {
private:
  ExtraInfo extraInfo;

public:
  CheckEqualVisitor(ExtraInfo &toCheck) : extraInfo(toCheck) {}
  CheckEqualVisitor(const ExtraInfo &toCheck) : extraInfo(toCheck) {}

  bool operator()(const int &i) const;
  bool operator()(const double &i) const;
  bool operator()(const std::string &i) const;
  bool operator()(const std::vector<int> &i) const;
  bool operator()(const std::vector<double> &i) const;
  bool operator()(const std::vector<std::string> &i) const;
  bool operator()(const std::map<int, std::vector<int>> &i) const;
  bool operator()(const std::vector<std::pair<double, double>> &i) const;
  bool operator()(const std::map<int, int> &i) const;
};

template<typename Writer>
class ToJsonVisitor {
private:
  Writer &writer;

public:
  ToJsonVisitor(Writer &w) : writer(w) {}

  void operator()(const int &i);
  void operator()(const double &i);
  void operator()(const std::string &i);
  void operator()(const std::vector<int> &i);
  void operator()(const std::vector<double> &i);
  void operator()(const std::vector<std::string> &i);
  void operator()(const std::map<int, std::vector<int>> &i);
  void operator()(const std::vector<std::pair<double, double>> &i) const;
  void operator()(const std::map<int, int> &i) const;
};

class ExtraInfoValue2HeterogeneousMap {
protected:
  HeterogeneousMap& m;
  const std::string& key;
public:
  ExtraInfoValue2HeterogeneousMap(HeterogeneousMap& map, const std::string& k) : m(map),key(k) {}
  template<typename T>
  void operator()(const T& t) {
      m.insert(key, t);
  }
};

class HeterogenousMap2ExtraInfo : public xacc::visitor_base<int, double, std::string, std::vector<int>,
                                 std::vector<double>, std::vector<std::string>,
                                 std::map<int, std::vector<int>>,
                                 std::vector<std::pair<double, double>>,
                                 std::map<int, int>> {
protected:
  std::map<std::string, ExtraInfo>& data;
  public:
  HeterogenousMap2ExtraInfo(std::map<std::string, ExtraInfo>& m) : data(m) {}

  template<typename T>
  void operator()(const std::string& key, const T& t) {
      data.insert({key, ExtraInfo(t)});
  }
};

// The AcceleratorBuffer serves as the mediator between
// clients and backend execution. It represents a buffer of
// bits on the targeted Accelerator backend. At its core, it exposes
// the number of bits it wraps, and a map of bit strings to counts.
//
// Each AcceleratorBuffer also keeps track of a map of string keys
// to a Variant type called ExtraInfo. This map serves as a heterogeneous map
// with specified value types. It provides unique functionality in xacc, in that
// it enables the storage of accelerator-specific execution information.
//
// Each AcceleratorBuffer can contain AcceleratorBuffer children. This is useful
// for iterative executions whereby many executions need to be persisted, but are all
// related to a single global AcceleratorBuffer.
class AcceleratorBuffer {

protected:
  std::map<std::string, int> bitStringToCounts;
  std::string bufferId;
  int nBits;
  std::vector<AcceleratorBufferChildPair> children;
  std::map<std::string, ExtraInfo> info;
  bool cacheFile = false;
  std::map<int, int> bit2IndexMap;

public:
  AcceleratorBuffer() {}
  AcceleratorBuffer(const int N);
  AcceleratorBuffer(const std::string &str, const int N);
  AcceleratorBuffer(const AcceleratorBuffer &other);

  void useAsCache() { cacheFile = true; }
  void appendChild(const std::string name,
                   std::shared_ptr<AcceleratorBuffer> buffer);
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  getChildren(const std::string name);
  std::vector<std::shared_ptr<AcceleratorBuffer>> getChildren();
  std::vector<std::string> getChildrenNames();

  void addExtraInfo(const std::string infoName, ExtraInfo i);
  bool addExtraInfo(const std::string infoName, ExtraInfo i,
                    AddPredicate predicate);
  std::vector<std::string> listExtraInfoKeys();
  bool hasExtraInfoKey(const std::string infoName);
  ExtraInfo getInformation(const std::string name);
  std::map<std::string, ExtraInfo> getInformation();

  std::shared_ptr<AcceleratorBuffer> clone();

  std::map<int, int> getBitMap() { return bit2IndexMap; }

  void setBitIndexMap(const std::map<int, int> bitMap) { bit2IndexMap = bitMap; }

  const int nChildren() { return getChildren().size(); }

   // Return all children with ExtraInfo infoName equal
   // to the given ExtraInfo i.
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  getChildren(const std::string infoName, ExtraInfo i);

  // FIXME GET ALL UNIQUE ExtraInfo values at given ExtraInfo key...
  std::vector<ExtraInfo> getAllUnique(const std::string name);

  void removeChild(const std::size_t idx) {
      children.erase(children.begin()+idx);
  }

  const int size() const;
  const std::string name() const;
  virtual void resetBuffer();

  virtual void appendMeasurement(const std::string &measurement);
  virtual void appendMeasurement(const std::string measurement,
                                 const int count);

  virtual double computeMeasurementProbability(const std::string &bitStr);

  virtual const double getExpectationValueZ();
  virtual void setExpectationValueZ(const double exp);

  virtual const std::vector<std::string> getMeasurements();
  virtual std::map<std::string, int> getMeasurementCounts();
  virtual void clearMeasurements() {
    // measurements.clear();
    bitStringToCounts.clear();
  }
  virtual void setMeasurements(std::map<std::string, int> counts) {
    clearMeasurements();
    bitStringToCounts = counts;
  }

  virtual void print();
  const std::string toString();

  void setName(const std::string n) {
      bufferId = n;
  }

  virtual void print(std::ostream &stream);
  virtual void load(std::istream &stream);

  void operator[](const int& i) {
      // Primarily here for qcor.
      return;
  }

  virtual ~AcceleratorBuffer() {}
};

} // namespace xacc

#endif
