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

// class ExtraInfo2HeterogeneousMap
//     {
// public:
//   ExtraInfo2HeterogeneousMap() {}

//   InstructionParameter operator()(const int &i) const {
//     return InstructionParameter(i);
//   }
//   InstructionParameter operator()(const double &i) const {
//     return InstructionParameter(i);
//   }
//   InstructionParameter operator()(const std::string &i) const {
//     return InstructionParameter(i);
//   }
//   InstructionParameter operator()(const std::vector<int> &i) const {
//     return InstructionParameter(i);
//   }
//   InstructionParameter operator()(const std::vector<double> &i) const {
//     return InstructionParameter(i);
//   }
//   InstructionParameter operator()(const std::vector<std::string> &i) const {
//     return InstructionParameter(i);
//   }
//   InstructionParameter
//   operator()(const std::map<int, std::vector<int>> &i) const {
//     XACCLogger::instance()->error(
//         "Cannot cast map<int, [int]> ExtraInfo to InstructionParameter");
//     return InstructionParameter(0);
//   }
//   InstructionParameter
//   operator()(const std::map<int, int> &i) const {
//       XACCLogger::instance()->error(
//           "Cannot cast map<int, int> ExtraInfo to InstructionParameter");
//       return InstructionParameter(0);
//   }
//   InstructionParameter
//   operator()(const std::vector<std::pair<double, double>> &i) const {
//     return InstructionParameter(i);
//   }
// };

// class InstructionParameter2ExtraInfo {
// private:
//   InstructionParameter parameter;

// public:
//   InstructionParameter2ExtraInfo() {}

//   ExtraInfo operator()(const int &i) const { return ExtraInfo(i); }
//   ExtraInfo operator()(const double &i) const { return ExtraInfo(i); }
//   ExtraInfo operator()(const std::string &i) const { return ExtraInfo(i); }
//   ExtraInfo operator()(const std::vector<int> &i) const { return ExtraInfo(i); }
//   ExtraInfo operator()(const std::vector<double> &i) const {
//     return ExtraInfo(i);
//   }
//   ExtraInfo operator()(const std::vector<std::string> &i) const {
//     return ExtraInfo(i);
//   }
//   ExtraInfo operator()(const std::vector<std::pair<double, double>> &i) const {
//     return ExtraInfo(i);
//   }
//   ExtraInfo operator()(const std::vector<std::pair<int, int>> &i) const {
//     XACCLogger::instance()->error(
//         "Cannot cast vector<int, int> InstructionParameter to ExtraInfo.");
//     return ExtraInfo(0);
//   }

//   ExtraInfo operator()(const std::complex<double> &i) const {
//     XACCLogger::instance()->error(
//         "Cannot cast complex InstructionParameter to ExtraInfo.");
//     return ExtraInfo(0);
//   }
// };

/**
 * The AcceleratorBuffer models an allocated buffer of
 * bits that are operated on by a kernel. As such,
 * the AcceleratorBuffer's primary role is to store
 * Accelerator execution results.
 *
 * @author Alex McCaskey
 */
class AcceleratorBuffer {

protected:
  /**
   * Reference to the Accelerator measurement result bit strings
   */
//   std::vector<std::string> measurements;

  std::map<std::string, int> bitStringToCounts;

  /**
   * The name of this AcceleratorBuffer
   */
  std::string bufferId;

  /**
   * The number of bits in this buffer.
   */
  int nBits;

  std::vector<AcceleratorBufferChildPair> children;

  std::map<std::string, ExtraInfo> info;

  bool cacheFile = false;

  std::map<int, int> bit2IndexMap;

public:
  AcceleratorBuffer() {}
  AcceleratorBuffer(const int N);

  /**
   * The Constructor
   */
  AcceleratorBuffer(const std::string &str, const int N);

  /**
   * The copy constructor
   */
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
  /**
   * Return all children with ExtraInfo infoName equal
   * to the given ExtraInfo i.
   */
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  getChildren(const std::string infoName, ExtraInfo i);

  // FIXME GET ALL UNIQUE ExtraInfo values at given ExtraInfo key...
  std::vector<ExtraInfo> getAllUnique(const std::string name);

  void removeChild(const std::size_t idx) {
      children.erase(children.begin()+idx);
  }

  /**
   * Return the number of bits in this buffer.
   *
   * @return size The number of bits in this buffer
   */
  const int size() const;

  /**
   * Return this AcceleratorBuffer's name
   *
   * @return name The name of this AcceleratorBuffer
   */
  const std::string name() const;

  /**
   * Reset the stored measured bit strings.
   */
  virtual void resetBuffer();

  virtual void appendMeasurement(const std::string &measurement);

  virtual void appendMeasurement(const std::string measurement,
                                 const int count);

  virtual double computeMeasurementProbability(const std::string &bitStr);

  /**
   * Compute and return the expectation value with respect
   * to the Pauli-Z operator. Here we provide a base implementation
   * based on an ensemble of measurement results. Subclasses
   * are free to implement this as they see fit, ie, for simulators
   * use the wavefunction.
   *
   * @return expVal The expectation value
   */
  virtual const double getExpectationValueZ();

  virtual void setExpectationValueZ(const double exp);

  /**
   * Return all measurements as bit strings.
   *
   * @return bitStrings List of bit strings.
   */
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

  /**
   * Print information about this AcceleratorBuffer to standard out.
   *
   */
  virtual void print();
  const std::string toString();

  void setName(const std::string n) {
      bufferId = n;
  }

  /**
   * Print information about this AcceleratorBuffer to the
   * given output stream.
   *
   * @param stream Stream to write the buffer to.
   */
  virtual void print(std::ostream &stream);

  virtual void load(std::istream &stream);

  void operator[](const int& i) {
      return;
  }

  /**
   * The Destructor
   */
  virtual ~AcceleratorBuffer() {}
};

} // namespace xacc

#endif
