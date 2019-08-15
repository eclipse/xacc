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
#ifndef QUANTUM_AQC_COMPILER_EMBEDDING_HPP_
#define QUANTUM_AQC_COMPILER_EMBEDDING_HPP_

#include <map>
#include <vector>
#include "Utils.hpp"

#include <iostream>
namespace xacc {

namespace quantum {

class Embedding : public std::map<int, std::vector<int>> {

public:
  void persist(std::ostream &stream) {
    for (auto &kv : *this) {
      stream << kv.first << ": ";
      for (int i = 0; i < kv.second.size(); i++) {
        if (i == kv.second.size() - 1) {
          stream << kv.second[i];
        } else {
          stream << kv.second[i] << " ";
        }
      }
      stream << "\n";
    }
  }

  void load(std::istream &stream) {
    std::string s(std::istreambuf_iterator<char>(stream), {});

    std::vector<std::string> splitNewLine, splitColon, splitSpaces;
    splitNewLine = xacc::split(s, '\n');
    for (auto line : splitNewLine) {
      if (!line.empty()) {
        splitColon = xacc::split(line,':');
        auto probVert = std::stoi(splitColon[0]);
        std::vector<int> hardwareVerts;
        splitSpaces = xacc::split(splitColon[1], ' ');
        for (auto i : splitSpaces) {
          if (!i.empty()) {
            hardwareVerts.push_back(std::stoi(i));
          }
        }
        insert(std::make_pair(probVert, hardwareVerts));
      }
    }

    return;
  }
};

} // namespace quantum

} // namespace xacc

#endif
