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
#include "AcceleratorBuffer.hpp"
#include "XACC.hpp"

namespace xacc {

bool CheckEqualVisitor::operator()(const int &i) const {
  return boost::get<int>(extraInfo) == i;
}
bool CheckEqualVisitor::operator()(const double &i) const {
  return boost::get<double>(extraInfo) == i;
}
bool CheckEqualVisitor::operator()(const std::string &i) const {
  auto asstr = boost::get<std::string>(extraInfo);
  return asstr.compare(i) == 0;
}
bool CheckEqualVisitor::operator()(const std::vector<int> &i) const {
  return std::equal(i.begin(), i.end(),
                    boost::get<std::vector<int>>(extraInfo).begin());
}
bool CheckEqualVisitor::operator()(const std::vector<double> &i) const {
  return std::equal(i.begin(), i.end(),
                    boost::get<std::vector<double>>(extraInfo).begin(),
                    [](const double &d, const double &f) {
                      return std::fabs(d - f) < 1e-12;
                    });
}
bool CheckEqualVisitor::operator()(const std::vector<std::string> &i) const {
  return std::equal(i.begin(), i.end(),
                    boost::get<std::vector<std::string>>(extraInfo).begin());
}

bool CheckEqualVisitor::
operator()(const std::map<int, std::vector<int>> &i) const {
  return std::equal(
      i.begin(), i.end(),
      boost::get<std::map<int, std::vector<int>>>(extraInfo).begin());
}
void ToJsonVisitor::operator()(const int &i) { writer.Int(i); }
void ToJsonVisitor::operator()(const double &i) { writer.Double(i); }
void ToJsonVisitor::operator()(const std::string &i) { writer.String(i); }
void ToJsonVisitor::operator()(const std::vector<int> &i) {
  writer.StartArray();
  for (auto &v : i)
    writer.Int(v);
  writer.EndArray();
}
void ToJsonVisitor::operator()(const std::vector<double> &i) {
  writer.StartArray();
  for (auto &v : i)
    writer.Double(v);
  writer.EndArray();
}
void ToJsonVisitor::operator()(const std::vector<std::string> &i) {
  writer.StartArray();
  for (auto &v : i)
    writer.String(v);
  writer.EndArray();
}

void ToJsonVisitor::operator()(const std::map<int, std::vector<int>> &i) {
  writer.StartArray();
  for (auto &kv : i) {
    writer.Key(std::to_string(kv.first));
    writer.StartArray();
    for (auto &v : kv.second) {
      writer.Int(v);
    }
    writer.EndArray();
  }
  writer.EndArray();
}

AcceleratorBuffer::AcceleratorBuffer(const std::string &str, const int N)
    : bufferId(str), nBits(N) {}

AcceleratorBuffer::AcceleratorBuffer(const AcceleratorBuffer &other)
    : nBits(other.nBits), bufferId(other.bufferId) {}

bool AcceleratorBuffer::addExtraInfo(const std::string infoName, ExtraInfo i,
                                     AddPredicate predicate) {
  if (info.count(infoName) && predicate(info[infoName])) {
    if (predicate(info[infoName])) {
      info[infoName] = i;
      return true;
    }
    return false;
  } else {
    info.insert({infoName, i});
    return true;
  }
}

void AcceleratorBuffer::addExtraInfo(const std::string infoName, ExtraInfo i) {
  if (info.count(infoName)) {
    // overwrite
    info[infoName] = i;
  } else {
    info.insert({infoName, i});
  }
}

std::vector<std::string> AcceleratorBuffer::listExtraInfoKeys() {
  std::vector<std::string> sv;
  for (auto &kv : info) {
    sv.push_back(kv.first);
  }
  return sv;
}

ExtraInfo AcceleratorBuffer::getInformation(const std::string name) {
  if (!info.count(name)) {
    xacc::error("Invalid AcceleratorBuffer ExtraInfo key name - " + name + ".");
  }

  return info[name];
}

std::map<std::string, ExtraInfo> AcceleratorBuffer::getInformation() {
  return info;
}

void AcceleratorBuffer::appendChild(const std::string name,
                                    std::shared_ptr<AcceleratorBuffer> buffer) {
  children.push_back(std::make_pair(name, buffer));
}

std::vector<std::shared_ptr<AcceleratorBuffer>>
AcceleratorBuffer::getChildren(const std::string name) {
  std::vector<std::shared_ptr<AcceleratorBuffer>> childrenWithName;
  for (auto &child : children) {
    if (child.first == name) {
      childrenWithName.push_back(child.second);
    }
  }

  return childrenWithName;
}

std::vector<std::string> AcceleratorBuffer::getChildrenNames() {
  std::vector<std::string> names;
  for (auto &child : children)
    names.push_back(child.first);
  return names;
}

bool AcceleratorBuffer::hasExtraInfoKey(const std::string infoName) {
  return info.count(infoName);
}

std::vector<std::shared_ptr<AcceleratorBuffer>>
AcceleratorBuffer::getChildren(const std::string infoName, ExtraInfo i) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> childrenWithExtraInfo;

  for (auto &child : children) {
    if (child.second->hasExtraInfoKey(infoName)) {
      auto childExtraInfo = child.second->getInformation(infoName);
      if (i.which() == childExtraInfo.which()) {
        auto isEqual =
            boost::apply_visitor(CheckEqualVisitor(i), childExtraInfo);
        if (isEqual) {
          childrenWithExtraInfo.push_back(child.second);
        }
      }
    }
  }

  return childrenWithExtraInfo;
}

std::vector<ExtraInfo> AcceleratorBuffer::getAllUnique(const std::string name) {
  std::vector<ExtraInfo> allExtraInfoAtName;

  for (auto &child : children) {
    if (child.second->hasExtraInfoKey(name)) {
      auto childExtraInfo = child.second->getInformation(name);
      allExtraInfoAtName.push_back(childExtraInfo);
    }
  }

  if (allExtraInfoAtName.empty()) {
    return allExtraInfoAtName;
  } else if (allExtraInfoAtName[0].which() < 3) {
    // these are int, double, or strings, so its fairly simple
    std::sort(allExtraInfoAtName.begin(), allExtraInfoAtName.end());
    auto it = std::unique(allExtraInfoAtName.begin(), allExtraInfoAtName.end());
    allExtraInfoAtName.erase(it, allExtraInfoAtName.end());
    return allExtraInfoAtName;
  } else {
    // if they are the vectors, use a custom equality predicate
    auto last =
        std::unique(allExtraInfoAtName.begin(), allExtraInfoAtName.end(),
                    [](const ExtraInfo &i, const ExtraInfo &j) -> bool {
                      return boost::apply_visitor(CheckEqualVisitor(i), j);
                    });
    allExtraInfoAtName.erase(last, allExtraInfoAtName.end());
    return allExtraInfoAtName;
  }
}

/**
 * Return the number of bits in this buffer.
 *
 * @return size The number of bits in this buffer
 */
const int AcceleratorBuffer::size() const { return nBits; }

/**
 * Return this AcceleratorBuffer's name
 *
 * @return name The name of this AcceleratorBuffer
 */
const std::string AcceleratorBuffer::name() const { return bufferId; }

/**
 * Reset the stored measured bit strings.
 */
void AcceleratorBuffer::resetBuffer() {
  measurements.clear();
  bitStringToCounts.clear();
}

void AcceleratorBuffer::appendMeasurement(const std::string &measurement) {
  measurements.push_back(boost::dynamic_bitset<>(measurement));
  std::stringstream ss;
  ss << measurement;
  bitStringToCounts[ss.str()]++;
}

/**
 * Add a measurement result to this Buffer
 *
 * @param measurement The measurement result
 */
void AcceleratorBuffer::appendMeasurement(
    const boost::dynamic_bitset<> &measurement) {
  measurements.push_back(measurement);
  std::stringstream ss;
  ss << measurement;
  bitStringToCounts[ss.str()]++;
}

void AcceleratorBuffer::appendMeasurement(
    const boost::dynamic_bitset<> &measurement, const int count) {
  std::stringstream ss;
  ss << measurement;
  bitStringToCounts[ss.str()] = count;
  for (int i = 0; i < count; i++)
    measurements.push_back(measurement);
  return;
}

double
AcceleratorBuffer::computeMeasurementProbability(const std::string &bitStr) {
  return (double)bitStringToCounts[bitStr] / (double)measurements.size();
}

/**
 * Compute and return the expectation value with respect
 * to the Pauli-Z operator. Here we provide a base implementation
 * based on an ensemble of measurement results. Subclasses
 * are free to implement this as they see fit, ie, for simulators
 * use the wavefunction.
 *
 * @return expVal The expectation value
 */
const double AcceleratorBuffer::getExpectationValueZ() {
  double aver = 0.0;
  auto has_even_parity = [](unsigned int x) -> int {
    unsigned int count = 0, i, b = 1;
    for (i = 0; i < 32; i++) {
      if (x & (b << i)) {
        count++;
      }
    }
    if ((count % 2)) {
      return 0;
    }
    return 1;
  };

  for (auto &kv : bitStringToCounts) {
    int i = std::stoi(kv.first, nullptr, 2);
    auto par = has_even_parity(i);
    auto p = computeMeasurementProbability(kv.first);
    if (!par) {
      p = -p;
    }
    aver += p;
  }
  return aver;
}

void AcceleratorBuffer::setExpectationValueZ(const double exp) {
  XACCLogger::instance()->error(
      "AcceleratorBuffer.setExpectationValueZ not "
      "implemented. This method is intended for subclasses.");
}

/**
 * Return a read-only view of this Buffer's measurement results
 *
 * @return results Measurement results
 */
const std::vector<boost::dynamic_bitset<>>
AcceleratorBuffer::getMeasurements() {
  return measurements;
}

/**
 * Return all measurements as bit strings.
 *
 * @return bitStrings List of bit strings.
 */
const std::vector<std::string> AcceleratorBuffer::getMeasurementStrings() {
  std::vector<std::string> strs;
  for (auto m : measurements) {
    std::stringstream ss;
    ss << m;
    strs.push_back(ss.str());
  }
  return strs;
}

std::map<std::string, int> AcceleratorBuffer::getMeasurementCounts() {
  return bitStringToCounts;
}

/**
 * Print information about this AcceleratorBuffer to standard out.
 *
 */
void AcceleratorBuffer::print() { print(std::cout); }

/**
 * Print information about this AcceleratorBuffer to the
 * given output stream.
 *
 * @param stream Stream to write the buffer to.
 */
void AcceleratorBuffer::print(std::ostream &stream) {
  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  writer.StartObject();
  writer.Key("AcceleratorBuffer");
  writer.StartObject();
  writer.Key("name");
  writer.String(name());
  writer.Key("size");
  writer.Int(size());

  writer.Key("Information");
  writer.StartObject();
  for (auto &kv : info) {
    writer.Key(kv.first);
    ToJsonVisitor vis(writer);
    kv.second.apply_visitor(vis);
  }
  writer.EndObject();

  writer.Key("Measurements");
  writer.StartObject();
  for (auto &kv : bitStringToCounts) {
    writer.Key(kv.first);
    writer.Int(kv.second);
  }
  writer.EndObject();

  if (!children.empty()) {
    writer.Key("Children");
    writer.StartObject();
    for (auto &pair : children) {
      writer.Key("child");
      writer.StartObject();
      writer.Key("name");
      writer.String(pair.first);
      writer.Key("Information");
      writer.StartObject();
      for (auto &kv : pair.second->getInformation()) {
        writer.Key(kv.first);
        ToJsonVisitor vis(writer);
        kv.second.apply_visitor(vis);
      }
      writer.EndObject();
      writer.Key("Measurements");
      writer.StartObject();
      for (auto &kv : pair.second->getMeasurementCounts()) {
        writer.Key(kv.first);
        writer.Int(kv.second);
      }
      writer.EndObject();
      writer.EndObject();
    }
    writer.EndObject();
  }

  writer.EndObject();
  writer.EndObject();
  stream << buffer.GetString();
}

} // namespace xacc
