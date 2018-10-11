#include "ROErrorDecorator.hpp"
#include "InstructionIterator.hpp"
#include "XACC.hpp"
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace xacc {
namespace quantum {
void ROErrorDecorator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                               const std::shared_ptr<Function> function) {

  decoratedAccelerator->execute(buffer, function);

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
      prod *= (std::pow(-1, (int)(bitString[j] - '0')) - piminus[j]) /
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

  auto buffers = decoratedAccelerator->execute(buffer, functions);

  // Goal here is take the resultant buffers and add a new
  // ExtraInfo key, 'ro-fixed-exp-val-z'.
  // To do this, we need to get the readout error fidelities
  // from the user provided characterization file 
  // This will give us p(+-)_i and we can use that to shift the
  // current exp-val-z values on the buffers.

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

  if (!xacc::optionExists("ro-error-file")) {
    xacc::info("Cannot find ro-error-file. Skipping ReadoutError "
               "correction.");
    return buffers;
  }

  // Get RO error probs
  auto roeStr = xacc::getOption("ro-error-file");
  buffer->addExtraInfo("ro-error-file", ExtraInfo(roeStr));

  std::ifstream t(roeStr);
  std::string json((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());

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
  auto tmpCounts = buffers[0]->getMeasurementCounts();
  for (auto &kv : tmpCounts) {
    nShots += kv.second;
  }

  int counter = 0;
  for (auto &b : buffers) {
    auto counts = b->getMeasurementCounts();

    auto fixedExp = 0.0;
    for (auto &kv : counts) {
      auto prod = 1.0;
      std::string bitString = kv.first;
      auto count = kv.second;
      for (auto j : supports(functions[counter])) {
        std::stringstream s;
        auto denom = (1.0 - piplus[j]);
        auto numerator =
            (bitString[buffer->size() - 1 - j] == '1' ? -1 : 1) - piminus[j];
        prod *= (numerator / denom);
      }

      fixedExp += ((double)count / (double)nShots) * prod;
    }

    b->addExtraInfo("ro-fixed-exp-val-z", ExtraInfo(fixedExp));

    counter++;
  }

  return buffers;
}

} // namespace quantum
} // namespace xacc