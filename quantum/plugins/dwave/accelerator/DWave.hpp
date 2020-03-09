/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#ifndef XACC_DWAVE_HPP_
#define XACC_DWAVE_HPP_

#include "AcceleratorDecorator.hpp"
#include "Utils.hpp"
#include "xacc_service.hpp"
#include <unordered_set>
#include "dwave_sapi.h"
#include "xacc.hpp"

namespace xacc {

namespace quantum {

class DWave : public Accelerator {
protected:
  double chain_strength = -1.;
  double shots = 100;
  std::string backend = "";
  std::string apiKey;
  sapi_Solver *solver = NULL;
  sapi_Connection *connection = NULL;
  const sapi_SolverProperties *solver_properties = NULL;
  std::string default_emb_algo = "cmr";

  void searchAPIKey(std::string &key);
  void findApiKeyInFile(std::string &key, const std::string &p);

public:
  DWave() {}

  void initialize(const HeterogeneousMap &params = {}) override {
    // decoratedAccelerator = xacc::getService<Accelerator>("dwave-internal");
    // decoratedAccelerator->initialize(params);
    updateConfiguration(params);

    searchAPIKey(apiKey);

    char err_msg[SAPI_ERROR_MESSAGE_MAX_SIZE];
    sapi_globalInit();
    auto code =
        sapi_remoteConnection("https://cloud.dwavesys.com/sapi", apiKey.c_str(),
                              NULL, &connection, err_msg);

    std::vector<std::string> available_solvers;
    auto solver_names = sapi_listSolvers(connection);
    for (int i = 0; solver_names[i] != NULL; ++i) {
      available_solvers.push_back(solver_names[i]);
    }

    if (!xacc::container::contains(available_solvers, backend)) {
        std::stringstream ss;
        ss << available_solvers;
        xacc::error("[Dwave Backend] Solver " + backend + " is not a valid solver. Must be one of "+ss.str()+"");
    }
    solver = sapi_getSolver(connection, backend.c_str());
    solver_properties = sapi_getSolverProperties(solver);
  }

  void updateConfiguration(const HeterogeneousMap &params) override {
    if (params.keyExists<double>("chain_strength")) {
      chain_strength = params.get<double>("chain_strength");
    }
    if (params.keyExists<int>("shots")) {
      shots = params.get<int>("shots");
    }
    if (params.stringExists("backend")) {
      backend = params.getString("backend");
    } else {
        xacc::error("[Dwave Backend] You must provide the backend name when you request this Accelerator.");
    }
    if (params.stringExists("embedding-algorithm")) {
        default_emb_algo = params.getString("embedding-algorithm");
    }
  }

  std::vector<std::pair<int, int>> getConnectivity() override;

  const std::string getSignature() override { return "dwave:" + backend; }
  const std::vector<std::string> configurationKeys() override { return {}; }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override;

  const std::string name() const override { return "dwave"; }
  const std::string description() const override { return ""; }

  ~DWave() override {}
};

} // namespace quantum
} // namespace xacc
#endif
