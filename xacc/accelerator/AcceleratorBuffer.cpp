/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 index accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "AcceleratorBuffer.hpp"
#include "xacc.hpp"

#include <numeric>

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

using namespace rapidjson;

namespace xacc {

bool CheckEqualVisitor::operator()(const int &i) const {
  return mpark::get<int>(extraInfo) == i;
}
bool CheckEqualVisitor::operator()(const double &i) const {
  return mpark::get<double>(extraInfo) == i;
}
bool CheckEqualVisitor::operator()(const std::string &i) const {
  auto asstr = mpark::get<std::string>(extraInfo);
  return asstr.compare(i) == 0;
}
bool CheckEqualVisitor::operator()(const std::vector<int> &i) const {
  return std::equal(i.begin(), i.end(),
                    mpark::get<std::vector<int>>(extraInfo).begin());
}
bool CheckEqualVisitor::operator()(const std::vector<double> &i) const {
  return std::equal(i.begin(), i.end(),
                    mpark::get<std::vector<double>>(extraInfo).begin(),
                    [](const double &d, const double &f) {
                      return std::fabs(d - f) < 1e-12;
                    });
}
bool CheckEqualVisitor::operator()(const std::vector<std::string> &i) const {
  return std::equal(i.begin(), i.end(),
                    mpark::get<std::vector<std::string>>(extraInfo).begin());
}

bool CheckEqualVisitor::operator()(
    const std::map<int, std::vector<int>> &i) const {
  return std::equal(
      i.begin(), i.end(),
      mpark::get<std::map<int, std::vector<int>>>(extraInfo).begin());
}

bool CheckEqualVisitor::operator()(const std::map<int, int> &i) const {
  return std::equal(i.begin(), i.end(),
                    mpark::get<std::map<int, int>>(extraInfo).begin());
}

bool CheckEqualVisitor::operator()(
    const std::map<std::string, double> &i) const {
  return std::equal(
      i.begin(), i.end(),
      mpark::get<std::map<std::string, double>>(extraInfo).begin());
}

bool CheckEqualVisitor::operator()(
    const std::vector<std::pair<double, double>> &i) const {
  return std::equal(
      i.begin(), i.end(),
      mpark::get<std::vector<std::pair<double, double>>>(extraInfo).begin(),
      [](const std::pair<double, double> &d,
         const std::pair<double, double> &f) {
        return std::fabs(d.first - f.first) < 1e-12 &&
               std::fabs(d.second - f.second) < 1e-12;
      });
}

template <class T> void ToJsonVisitor<T>::operator()(const int &i) {
  writer.Int(i);
}
template <class T> void ToJsonVisitor<T>::operator()(const double &i) {
  writer.Double(i);
}
template <class T> void ToJsonVisitor<T>::operator()(const std::string &i) {
  writer.String(i);
}
template <class T>
void ToJsonVisitor<T>::operator()(const std::vector<int> &i) {
  writer.StartArray();
  for (auto &v : i)
    writer.Int(v);
  writer.EndArray();
}
template <class T>
void ToJsonVisitor<T>::operator()(const std::vector<double> &i) {
  writer.StartArray();
  for (auto &v : i)
    writer.Double(v);
  writer.EndArray();
}
template <class T>
void ToJsonVisitor<T>::operator()(const std::vector<std::string> &i) {
  writer.StartArray();
  for (auto &v : i)
    writer.String(v);
  writer.EndArray();
}

template <class T>
void ToJsonVisitor<T>::operator()(const std::map<int, std::vector<int>> &i) {
  writer.StartObject();
  for (auto &kv : i) {
    writer.Key(std::to_string(kv.first));
    writer.StartArray();
    for (auto &v : kv.second) {
      writer.Int(v);
    }
    writer.EndArray();
  }
  writer.EndObject();
}

template <class T>
void ToJsonVisitor<T>::operator()(const std::map<int, int> &i) const {
  writer.StartObject();
  for (auto &kv : i) {
    writer.Key(std::to_string(kv.first));
    writer.Int(kv.second);
  }
  writer.EndObject();
}

template <class T>
void ToJsonVisitor<T>::operator()(
    const std::map<std::string, double> &i) const {
  writer.StartObject();
  for (auto &kv : i) {
    writer.Key(kv.first);
    writer.Double(kv.second);
  }
  writer.EndObject();
}

template <class T>
void ToJsonVisitor<T>::operator()(
    const std::vector<std::pair<double, double>> &i) const {
  writer.StartArray();
  for (auto &v : i) {
    writer.StartArray();
    writer.Double(v.first);
    writer.Double(v.second);
    writer.EndArray();
  }
  writer.EndArray();
}

template class ToJsonVisitor<PrettyWriter<StringBuffer>>;

AcceleratorBuffer::AcceleratorBuffer(const int N) : bufferId(""), nBits(N) {}

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
std::vector<std::shared_ptr<AcceleratorBuffer>>
AcceleratorBuffer::getChildren() {
  std::vector<std::shared_ptr<AcceleratorBuffer>> childrenReturn;
  for (auto &child : children) {
    childrenReturn.push_back(child.second);
  }

  return childrenReturn;
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
      if (i.index() == childExtraInfo.index()) {
        auto isEqual = mpark::visit(CheckEqualVisitor(i), childExtraInfo);
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
  } else if (allExtraInfoAtName[0].index() < 3) {
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
                      return mpark::visit(CheckEqualVisitor(i), j);
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
  //   measurements.clear();
  bitStringToCounts.clear();
  children.clear();
  info.clear();
  single_measurements.clear();
}

void AcceleratorBuffer::appendMeasurement(const std::string &measurement) {

  bitStringToCounts[measurement]++;
}

void AcceleratorBuffer::appendMeasurement(const std::string measurement,
                                          const int count) {
  bitStringToCounts[measurement] = count;

  return;
}

bool AcceleratorBuffer::operator[](const std::size_t &i) {
  if (!single_measurements.count(i)) {
    xacc::error("This bit (" + std::to_string(i) +
                ") has not been measured yet.");
  }
  return single_measurements[i];
}

bool AcceleratorBuffer::getCregValue(const std::string &cregName,
                                     const std::size_t &i) {
  if (cregName == bufferId) {
    return operator[](i);
  }
  auto iter = cReg_to_single_measurements.find(std::make_pair(cregName, i));
  if (iter != cReg_to_single_measurements.end()) {
    return operator[](iter->second);
  } else {
    std::cout << "This creg bit " << cregName << "[" << i
              << "] has not been assigned any value yet. Default = false.\n";
    return false;
  }
}

std::string
AcceleratorBuffer::single_measurements_to_bitstring(BitOrder bitOrder,
                                                    bool shouldClear) {
  std::string bitString;
  // single_measurements is an *ordered* map
  if (bitOrder == BitOrder::LSB) {
    for (auto iter = single_measurements.begin();
         iter != single_measurements.end(); ++iter) {
      bitString.append(iter->second ? "1" : "0");
    }
  } else {
    for (auto iter = single_measurements.rbegin();
         iter != single_measurements.rend(); ++iter) {
      bitString.append(iter->second ? "1" : "0");
    }
  }

  if (shouldClear) {
    single_measurements.clear();
  }
  return bitString;
}

double
AcceleratorBuffer::computeMeasurementProbability(const std::string &bitStr) {
  return (double)bitStringToCounts[bitStr] /
         std::accumulate(
             bitStringToCounts.begin(), bitStringToCounts.end(), 0,
             [](int value, const std::map<std::string, int>::value_type &p) {
               return value + p.second;
             });
}

std::shared_ptr<AcceleratorBuffer> AcceleratorBuffer::clone() {
  std::stringstream s;
  print(s);
  std::istringstream is(s.str());
  auto cloned = std::make_shared<AcceleratorBuffer>();
  cloned->load(is);
  return cloned;
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

  if (bitStringToCounts.empty() && this->hasExtraInfoKey("exp-val-z")) {
    aver = mpark::get<double>(getInformation("exp-val-z"));
  } else {
    for (auto &kv : bitStringToCounts) {
      int i = std::stoi(kv.first, nullptr, 2);
      auto par = has_even_parity(i);
      auto p = computeMeasurementProbability(kv.first);
      if (!par) {
        p = -p;
      }
      aver += p;
    }
  }
  return aver;
}

void AcceleratorBuffer::setExpectationValueZ(const double exp) {
  XACCLogger::instance()->error(
      "AcceleratorBuffer.setExpectationValueZ not "
      "implemented. This method is intended for subclasses.");
}

/**
 * Return all measurements as bit strings.
 *
 * @return bitStrings List of bit strings.
 */
const std::vector<std::string> AcceleratorBuffer::getMeasurements() {
  std::vector<std::string> strs;
  for (auto m : bitStringToCounts) {
    strs.push_back(m.first);
  }
  return strs;
}

std::map<std::string, int> AcceleratorBuffer::getMeasurementCounts() {
  return bitStringToCounts;
}

std::map<std::string, int>
AcceleratorBuffer::getMarginalCounts(const std::vector<int> &measIdxs,
                                     BitOrder bitOrder) {
  std::map<std::string, int> result;

  const auto bitMask = [&](const std::string &bitString) {
    std::string marginalBitString;
    for (const auto &bit : measIdxs) {
      if (bitOrder == BitOrder::MSB) {
        marginalBitString.push_back(bitString[bitString.size() - bit - 1]);
      } else {
        marginalBitString.push_back(bitString[bit]);
      }
    }
    return marginalBitString;
  };
  for (const auto &[bitString, count] : bitStringToCounts) {
    auto marginalBitStr = bitMask(bitString);
    if (result.find(marginalBitStr) != result.end()) {
      result[marginalBitStr] += count;
    } else {
      result[marginalBitStr] = count;
    }
  }

  return result;
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
  writer.StartObject(); // start root object
  if (!cacheFile) {
    writer.Key("AcceleratorBuffer");
    writer.StartObject();
    writer.Key("name");
    writer.String(name());
    writer.Key("size");
    writer.Int(size());
  }

  writer.Key((cacheFile ? "parameter_cache" : "Information"));
  writer.StartObject(); // start ab information
  for (auto &kv : info) {
    writer.Key(kv.first);
    ToJsonVisitor<PrettyWriter<StringBuffer>> vis(writer);
    mpark::visit(vis, kv.second);
    // kv.second.apply_visitor(vis);
  }
  // end ab information object
  writer.EndObject();

  if (!cacheFile) {
    writer.Key("Measurements");
    writer.StartObject();
    for (auto &kv : bitStringToCounts) {
      writer.Key(kv.first);
      writer.Int(kv.second);
    }
    writer.EndObject();
  }

  if (!children.empty()) {
    writer.Key("Children");
    writer.StartArray();
    for (auto &pair : children) {
      writer.StartObject(); // start child object
      writer.Key("name");
      writer.String(pair.first);
      writer.Key("Information");
      writer.StartObject(); // start information
      for (auto &kv : pair.second->getInformation()) {
        writer.Key(kv.first);
        ToJsonVisitor<PrettyWriter<StringBuffer>> vis(writer);
        mpark::visit(vis, kv.second);
        // kv.second.apply_visitor(vis);
      }
      // end information object
      writer.EndObject();
      writer.Key("Measurements");
      writer.StartObject();
      for (auto &kv : pair.second->getMeasurementCounts()) {
        writer.Key(kv.first);
        writer.Int(kv.second);
      }
      // end measurement object
      writer.EndObject();
      // End child object
      writer.EndObject();
    }

    writer.EndArray();
  }

  // end AB object
  if (!cacheFile)
    writer.EndObject();

  // end root object
  writer.EndObject();

  stream << buffer.GetString() << "\n";
}

const std::string AcceleratorBuffer::toString() {
  std::stringstream s;
  print(s);
  return s.str();
}

void AcceleratorBuffer::load(std::istream &stream) {
  std::string json(std::istreambuf_iterator<char>(stream), {});
  Document doc;
  doc.Parse(json);

  resetBuffer();

  if (!cacheFile) {
    bufferId = doc["AcceleratorBuffer"]["name"].GetString();
    nBits = doc["AcceleratorBuffer"]["size"].GetInt();
  }

  auto &topInfo = cacheFile ? doc["parameter_cache"]
                            : doc["AcceleratorBuffer"]["Information"];
  for (auto itr = topInfo.MemberBegin(); itr != topInfo.MemberEnd(); ++itr) {
    auto &value = topInfo[itr->name.GetString()];
    if (value.IsInt()) {
      addExtraInfo(itr->name.GetString(), value.GetInt());
    } else if (value.IsDouble()) {
      addExtraInfo(itr->name.GetString(), value.GetDouble());
    } else if (value.IsString()) {
      addExtraInfo(itr->name.GetString(), value.GetString());
    } else if (value.IsArray() && !value.GetArray().Empty()) {
      auto arr = value.GetArray();
      auto &firstVal = arr[0];
      if (firstVal.IsInt()) {
        std::vector<int> childValues;
        for (int i = 0; i < arr.Size(); i++)
          childValues.push_back(arr[i].GetInt());
        addExtraInfo(itr->name.GetString(), ExtraInfo(childValues));
      } else if (firstVal.IsDouble()) {
        std::vector<double> childValues;
        for (int i = 0; i < arr.Size(); i++)
          childValues.push_back(arr[i].GetDouble());
        addExtraInfo(itr->name.GetString(), ExtraInfo(childValues));
      } else if (firstVal.IsString()) {
        std::vector<std::string> childValues;
        for (int i = 0; i < arr.Size(); i++)
          childValues.push_back(arr[i].GetString());
        addExtraInfo(itr->name.GetString(), ExtraInfo(childValues));
      } else if (firstVal.IsArray() && !firstVal.GetArray().Empty() &&
                 firstVal.GetArray().Size() == 2) {
        std::vector<std::pair<double, double>> v;
        for (int i = 0; i < arr.Size(); i++)
          v.push_back({arr[i].GetArray()[0].GetDouble(),
                       arr[i].GetArray()[1].GetDouble()});

        addExtraInfo(itr->name.GetString(), ExtraInfo(v));
      }
    } else {
      // Here we have the case of an object([key:value])
      if (value.IsObject()) {
        std::map<int, std::vector<int>> map;
        std::map<int, int> map2;
        std::map<std::string, double> map3;
        for (auto itr2 = value.MemberBegin(); itr2 != value.MemberEnd();
             ++itr2) {
          auto keyIsInt = true;
          int key;
          try {
            key = std::stoi(itr2->name.GetString());
          } catch (std::exception &e) {
            keyIsInt = false;
          }
          if (itr2->value.IsArray() && keyIsInt) {
            // we ahve a map<int,[int*]>
            auto arr = itr2->value.GetArray();
            std::vector<int> vec;
            for (int i = 0; i < arr.Size(); i++)
              vec.push_back(arr[i].GetInt());
            map.insert({key, vec});
          } else if (itr2->value.IsInt() && keyIsInt) {
            auto val = itr2->value.GetInt();
            map2.insert({key, val});
          } else if (itr2->value.IsDouble()) {
            map3.insert({itr2->name.GetString(), itr2->value.GetDouble()});
          } else {
            break;
          }
        }
        if (!map.empty()) {
          addExtraInfo(itr->name.GetString(), ExtraInfo(map));
        }
        if (!map2.empty()) {
          addExtraInfo(itr->name.GetString(), ExtraInfo(map2));
        }
        if (!map3.empty()) {
          addExtraInfo(itr->name.GetString(), ExtraInfo(map3));
        }
      }
    }
  }

  // FIXME Handle Measurements
  if (!cacheFile) {
    auto &measures = doc["AcceleratorBuffer"]["Measurements"];
    for (auto itr = measures.MemberBegin(); itr != measures.MemberEnd();
         ++itr) {
      appendMeasurement(itr->name.GetString(), itr->value.GetInt());
    }
  }

  // if (!cacheFile) {
  // auto &bitMap = doc["AcceleratorBuffer"]["Bitmap"];
  // std::map<int, int> tmpMap;
  // for (auto itr = bitMap.MemberBegin(); itr != bitMap.MemberEnd(); ++itr) {
  //  tmpMap.insert({std::stoi(itr->name.GetString()), itr->value.GetInt()});
  //}
  // setBitIndexMap(tmpMap);
  // }

  if (!cacheFile && doc["AcceleratorBuffer"].HasMember("Children")) {
    auto children = doc["AcceleratorBuffer"]["Children"].GetArray();
    for (auto &c : children) {
      auto childBuffer =
          std::make_shared<AcceleratorBuffer>(c["name"].GetString(), nBits);
      auto &info = c["Information"];
      for (auto itr = info.MemberBegin(); itr != info.MemberEnd(); ++itr) {
        auto &value = info[itr->name.GetString()];
        std::stringstream sss;
        sss << itr->name.GetString();
        if (value.IsInt()) {
          childBuffer->addExtraInfo(itr->name.GetString(),
                                    ExtraInfo(value.GetInt()));
        } else if (value.IsDouble()) {
          childBuffer->addExtraInfo(itr->name.GetString(),
                                    ExtraInfo(value.GetDouble()));
        } else if (value.IsString()) {
          childBuffer->addExtraInfo(itr->name.GetString(),
                                    ExtraInfo(value.GetString()));
        } else if (value.IsArray() && !value.GetArray().Empty()) {
          auto arr = value.GetArray();
          auto &firstVal = arr[0];
          if (firstVal.IsInt()) {
            std::vector<int> childValues;
            for (int i = 0; i < arr.Size(); i++)
              childValues.push_back(arr[i].GetInt());
            childBuffer->addExtraInfo(itr->name.GetString(),
                                      ExtraInfo(childValues));
          } else if (firstVal.IsDouble()) {
            std::vector<double> childValues;
            for (int i = 0; i < arr.Size(); i++)
              childValues.push_back(arr[i].GetDouble());
            childBuffer->addExtraInfo(itr->name.GetString(),
                                      ExtraInfo(childValues));
          } else if (firstVal.IsString()) {
            std::vector<std::string> childValues;
            for (int i = 0; i < arr.Size(); i++)
              childValues.push_back(arr[i].GetString());
            childBuffer->addExtraInfo(itr->name.GetString(),
                                      ExtraInfo(childValues));
          } else {
            //   xacc::info("HELLO EXTRA: " +
            //  std::string(itr->name.GetString())); // << "\n";
          }
          // FIXME Handle Map<int, [int*]>
        }
      }

      auto &measures = c["Measurements"];
      for (auto itr = measures.MemberBegin(); itr != measures.MemberEnd();
           ++itr) {
        childBuffer->appendMeasurement(itr->name.GetString(),
                                       itr->value.GetInt());
      }

      appendChild(c["name"].GetString(), childBuffer);
    }
  }
}

} // namespace xacc
