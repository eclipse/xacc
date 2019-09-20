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
#ifndef XACC_ACCELERATOR_REMOTE_REMOTEACCELERATOR_HPP_
#define XACC_ACCELERATOR_REMOTE_REMOTEACCELERATOR_HPP_

#include "Accelerator.hpp"

namespace xacc {

class Client {

public:
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

  virtual ~Client() {}
};

class RemoteAccelerator : public Accelerator {

public:
  RemoteAccelerator() : Accelerator(), restClient(std::make_shared<Client>()) {}

  RemoteAccelerator(std::shared_ptr<Client> client) : restClient(client) {}
  void updateConfiguration(const HeterogeneousMap &config) override {}

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> circuit) override;

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   circuits) override;

  bool isRemote() override { return true; }
  void setClient(std::shared_ptr<Client> client) { restClient = client; }

protected:
  std::shared_ptr<Client> restClient;
  std::string postPath;
  std::string remoteUrl;
  std::map<std::string, std::string> headers;

  virtual const std::string
  processInput(std::shared_ptr<AcceleratorBuffer> buffer,
               std::vector<std::shared_ptr<CompositeInstruction>> circuits) = 0;

  virtual void processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                               const std::string &response) = 0;

  std::string handleExceptionRestClientPost(
      const std::string &_url, const std::string &path,
      const std::string &postStr, std::map<std::string, std::string> headers);

  std::string handleExceptionRestClientGet(
      const std::string &_url, const std::string &path,
      std::map<std::string, std::string> headers =
          std::map<std::string, std::string>{},
      std::map<std::string, std::string> extraParams = {});
};

} // namespace xacc

#endif
