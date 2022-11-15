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
#ifndef XACC_HONEYWELL_ACCELERATOR_HPP_
#define XACC_HONEYWELL_ACCELERATOR_HPP_

#include "InstructionIterator.hpp"
#include "Accelerator.hpp"
#include <bitset>
#include <string>
#include <type_traits>

namespace xacc {
namespace quantum {

class RestClient {

protected:
  bool verbose = true;

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

  virtual ~RestClient() {}
};

#define IS_INTEGRAL(T)                                                         \
  typename std::enable_if<std::is_integral<T>::value>::type * = 0

template <class T>
std::string integral_to_binary_string(T byte, IS_INTEGRAL(T)) {
  std::bitset<sizeof(T) * 8> bs(byte);
  return bs.to_string();
}

std::string hex_string_to_binary_string(std::string hex);

class HoneywellAccelerator : public Accelerator {
public:
  void initialize(const HeterogeneousMap &params = {}) override;
  void updateConfiguration(const HeterogeneousMap &config) override {
    if (config.keyExists<int>("shots")) {
      shots = config.get<int>("shots");
    }
    if (config.stringExists("backend")) {
      backend = config.getString("backend");
    }
    if (config.keyExists<bool>("http-verbose")) {
      restClient->setVerbose(config.get<bool>("http-verbose"));
    }
  }

  const std::vector<std::string> configurationKeys() override {
    return {"shots", "backend"};
  }

  const std::string getSignature() override {
    return "honeywell";
  }

   const std::string name() const override { return "honeywell"; }
  const std::string description() const override {
    return "";
  }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> circuit) override;

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   circuits) override;
  std::string getNativeCode(std::shared_ptr<CompositeInstruction> program,
                            const HeterogeneousMap &config = {}) override;
  bool isRemote() override { return true; }

  HoneywellAccelerator()
      : Accelerator(), restClient(std::make_shared<RestClient>()) {}

  virtual ~HoneywellAccelerator() {}

private:
  void refresh_tokens(bool force = false);
  void searchAPIKey(std::string &key);
  void findApiKeyInFile(std::string &key, const std::string &p);
  std::map<std::string, std::string> generateRequestHeader() const;
  std::shared_ptr<RestClient> restClient;

  std::string api_key;
  std::string time_str;
  std::string refresh_key;
  std::string email;
  std::string job_name;
  std::string retrieve_job_id;

  int shots = 1024;
  std::string backend = "";
  bool initialized = false;
  static inline const std::string url = "https://qapi.honeywell.com/v1/";
  // List of backend names:
  std::vector<std::string> available_backends;
  std::string post(const std::string &_url, const std::string &path,
                   const std::string &postStr,
                   std::map<std::string, std::string> headers = {});

  std::string get(const std::string &_url, const std::string &path,
                  std::map<std::string, std::string> headers =
                      std::map<std::string, std::string>{},
                  std::map<std::string, std::string> extraParams = {});
};

} // namespace quantum
} // namespace xacc

#endif
