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
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

namespace xacc {

class RestClient {

public:

	virtual const std::string post(const std::string& remoteUrl,
			const std::string& path, const std::string& postStr,
			std::map<std::string, std::string> headers = std::map<std::string,
					std::string> { }) {

		if (headers.empty()) {
			headers.insert(std::make_pair("Content-type", "application/json"));
			headers.insert(std::make_pair("Connection", "keep-alive"));
			headers.insert(std::make_pair("Accept", "*/*"));
		}

//		std::cout <<"URL: " << remoteUrl << "\n";
//		std::cout << "POST STRING: " << postStr << "\n";
//		std::cout << "PATH: " << path << "\n";
//		for (auto& kv : headers) {
//			std::cout << "HEADER: " << kv.first << ", " << kv.second << "\n";
//		}

		http::uri uri = http::uri(remoteUrl);
		http_client postClient(
				http::uri_builder(uri).append_path(U(path)).to_uri());
		http_request postRequest(methods::POST);
		for (auto& kv : headers) {
			postRequest.headers().add(kv.first, kv.second);
		}
		postRequest.set_body(postStr);

		// Post the problem, get the response as json
		auto postResponse = postClient.request(postRequest);
		auto respJson = postResponse.get().extract_string().get();

		// Map that response to a string
		std::stringstream ss;
		ss << respJson;
		return ss.str();
	}

	virtual const std::string get(const std::string& remoteUrl,
			const std::string& path,
			std::map<std::string, std::string> headers = std::map<std::string,
					std::string> { }) {
		http_client getClient(
				http::uri_builder(http::uri(remoteUrl)).append_path(U(path)).to_uri());
		http_request getRequest(methods::GET);
		for (auto& kv : headers) {
			getRequest.headers().add(kv.first, kv.second);
		}
		auto getResponse = getClient.request(getRequest);
		// get the result as a string
		std::stringstream z;
		z << getResponse.get().extract_json().get();
		return z.str();
	}

	virtual ~RestClient() {}

};

class RemoteAccelerator : public Accelerator {

public:

	RemoteAccelerator() : Accelerator(), restClient(std::make_shared<RestClient>()) {}

	RemoteAccelerator(std::shared_ptr<RestClient> client) : restClient(client) {}


	/**
	 * Execute the provided XACC IR Function on the provided AcceleratorBuffer.
	 *
	 * @param buffer The buffer of bits this Accelerator should operate on.
	 * @param function The kernel to execute.
	 */
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
				const std::shared_ptr<Function> function) {

		auto jsonPostStr = processInput(buffer, std::vector<std::shared_ptr<Function>> {
				function });

		auto responseStr = restClient->post(remoteUrl, postPath, jsonPostStr, headers);

		processResponse(buffer, responseStr);

		return;

	}

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
			const std::vector<std::shared_ptr<Function>> functions) {
		auto jsonPostStr = processInput(buffer, functions);

		auto responseStr = restClient->post(remoteUrl, postPath, jsonPostStr, headers);

		return processResponse(buffer, responseStr);
	}
protected:

	std::shared_ptr<RestClient> restClient;

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

};

}

#endif
