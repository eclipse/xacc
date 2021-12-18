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
#include "ROErrorDecorator.hpp"
#include "InstructionIterator.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include <fstream>
#include <set>

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
using namespace rapidjson;

namespace xacc {
namespace quantum {
void ROErrorDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  if (decoratedAccelerator)
    decoratedAccelerator->execute(buffer, function);

  HeterogeneousMap properties;
  if (decoratedAccelerator) {
    properties = decoratedAccelerator->getProperties();
  }

  std::map<int, double> piplus, piminus;

  if (xacc::fileExists(roErrorFile)) {
    // First: if a readout error file is provided, use it (since user explicitly provided it)
    // Get RO error probs
    buffer->addExtraInfo("ro-error-file", ExtraInfo(roErrorFile));

    std::ifstream t(roErrorFile);
    std::string json((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    if (json.empty()) {
      xacc::error("Invalid ROError JSON file: " + roErrorFile);
    }

    Document d;
    d.Parse(json);

    for (auto itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
      std::string key = itr->name.GetString();
      if (key.compare("shots") != 0 && key.compare("backend") != 0) {
        auto &value = d[itr->name.GetString()];
        auto qbit = std::stoi(key);
        piplus.insert({qbit, value["+"].GetDouble()});
        piminus.insert({qbit, value["-"].GetDouble()});
      }
    }
  } else if (properties.keyExists<std::vector<double>>("p01s") &&
             properties.keyExists<std::vector<double>>("p10s")) {
    // If no readout error file is provided, use the backend's p01s and p10s properties           
    auto p01s = properties.get<std::vector<double>>("p01s");
    auto p10s = properties.get<std::vector<double>>("p10s");
    for (int i = 0; i < p01s.size(); i++) {
      piplus.insert({i, p01s[i] + p10s[i]});
      piminus.insert({i, p01s[i] - p10s[i]});
    }
  } else {
    xacc::warning("Accelerator does not have readout error properties and Cannot "
               "find readout erro file (key 'file'). Skipping ReadoutError "
               "correction.");
    return;
  }

  auto supports = [](std::shared_ptr<CompositeInstruction> f) {
    std::set<int> supportSet;
    std::map<int, int> support_to_creg_map;
    int creg_count = 0;
    InstructionIterator it(f);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->name() == "Measure") {
        auto bits = nextInst->bits();
        supportSet.insert(bits[0]);
        support_to_creg_map.insert({bits[0], creg_count});
        creg_count++;
        for (auto &b : bits) {
          supportSet.insert(b);
        }
      }
    }
    return std::make_pair(supportSet, support_to_creg_map);
  };

  // Get the number of shots first
  int nShots = 0;
  auto tmpCounts = buffer->getMeasurementCounts();
  for (auto &kv : tmpCounts) {
    nShots += kv.second;
  }

  auto counts = buffer->getMeasurementCounts();

  auto fixedExp = 0.0;
  for (auto &kv : counts) {
    auto prod = 1.0;
    std::string bitString = kv.first;
    auto count = kv.second;
    auto [_supports, supports_to_creg_map] = supports(function);
    for (auto j : _supports) {
      prod *=
          (std::pow(
               -1,
               (int)(bitString[buffer->size() - supports_to_creg_map[j] - 1] -
                     '0')) -
           piminus[j]) /
          (1.0 - piplus[j]);
    }

    fixedExp += ((double)count / (double)nShots) * prod;
  }
  if (fixedExp > 1.0) {
    fixedExp = 1.0;
  }
  if (fixedExp < -1.0) {
    fixedExp = -1.0;
  }
  buffer->addExtraInfo("ro-fixed-exp-val-z", ExtraInfo(fixedExp));

  return;
}

void ROErrorDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;
  std::map<std::string, std::shared_ptr<CompositeInstruction>> nameToFunction;
  std::map<std::string, std::pair<std::set<int>, std::map<int, int>>>
      supportSets;

  if (decoratedAccelerator) {
    decoratedAccelerator->execute(buffer, functions);
    buffers = buffer->getChildren();
  } else {
    buffers = buffer->getChildren();
  }

  auto supports = [](std::shared_ptr<CompositeInstruction> f) {
    std::set<int> supportSet;
    std::map<int, int> support_to_creg_map;
    int creg_count = 0;
    InstructionIterator it(f);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->name() == "Measure") {
        auto bits = nextInst->bits();
        supportSet.insert(bits[0]);
        support_to_creg_map.insert({bits[0], creg_count});
        creg_count++;
        for (auto &b : bits) {
          supportSet.insert(b);
        }
      }
    }
    return std::make_pair(supportSet, support_to_creg_map);
  };

  for (auto &f : functions) {
    nameToFunction.insert({f->name(), f});
    supportSets.insert({f->name(), supports(f)});
  }

  std::map<int, double> piplus, piminus;
  HeterogeneousMap properties;
  if (decoratedAccelerator) {
    properties = decoratedAccelerator->getProperties();
  }

  if (xacc::fileExists(roErrorFile)) {
    // Get RO error probs
    buffer->addExtraInfo("ro-error-file", ExtraInfo(roErrorFile));

    std::ifstream t(roErrorFile);
    std::string json((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    if (json.empty()) {
      xacc::error("Invalid ROError JSON file: " + roErrorFile);
    }

    Document d;
    d.Parse(json);

    for (auto itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
      std::string key = itr->name.GetString();
      if (key.compare("shots") != 0 && key.compare("backend") != 0) {
        auto &value = d[itr->name.GetString()];
        auto qbit = std::stoi(key);
        piplus.insert({qbit, value["+"].GetDouble()});
        piminus.insert({qbit, value["-"].GetDouble()});
      }
    }
  } else if (properties.keyExists<std::vector<double>>("p01s") &&
             properties.keyExists<std::vector<double>>("p10s")) {

    auto p01s = properties.get<std::vector<double>>("p01s");
    auto p10s = properties.get<std::vector<double>>("p10s");
    for (int i = 0; i < p01s.size(); i++) {

      piplus.insert({i, p01s[i] + p10s[i]});
      piminus.insert({i, p01s[i] - p10s[i]});
    }
  } else {
    xacc::warning(
        "Accelerator does not have readout error properties and Cannot "
        "find readout erro file (key 'file'). Skipping ReadoutError "
        "correction.");
    return;
  }

  // Get the number of shots first
  int nShots = 0;
  std::map<std::string, int> tmpCounts;
  for (auto &b : buffers) {
    if (!b->getMeasurementCounts().empty()) {
      tmpCounts = b->getMeasurementCounts();
      break;
    }
  }
  for (auto &kv : tmpCounts) {
    nShots += kv.second;
  }
  int counter = 0;
  for (auto &b : buffers) {
    auto counts = b->getMeasurementCounts();
    auto functionName = b->name();
    auto f = nameToFunction[functionName];
    auto [fSupports, support_to_creg_map] = supportSets[functionName];
    auto exp_val = b->getExpectationValueZ();
    auto fixedExp = 0.0;
    for (auto &kv : counts) {
      auto prod = 1.0;
      std::string bitString = kv.first;
      auto count = kv.second;
      for (auto &j : fSupports) {
        auto denom = (1.0 - piplus[j]);
        double numerator =
            (bitString[bitString.length() - 1 - support_to_creg_map[j]] == '1'
                 ? -1
                 : 1) -
            piminus[j];
        prod *= (numerator / denom);
      }
      fixedExp += ((double)count / (double)nShots) * prod;
    }
    if (fixedExp > 1.0) {
      fixedExp = 1.0;
    }
    if (fixedExp < -1.0) {
      fixedExp = -1.0;
    }
    b->addExtraInfo("ro-fixed-exp-val-z", ExtraInfo(fixedExp));

    counter++;
  }

  return;
}

} // namespace quantum
} // namespace xacc