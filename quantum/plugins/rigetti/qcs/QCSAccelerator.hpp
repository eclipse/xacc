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
#ifndef QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_

#include "InstructionIterator.hpp"
#include "QuilVisitor.hpp"
#include "CLIParser.hpp"
#include "RemoteAccelerator.hpp"

#include <pybind11/embed.h>

#include <dlfcn.h>
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
using namespace rapidjson;

using namespace xacc;

namespace xacc {
namespace quantum {

class MapToPhysical : public xacc::IRTransformation {
protected:
  std::vector<std::pair<int, int>> _edges;
public:
  MapToPhysical(std::vector<std::pair<int, int>> &edges) : _edges(edges) {}
  std::shared_ptr<IR> transform(std::shared_ptr<IR> ir) override;
  bool hardwareDependent() override { return true; }
  const std::string name() const override { return "qcs-map-qubits"; }
  const std::string description() const override { return ""; }
};

/**
 *
 */
class QCSAccelerator : virtual public Accelerator {
protected:
  std::vector<int> physicalQubits;
  std::vector<std::pair<int, int>> latticeEdges;
  Document latticeJson;

  pybind11::scoped_interpreter * guard;

public:
  QCSAccelerator() : Accelerator() {}

  /**
   * Create, store, and return an AcceleratorBuffer with the given
   * variable id string and of the given number of bits.
   * The string id serves as a unique identifier
   * for future lookups and reuse of the AcceleratorBuffer.
   *
   * @param varId
   * @param size
   * @return
   */
  std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string &varId,
                                                  const int size) override;

  std::shared_ptr<AcceleratorBuffer>
  createBuffer(const std::string &varId) override;

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<Function> function) override;
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) override;

  void initialize() override {
    void*const libpython_handle = dlopen("libpython3.6m.so", RTLD_LAZY | RTLD_GLOBAL);
    if (xacc::optionExists("qcs-backend") && !xacc::isPyApi) {
      auto backend = xacc::getOption("qcs-backend");

      if (backend.find("-qvm") != std::string::npos) {
         backend.erase(backend.find("-qvm"), 4);
      }

      Client client;
      auto response = client.get("https://forest-server.qcs.rigetti.com",
                                 "/lattices/" + backend);

      latticeJson.Parse(response);
      const auto &oneq = latticeJson["lattice"]["isa"]["1Q"];
      auto &twoq = latticeJson["lattice"]["isa"]["2Q"];

      for (auto itr = oneq.MemberBegin(); itr != oneq.MemberEnd(); ++itr) {
        physicalQubits.push_back(std::stoi(itr->name.GetString()));
      }
      for (auto itr = twoq.MemberBegin(); itr != twoq.MemberEnd(); ++itr) {
        auto connStr = itr->name.GetString();
        auto split = xacc::split(connStr, '-');
        latticeEdges.push_back({std::stoi(split[0]), std::stoi(split[1])});
      }
    }

    if (!xacc::isPyApi && !guard) {
        guard = new pybind11::scoped_interpreter();
    }
  }

  std::vector<std::pair<int, int>> getAcceleratorConnectivity() override{
    if (!latticeEdges.empty()) {
      return latticeEdges;
    }
    return std::vector<std::pair<int, int>>{};
  }
  /**
   * Return true if this Accelerator can allocated
   * NBits number of bits.
   * @param NBits
   * @return
   */
  bool isValidBufferSize(const int NBits) override;

  /**
   * This Accelerator models QPU Gate accelerators.
   * @return
   */
  AcceleratorType getType() override { return AcceleratorType::qpu_gate; }

  /**
   * We have no need to transform the IR for this Accelerator,
   * so return an empty list, for now.
   * @return
   */
  std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() override {
    std::vector<std::shared_ptr<IRTransformation>> v;
    if (!latticeEdges.empty()) {
      v.push_back(std::make_shared<MapToPhysical>(latticeEdges));
    }
    return v;
  }

  /**
   * Return all relevant RigettiAccelerator runtime options.
   * Users can set the api-key, execution type, and number of triels
   * from the command line with these options.
   */
  OptionPairs getOptions() override {
    OptionPairs desc{{"qcs-shots", "Provide the number of trials to execute."},
                     {"qcs-backend", ""}};
    return desc;
  }

  const std::string name() const override { return "qcs"; }
  const std::string description() const override { return ""; }

  /**
   * The destructor
   */
  virtual ~QCSAccelerator() { delete guard;}
};

} // namespace quantum
} // namespace xacc

#endif
