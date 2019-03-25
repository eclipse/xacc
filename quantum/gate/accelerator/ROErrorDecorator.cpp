#include "ROErrorDecorator.hpp"
#include "InstructionIterator.hpp"
#include "XACC.hpp"
#include <fstream>
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
using namespace rapidjson;

namespace xacc {
namespace quantum {
void ROErrorDecorator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                               const std::shared_ptr<Function> function) {

  if (decoratedAccelerator) decoratedAccelerator->execute(buffer, function);

  if (!xacc::optionExists("ro-error-file")) {
    xacc::info("Cannot find ro-error-file. Skipping ReadoutError "
               "correction.");
    return;
  }

  // Get RO error probs
  auto roeStr = xacc::getOption("ro-error-file");
  buffer->addExtraInfo("ro-error-file", ExtraInfo(roeStr));

  std::ifstream t(roeStr);
  std::string json((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());

  if (json.empty()) {
      xacc::error("Invalid ROError JSON file: " + roeStr);
  }

  Document d;
  d.Parse(json);
  std::map<int, double> piplus, piminus;

  for (auto itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
    std::string key = itr->name.GetString();
    if (key.compare("shots") != 0 && key.compare("backend") != 0) {
      auto &value = d[itr->name.GetString()];
      auto qbit = std::stoi(key);
      piplus.insert({qbit, value["+"].GetDouble()});
      piminus.insert({qbit, value["-"].GetDouble()});
    }
  }

  auto supports = [](std::shared_ptr<Function> f) {
    std::set<int> supportSet;
    InstructionIterator it(f);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->name() == "Measure") {
        auto bits = nextInst->bits();
        for (auto &b : bits)
          supportSet.insert(b);
      }
    }
    return supportSet;
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
    for (auto j : supports(function)) {
      prod *= (std::pow(-1, (int)(bitString[buffer->size()-j-1] - '0')) - piminus[j]) /
              (1.0 - piplus[j]);
    }

    fixedExp += ((double)count / (double)nShots) * prod;
  }

  buffer->addExtraInfo("ro-fixed-exp-val-z", ExtraInfo(fixedExp));

  return;
}

std::vector<std::shared_ptr<AcceleratorBuffer>> ROErrorDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<Function>> functions) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;
  std::map<std::string,std::shared_ptr<Function>> nameToFunction;
  std::map<std::string, std::set<int>> supportSets;

  if (decoratedAccelerator) {
    buffers = decoratedAccelerator->execute(buffer, functions);
  } else {
    buffers = buffer->getChildren();
  }

  if (!xacc::optionExists("ro-error-file")) {
    xacc::info("Cannot find ro-error-file. Skipping ReadoutError "
               "correction.");
    return buffers;
  }

  auto supports = [](std::shared_ptr<Function> f) {
    std::set<int> supportSet;
    InstructionIterator it(f);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->name() == "Measure") {
        auto bits = nextInst->bits();
        for (auto &b : bits)
          supportSet.insert(b);
      }
    }
    return supportSet;
  };

  for (auto& f : functions) {
      nameToFunction.insert({f->name(), f});
      supportSets.insert({f->name(), supports(f)});
  }

  // Get RO error probs
  auto roeStr = xacc::getOption("ro-error-file");
  buffer->addExtraInfo("ro-error-file", ExtraInfo(roeStr));

  std::ifstream t(roeStr);
  std::string json((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());
  if (json.empty()) {
      xacc::error("Invalid ROError JSON file: " + roeStr);
  }

  Document d;
  d.Parse(json);
  std::map<int, double> piplus, piminus;

  for (auto itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
    std::string key = itr->name.GetString();
    if (key.compare("shots") != 0 && key.compare("backend") != 0) {
      auto &value = d[itr->name.GetString()];
      auto qbit = std::stoi(key);
      piplus.insert({qbit, value["+"].GetDouble()});
      piminus.insert({qbit, value["-"].GetDouble()});
    }
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
    auto fSupports = supportSets[functionName];

    auto fixedExp = 0.0;
    for (auto &kv : counts) {
      auto prod = 1.0;
      std::string bitString = kv.first;
      auto count = kv.second;
      for (auto& j : fSupports) {
        std::stringstream s;
        auto denom = (1.0 - piplus[j]);
        auto numerator =
            (bitString[buffer->size() - 1 - j] == '1' ? -1 : 1) - piminus[j];
        prod *= (numerator / denom);
      }
      fixedExp += ((double)count / (double)nShots) * prod;
    }

    // Correct in case our shift has gone outside physical bounds
    if (fixedExp > 1.0) {fixedExp = 1.0;}
    if (fixedExp < -1.0) {fixedExp = -1.0;}

    b->addExtraInfo("ro-fixed-exp-val-z", ExtraInfo(fixedExp));

    counter++;
  }

  return buffers;
}

} // namespace quantum
} // namespace xacc