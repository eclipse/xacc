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

#include <boost/dynamic_bitset.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include "Utils.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

using namespace rapidjson;

#include <boost/variant.hpp>

namespace xacc {

class AcceleratorBuffer;

using AcceleratorBufferChildPair =
    std::pair<std::string, std::shared_ptr<AcceleratorBuffer>>;
using ExtraInfo = boost::variant<int, double, std::string, std::vector<int>,
                                 std::vector<double>, std::vector<std::string>,
                                 std::map<int, std::vector<int>>>;
using AddPredicate = std::function<bool(ExtraInfo &)>;

class CheckEqualVisitor : public boost::static_visitor<bool> {
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
};

class ToJsonVisitor : public boost::static_visitor<> {
private:
  PrettyWriter<StringBuffer> &writer;

public:
  ToJsonVisitor(PrettyWriter<StringBuffer> &w) : writer(w) {}

  void operator()(const int &i);
  void operator()(const double &i);
  void operator()(const std::string &i);
  void operator()(const std::vector<int> &i);
  void operator()(const std::vector<double> &i);
  void operator()(const std::vector<std::string> &i);
  void operator()(const std::map<int, std::vector<int>> &i);
};

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
  std::vector<boost::dynamic_bitset<>> measurements;

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

public:
  AcceleratorBuffer() {}

  /**
   * The Constructor
   */
  AcceleratorBuffer(const std::string &str, const int N);

  /**
   * The copy constructor
   */
  AcceleratorBuffer(const AcceleratorBuffer &other);

  void appendChild(const std::string name,
                   std::shared_ptr<AcceleratorBuffer> buffer);
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  getChildren(const std::string name);
  std::vector<std::string> getChildrenNames();

  void addExtraInfo(const std::string infoName, ExtraInfo i);
  bool addExtraInfo(const std::string infoName, ExtraInfo i,
                    AddPredicate predicate);
  std::vector<std::string> listExtraInfoKeys();
  bool hasExtraInfoKey(const std::string infoName);
  ExtraInfo getInformation(const std::string name);
  std::map<std::string, ExtraInfo> getInformation();

  /**
   * Return all children with ExtraInfo infoName equal
   * to the given ExtraInfo i.
   */
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  getChildren(const std::string infoName, ExtraInfo i);

  // FIXME GET ALL UNIQUE ExtraInfo values at given ExtraInfo key...
  std::vector<ExtraInfo> getAllUnique(const std::string name);

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

  /**
   * Add a measurement result to this Buffer
   *
   * @param measurement The measurement result
   */
  virtual void appendMeasurement(const boost::dynamic_bitset<> &measurement);

  virtual void appendMeasurement(const boost::dynamic_bitset<> &measurement,
                                 const int count);
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
   * Return a read-only view of this Buffer's measurement results
   *
   * @return results Measurement results
   */
  virtual const std::vector<boost::dynamic_bitset<>> getMeasurements();

  /**
   * Return all measurements as bit strings.
   *
   * @return bitStrings List of bit strings.
   */
  virtual const std::vector<std::string> getMeasurementStrings();

  virtual std::map<std::string, int> getMeasurementCounts();

  /**
   * Print information about this AcceleratorBuffer to standard out.
   *
   */
  virtual void print();
  const std::string toString();

  /**
   * Print information about this AcceleratorBuffer to the
   * given output stream.
   *
   * @param stream Stream to write the buffer to.
   */
  virtual void print(std::ostream &stream);

  virtual void load(std::istream &stream);

  /**
   * The Destructor
   */
  virtual ~AcceleratorBuffer() {}
};

} // namespace xacc

#endif
