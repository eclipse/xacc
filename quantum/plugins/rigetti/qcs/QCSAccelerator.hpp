/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_

#include "InstructionIterator.hpp"
#include "QuilVisitor.hpp"
#include "CLIParser.hpp"
#include "RemoteAccelerator.hpp"

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
  bool hardwareDependent() { return true; }
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
                                                  const int size);

  virtual std::shared_ptr<AcceleratorBuffer>
  createBuffer(const std::string &varId);

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<Function> function) override;
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) override;

  virtual void initialize() {
  void*const libpython_handle = dlopen("libpython3.6m.so", RTLD_LAZY | RTLD_GLOBAL);
    if (xacc::optionExists("qcs-backend")) {
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
  }

  std::vector<std::pair<int, int>> getAcceleratorConnectivity() {
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
  virtual bool isValidBufferSize(const int NBits);

  /**
   * This Accelerator models QPU Gate accelerators.
   * @return
   */
  virtual AcceleratorType getType() { return AcceleratorType::qpu_gate; }

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
  virtual OptionPairs getOptions() {
    OptionPairs desc{{"qcs-shots", "Provide the number of trials to execute."},
                     {"qcs-backend", ""}};
    return desc;
  }

  virtual const std::string name() const { return "qcs"; }

  virtual const std::string description() const { return ""; }

  /**
   * The destructor
   */
  virtual ~QCSAccelerator() {}
};

} // namespace quantum
} // namespace xacc

#endif
