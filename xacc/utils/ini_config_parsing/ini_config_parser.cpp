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
#include "xacc_plugin.hpp"
#include "config_file_parser.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>

namespace {
template <typename T>
bool addPrimitiveFieldToMap(const std::string &in_fieldPath,
                            const boost::property_tree::ptree &in_ptree,
                            xacc::HeterogeneousMap &io_mapToAdd) {
  try {
    T val = in_ptree.get<T>(in_fieldPath);
    // std::cout << in_fieldPath << " --> " << val << "\n";
    io_mapToAdd.insert(in_fieldPath, val);
    return true;
  } catch (...) {
    return false;
  }
}

bool isContainerField(const std::string &in_fieldName,
                      const boost::property_tree::ptree &in_tree) {
  const std::string fieldAsString = in_tree.get<std::string>(in_fieldName);
  return !fieldAsString.empty() && fieldAsString.front() == '[' &&
         fieldAsString.back() == ']';
}

template <typename T> std::vector<T> to_array(const std::string &s) {
  std::vector<T> result;
  std::string arrayStr = s.substr(1, s.size() - 2);
  std::stringstream ss(arrayStr);
  std::string item;
  while (std::getline(ss, item, ',')) {
    result.emplace_back(boost::lexical_cast<T>(item));
  }
  return result;
}

template <>
std::vector<std::pair<int, int>>
to_array<std::pair<int, int>>(const std::string &s) {
  std::string arrayStr = s;
  arrayStr.erase(std::remove_if(arrayStr.begin(), arrayStr.end(), isspace),
                 arrayStr.end());
  std::vector<std::pair<int, int>> result;
  arrayStr = arrayStr.substr(1, s.size() - 2);
  std::stringstream ss(arrayStr);
  std::string pairItem;
  while (std::getline(ss, pairItem, ']')) {
    pairItem += ']';
    const std::vector<int> pairAsVec = to_array<int>(pairItem);
    if (pairAsVec.size() == 2) {
      result.emplace_back(std::make_pair(pairAsVec[0], pairAsVec[1]));
    }
    std::string temp;
    std::getline(ss, pairItem, ',');
  }
  return result;
}

template <typename T>
bool addArrayFieldToMap(const std::string &in_fieldPath,
                        const boost::property_tree::ptree &in_ptree,
                        xacc::HeterogeneousMap &io_mapToAdd) {
  try {
    auto array = to_array<T>(in_ptree.get<std::string>(in_fieldPath));
    io_mapToAdd.insert(in_fieldPath, array);
    return true;
  } catch (...) {
    return false;
  }
}
} // namespace

namespace xacc {
class IniFileParsingUtil : public ConfigFileParsingUtil {
public:
  HeterogeneousMap parse(const std::string &in_fileName) override {
    // TODO: check file exists
    boost::property_tree::ptree parsedTree;
    boost::property_tree::ini_parser::read_ini(in_fileName, parsedTree);
    HeterogeneousMap result;
    for (const auto &it : parsedTree) {
      // std::cout << it.first << ": " << it.second.data() << "\n";
      if (isContainerField(it.first, parsedTree)) {
        for (auto &converter : CONTAINER_TYPE_CONVERTERS) {
          if (converter(it.first, parsedTree, result)) {
            break;
          }
        }
      } else {
        for (auto &converter : PRIMITIVE_TYPE_CONVERTERS) {
          if (converter(it.first, parsedTree, result)) {
            break;
          }
        }
      }
    }

    return result;
  }
  const std::string name() const override { return "ini"; }
  const std::string description() const override {
    return "Parse config file in INI format";
  }

private:
  using AddFieldFnType = std::function<bool(const std::string &,
                                            const boost::property_tree::ptree &,
                                            HeterogeneousMap &)>;
  static inline const std::vector<AddFieldFnType> PRIMITIVE_TYPE_CONVERTERS{
      addPrimitiveFieldToMap<int>, addPrimitiveFieldToMap<double>,
      addPrimitiveFieldToMap<bool>, addPrimitiveFieldToMap<std::string>};

  static inline const std::vector<AddFieldFnType> CONTAINER_TYPE_CONVERTERS{
      addArrayFieldToMap<int>, addArrayFieldToMap<double>,
      addArrayFieldToMap<std::pair<int, int>>};
};
} // namespace xacc
REGISTER_PLUGIN(xacc::IniFileParsingUtil, xacc::ConfigFileParsingUtil)