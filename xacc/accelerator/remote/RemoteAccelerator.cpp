/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "RemoteAccelerator.hpp"

namespace xacc {
const std::string RestClient::post(const std::string& remoteUrl,
		const std::string& path, const std::string& postStr,
		std::map<std::string, std::string> headers) {

	if (headers.empty()) {
		headers.insert(std::make_pair("Content-type", "application/json"));
		headers.insert(std::make_pair("Connection", "keep-alive"));
		headers.insert(std::make_pair("Accept", "*/*"));
	}

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

const std::string RestClient::get(const std::string& remoteUrl,
		const std::string& path, std::map<std::string, std::string> headers) {
	http_client getClient(
			http::uri_builder(http::uri(remoteUrl)).append_path(U(path)).to_uri());
	http_request getRequest(methods::GET);
	for (auto& kv : headers) {
		getRequest.headers().add(kv.first, kv.second);
	}
	auto getResponse = getClient.request(getRequest);
	// get the result as a string
	std::stringstream z;
	try {
		z << getResponse.get().extract_json().get();
//			xacc::info("GET RESPONSE: " + z.str());
	} catch (std::exception& e) {
		std::cout << "HELLO WORLD!, Caught exception, " << e.what() << "\n";
	}

	return z.str();
}

void RemoteAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
			const std::shared_ptr<Function> function) {

	auto jsonPostStr = processInput(buffer, std::vector<std::shared_ptr<Function>> {
			function });

	auto responseStr = restClient->post(remoteUrl, postPath, jsonPostStr, headers);

	processResponse(buffer, responseStr);

	return;

}

std::vector<std::shared_ptr<AcceleratorBuffer>> RemoteAccelerator::execute(
		std::shared_ptr<AcceleratorBuffer> buffer,
		const std::vector<std::shared_ptr<Function>> functions) {
	auto jsonPostStr = processInput(buffer, functions);

	auto responseStr = restClient->post(remoteUrl, postPath, jsonPostStr, headers);

	return processResponse(buffer, responseStr);
}

}
