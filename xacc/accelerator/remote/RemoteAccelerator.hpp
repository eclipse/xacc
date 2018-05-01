/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_ACCELERATOR_REMOTE_REMOTEACCELERATOR_HPP_
#define XACC_ACCELERATOR_REMOTE_REMOTEACCELERATOR_HPP_

#include "Accelerator.hpp"
#include "XACC.hpp"

namespace xacc {

class Client {

public:

	virtual const std::string post(const std::string& remoteUrl,
			const std::string& path, const std::string& postStr,
			std::map<std::string, std::string> headers = std::map<std::string,
					std::string> { });

	virtual const std::string get(const std::string& remoteUrl,
			const std::string& path,
			std::map<std::string, std::string> headers = std::map<std::string,
					std::string> { });

	virtual ~Client() {}


};

class RemoteAccelerator : public Accelerator {

public:

	RemoteAccelerator() : Accelerator(), restClient(std::make_shared<Client>()) {}

	RemoteAccelerator(std::shared_ptr<Client> client) : restClient(client) {}


	/**
	 * Execute the provided XACC IR Function on the provided AcceleratorBuffer.
	 *
	 * @param buffer The buffer of bits this Accelerator should operate on.
	 * @param function The kernel to execute.
	 */
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
				const std::shared_ptr<Function> function);

	/**
	 * Execute a set of kernels with one remote call. Return
	 * a list of AcceleratorBuffers that provide a new view
	 * of the given one AcceleratorBuffer. The ith AcceleratorBuffer
	 * contains the results of the ith kernel execution.
	 *
	 * @param buffer The AcceleratorBuffer to execute on
	 * @param functions The list of IR Functions to execute
	 * @return tempBuffers The list of new AcceleratorBuffers
	 */
	virtual std::vector<std::shared_ptr<AcceleratorBuffer>> execute(
			std::shared_ptr<AcceleratorBuffer> buffer,
			const std::vector<std::shared_ptr<Function>> functions);

	virtual bool isRemote() {
		return true;
	}

protected:

	std::shared_ptr<Client> restClient;

	std::string postPath;

	std::string remoteUrl;

	std::map<std::string, std::string> headers;

	/**
	 * take ir, generate json post string
	 */
	virtual const std::string processInput(std::shared_ptr<AcceleratorBuffer> buffer,
			std::vector<std::shared_ptr<Function>> functions) = 0;

	/**
	 * take response and create
	 */
	virtual std::vector<std::shared_ptr<AcceleratorBuffer>> processResponse(
			std::shared_ptr<AcceleratorBuffer> buffer,
			const std::string& response) = 0;

	std::string handleExceptionRestClientPost(const std::string& _url, const std::string& path, const std::string& postStr,
			std::map<std::string, std::string> headers) {
		std::string postResponse;
		int retries = 10;
		std::exception ex;
		bool succeeded = false;

		// Execute HTTP Post
		do {
			try {
				postResponse = restClient->post(_url, path, postStr, headers);
				succeeded = true;
				break;
			} catch (std::exception& e) {
				ex = e;
				xacc::info("Remote Accelerator " + name() + " caught exception while calling restClient->post() "
						"- " + std::string(e.what()));
				retries--;
				if (retries > 0) {
					xacc::info("Retrying HTTP Post.");
				}
			}
		} while (retries > 0);

		if (!succeeded) {
			xacc::error("Remote Accelerator " + name() + " failed HTTP Post for Job Response - " + std::string(ex.what()));
		}

		return postResponse;
	}

	std::string handleExceptionRestClientGet(const std::string& _url, const std::string& path) {
		std::string getResponse;
		int retries = 10;
		std::exception ex;
		bool succeeded = false;
		// Execute HTTP Get
		do {
			try {
				getResponse = restClient->get(_url, path);
				succeeded = true;
				break;
			} catch (std::exception& e) {
				ex = e;
				xacc::info("Remote Accelerator " + name() + " caught exception while calling restClient->get() "
						"- " + std::string(e.what()));
				retries--;
				if (retries > 0) {
					xacc::info("Retrying HTTP Get.");
				}
			}
		} while (retries > 0);

		if (!succeeded) {
			xacc::error("Remote Accelerator " + name() + " failed HTTP Get for Job Response - " + std::string(ex.what()));
		}

		return getResponse;
	}



};

}

#endif
