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
#include "RemoteAccelerator.hpp"

#include <fstream>
#include "IRTransformation.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include <dlfcn.h>

#include <pybind11/embed.h>

namespace py = pybind11;

using namespace xacc;

namespace xacc {
namespace quantum {

class QCSRestClient {

protected:
  bool verbose = false;

public:
  void setVerbose(bool v) { verbose = v; }

  virtual const std::string post(const std::string &remoteUrl,
                                 const std::string &path,
                                 const std::string &postStr,
                                 std::map<std::string, std::string> headers =
                                     std::map<std::string, std::string>{});
  virtual const std::string
  get(const std::string &remoteUrl, const std::string &path,
      std::map<std::string, std::string> headers =
          std::map<std::string, std::string>{},
      std::map<std::string, std::string> extraParams = {});

  virtual ~QCSRestClient() {}
};

// ResultsDecoder is a utility class for
// taking the raw binary results from the QPU
// and converting to bit strings and pushing to the
// AcceleratorBuffer
class ResultsDecoder {
public:
  void decode(std::shared_ptr<AcceleratorBuffer> buffer, py::object results,
               int shots);
};

class QCSPlacement : public IRTransformation {
protected:
  std::string backend;
  std::string isa;
public:
  QCSPlacement() {}
  QCSPlacement(const std::string& _backend, const std::string& _isa) : backend(_backend), isa(_isa) {}
  void apply(std::shared_ptr<CompositeInstruction> program,
             const std::shared_ptr<Accelerator> acc,
             const HeterogeneousMap &options = {}) override;
  const IRTransformationType type() const override {
    return IRTransformationType::Placement;
  }
  const std::string name() const override { return "qcs-quilc"; }
  const std::string description() const override { return ""; }
};

class QCSAccelerator : virtual public Accelerator {
protected:
  std::vector<int> physicalQubits;
  std::vector<std::pair<int, int>> latticeEdges;
  std::string backend;
  std::string endpoint;

  std::string engagement_url = "https://api.qcs.rigetti.com";
  std::string qpu_compiler_url = "https://translation.services.qcs.rigetti.com";
  std::string forest_server_url = "https://forest-server.qcs.rigetti.com";

  int shots = 1024;
  
  std::string qpu_compiler_endpoint;
  std::string qpu_endpoint;
  std::shared_ptr<QCSRestClient> restClient;

  std::string client_public = "";
  std::string client_secret = "";
  std::string server_public = "";
  std::string auth_token = "";
  std::string user_id = "";
  bool use_rpcq_auth_config = true;

  bool initialized = false;
  std::shared_ptr<py::scoped_interpreter> guard;

  void _internal_init();
  
public:
  QCSAccelerator()
      : Accelerator(), restClient(std::make_shared<QCSRestClient>()) {}

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override;

  void initialize(const HeterogeneousMap &params = {}) override;

  void updateConfiguration(const HeterogeneousMap &config) override {
    if (config.keyExists<int>("shots")) {
      shots = config.get<int>("shots");
    }
    if (config.stringExists("backend")) {
      backend = config.getString("backend");
    }
  }

  const std::vector<std::string> configurationKeys() override {
    return {"shots", "backend"};
  }

  HeterogeneousMap getProperties() override { return HeterogeneousMap(); }

  const std::string getSignature() override { return "qcs:" + backend; }
  const std::string defaultPlacementTransformation() override {return "qcs-quilc";}

  std::vector<std::pair<int, int>> getConnectivity() override {
    if (!latticeEdges.empty()) {
      return latticeEdges;
    }
    return std::vector<std::pair<int, int>>{};
  }

  const std::string name() const override { return "qcs"; }
  const std::string description() const override { return ""; }

  std::string post(const std::string &_url, const std::string &path,
                   const std::string &postStr,
                   std::map<std::string, std::string> headers = {});

  std::string get(const std::string &_url, const std::string &path,
                  std::map<std::string, std::string> headers =
                      std::map<std::string, std::string>{},
                  std::map<std::string, std::string> extraParams = {});

  virtual ~QCSAccelerator() {}
};

} // namespace quantum
} // namespace xacc

#endif
