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
const std::string Client::post(const std::string& remoteUrl,
		const std::string& path, const std::string& postStr,
		std::map<std::string, std::string> headers) {

	using namespace RestClient;
	init();
	Connection* conn = new Connection(remoteUrl);
	HeaderFields _headers;
	if (headers.empty()) {
		headers.insert(std::make_pair("Content-type", "application/json"));
		headers.insert(std::make_pair("Connection", "keep-alive"));
		headers.insert(std::make_pair("Accept", "*/*"));
	}
	for (auto& kv : headers) {
		_headers[kv.first] = kv.second;
	}
	conn->SetHeaders(headers);
	Response r = conn->post(path, postStr);
	if (r.code != 200) throw std::runtime_error("HTTP POST Error");
	delete conn;
	disable();
	return r.body;
}

const std::string Client::get(const std::string& remoteUrl,
		const std::string& path, std::map<std::string, std::string> headers) {

	using namespace RestClient;
	Response r = RestClient::get(remoteUrl+path);
	if (r.code != 200) throw std::runtime_error("HTTP GET Error");
	return r.body;
}

void RemoteAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
			const std::shared_ptr<Function> function) {

	auto jsonPostStr = processInput(buffer, std::vector<std::shared_ptr<Function>> {
			function });

	auto responseStr = handleExceptionRestClientPost(remoteUrl, postPath, jsonPostStr, headers);

	processResponse(buffer, responseStr);

	return;

}

std::vector<std::shared_ptr<AcceleratorBuffer>> RemoteAccelerator::execute(
		std::shared_ptr<AcceleratorBuffer> buffer,
		const std::vector<std::shared_ptr<Function>> functions) {
	auto jsonPostStr = processInput(buffer, functions);

	auto responseStr = handleExceptionRestClientPost(remoteUrl, postPath, jsonPostStr, headers);

	return processResponse(buffer, responseStr);
}

}
