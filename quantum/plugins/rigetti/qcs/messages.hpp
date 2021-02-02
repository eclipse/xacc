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
#ifndef QUANTUM_GATE_ACCELERATORS_QCS_MESSAGES_HPP_
#define QUANTUM_GATE_ACCELERATORS_QCS_MESSAGES_HPP_

#include <msgpack.hpp>
#include <vector>
namespace xacc {
namespace qcs {

class BinaryExecutableRequest {
protected:
  int num_shots = 1024;
  std::string quil;
  std::string _type = "BinaryExecutableRequest";
public:
   BinaryExecutableRequest() = default;
   BinaryExecutableRequest(int s, std::string q) : quil(q), num_shots(s) {}
   BinaryExecutableRequest(const BinaryExecutableRequest& r) :quil(r.quil), num_shots(r.num_shots) {}
   MSGPACK_DEFINE_MAP(quil, num_shots, _type);
};

class GetBuffersRequest {
protected:
  bool wait = true;
  std::vector<std::string> args;
  public:
  GetBuffersRequest(std::string id) : args({id}) {}
  MSGPACK_DEFINE_MAP(wait, MSGPACK_NVP("*args", args));
};

struct QType {
  std::vector<int> shape;
  std::string dtype;
  std::vector<char> data;
  MSGPACK_DEFINE_MAP(shape, dtype, data);
};

class GetBuffersResponse {
public:
  std::string id = "";
  std::string jsonrpc = "2.0";
  std::map<std::string, QType> result;
  std::vector<int> warnings = {};
  std::string _type = "RPCReply";
  MSGPACK_DEFINE_MAP(id, jsonrpc, warnings, _type, result);
};

class RPCRequestGetBuffers {
protected:
  std::string id;
  std::string jsonrpc = "2.0";
  std::string method = "get_buffers";
  GetBuffersRequest& params;
  std::string _type = "RPCRequest";

public:
  RPCRequestGetBuffers(std::string i, GetBuffersRequest &p) : params(p), id(i) {}
  MSGPACK_DEFINE_MAP(method, id, jsonrpc, params, _type);
};

class BinaryExecutableParams {
protected:
  std::vector<BinaryExecutableRequest> args;

public:
  BinaryExecutableParams(BinaryExecutableRequest& a)  {
    args.push_back(a);
  }
  MSGPACK_DEFINE_MAP(MSGPACK_NVP("*args", args));
};

class RPCRequestBinaryExecutable {
protected:
  std::string id;
  std::string jsonrpc = "2.0";
  std::string method = "native_quil_to_binary";
  BinaryExecutableParams& params;
  std::string _type = "RPCRequest";

public:
  RPCRequestBinaryExecutable(std::string i, BinaryExecutableParams &p) :params(p), id(i) {}
  MSGPACK_DEFINE_MAP(method, id, jsonrpc, params, _type);
};
    

class QPURequest {
  protected:
    std::string program;
    std::string id;
    std::map<std::string,std::string> patch_values;
    std::string _type = "QPURequest";

  public:
    QPURequest(std::string p, std::string i) : program(p), id(i) {}
    MSGPACK_DEFINE_MAP(id, program, _type, patch_values);
};

class QPURequestParams {
protected:
  QPURequest& request;
  std::string user = "00u4i1atsfsrjqiiy356";
  int priority = 1;
public:
  QPURequestParams(QPURequest &a) : request(a) {}
  MSGPACK_DEFINE_MAP(MSGPACK_NVP("request", request), user, priority);
};

class RPCRequestQPURequest {
protected:
  std::string id;
  std::string jsonrpc = "2.0";
  std::string method = "execute_qpu_request";
  QPURequestParams& params;
  std::string _type = "RPCRequest";
  msgpack::type::nil_t client_key;
  msgpack::type::nil_t client_timeout;

public:
  RPCRequestQPURequest(std::string i, QPURequestParams &p) : params(p), id(i) {}
  MSGPACK_DEFINE_MAP(method, id, jsonrpc, params, _type, client_key, client_timeout);
};

}
}
#endif
