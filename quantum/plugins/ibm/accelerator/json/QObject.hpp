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
#ifndef QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_QOBJECT_HPP_
#define QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_QOBJECT_HPP_

#include "variant.hpp"
#include "json.hpp"

// #include <optional>
#include <stdexcept>
#include <regex>
#include <unordered_map>
#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
template <typename T> struct adl_serializer<std::shared_ptr<T>> {
  static void to_json(json &j, const std::shared_ptr<T> &opt) {
    if (!opt)
      j = nullptr;
    else
      j = *opt;
  }

  static std::shared_ptr<T> from_json(const json &j) {
    if (j.is_null())
      return std::unique_ptr<T>();
    else
      return std::unique_ptr<T>(new T(j.get<T>()));
  }
};
} // namespace nlohmann
#endif

namespace xacc {
namespace ibm {
using nlohmann::json;

inline json get_untyped(const json &j, const char *property) {
  if (j.find(property) != j.end()) {
    return j.at(property).get<json>();
  }
  return json();
}

inline json get_untyped(const json &j, std::string property) {
  return get_untyped(j, property.data());
}

template <typename T>
inline std::shared_ptr<T> get_optional(const json &j, const char *property) {
  if (j.find(property) != j.end()) {
    return j.at(property).get<std::shared_ptr<T>>();
  }
  return std::shared_ptr<T>();
}

template <typename T>
inline std::shared_ptr<T> get_optional(const json &j, std::string property) {
  return get_optional<T>(j, property.data());
}

class Backend {
public:
  Backend() = default;
  virtual ~Backend() = default;

private:
  std::string name;

public:
  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }
};

class QObjectConfig {
public:
  QObjectConfig() = default;
  virtual ~QObjectConfig() = default;

private:
  int64_t shots;
  int64_t n_qubits;
  int64_t memory_slots;
  bool memory;
  std::vector<nlohmann::json> parameter_binds;
  std::vector<nlohmann::json> meas_lo_freq;
  std::vector<nlohmann::json> schedule_los;
  std::vector<nlohmann::json> qubit_lo_freq;
  std::vector<nlohmann::json> qubit_lo_range;
  std::vector<nlohmann::json> meas_lo_range;
  std::string meas_return;
  int64_t meas_level;
  int64_t memory_slot_size;

public:
  const int64_t &get_shots() const { return shots; }
  int64_t &get_mutable_shots() { return shots; }
  void set_shots(const int64_t &value) { this->shots = value; }

  const int64_t &get_n_qubits() const { return n_qubits; }
  int64_t &get_mutable_n_qubits() { return n_qubits; }
  void set_n_qubits(const int64_t &value) { this->n_qubits = value; }

  const int64_t &get_memory_slots() const { return memory_slots; }
  int64_t &get_mutable_memory_slots() { return memory_slots; }
  void set_memory_slots(const int64_t &value) { this->memory_slots = value; }

  const bool &get_memory() const { return memory; }
  bool &get_mutable_memory() { return memory; }
  void set_memory(const bool &value) { this->memory = value; }

  const std::vector<nlohmann::json> &get_parameter_binds() const {
    return parameter_binds;
  }
  std::vector<nlohmann::json> &get_mutable_parameter_binds() {
    return parameter_binds;
  }
  void set_parameter_binds(const std::vector<nlohmann::json> &value) {
    this->parameter_binds = value;
  }

  const std::vector<nlohmann::json> &get_meas_lo_freq() const {
    return meas_lo_freq;
  }
  std::vector<nlohmann::json> &get_mutable_meas_lo_freq() {
    return meas_lo_freq;
  }
  void set_meas_lo_freq(const std::vector<nlohmann::json> &value) {
    this->meas_lo_freq = value;
  }

  const std::vector<nlohmann::json> &get_schedule_los() const {
    return schedule_los;
  }
  std::vector<nlohmann::json> &get_mutable_schedule_los() {
    return schedule_los;
  }
  void set_schedule_los(const std::vector<nlohmann::json> &value) {
    this->schedule_los = value;
  }

  const std::vector<nlohmann::json> &get_qubit_lo_freq() const {
    return qubit_lo_freq;
  }
  std::vector<nlohmann::json> &get_mutable_qubit_lo_freq() {
    return qubit_lo_freq;
  }
  void set_qubit_lo_freq(const std::vector<nlohmann::json> &value) {
    this->qubit_lo_freq = value;
  }

  const std::vector<nlohmann::json> &get_qubit_lo_range() const {
    return qubit_lo_range;
  }
  std::vector<nlohmann::json> &get_mutable_qubit_lo_range() {
    return qubit_lo_range;
  }
  void set_qubit_lo_range(const std::vector<nlohmann::json> &value) {
    this->qubit_lo_range = value;
  }

  const std::vector<nlohmann::json> &get_meas_lo_range() const {
    return meas_lo_range;
  }
  std::vector<nlohmann::json> &get_mutable_meas_lo_range() {
    return meas_lo_range;
  }
  void set_meas_lo_range(const std::vector<nlohmann::json> &value) {
    this->meas_lo_range = value;
  }

  const std::string &get_meas_return() const { return meas_return; }
  std::string &get_mutable_meas_return() { return meas_return; }
  void set_meas_return(const std::string &value) { this->meas_return = value; }

  const int64_t &get_meas_level() const { return meas_level; }
  int64_t &get_mutable_meas_level() { return meas_level; }
  void set_meas_level(const int64_t &value) { this->meas_level = value; }

  const int64_t &get_memory_slot_size() const { return memory_slot_size; }
  int64_t &get_mutable_memory_slot_size() { return memory_slot_size; }
  void set_memory_slot_size(const int64_t &value) {
    this->memory_slot_size = value;
  }
};

class ExperimentConfig {
public:
  ExperimentConfig() = default;
  virtual ~ExperimentConfig() = default;

private:
  int64_t n_qubits;
  int64_t memory_slots;

public:
  const int64_t &get_n_qubits() const { return n_qubits; }
  int64_t &get_mutable_n_qubits() { return n_qubits; }
  void set_n_qubits(const int64_t &value) { this->n_qubits = value; }

  const int64_t &get_memory_slots() const { return memory_slots; }
  int64_t &get_mutable_memory_slots() { return memory_slots; }
  void set_memory_slots(const int64_t &value) { this->memory_slots = value; }
};

using ClbitLabel = mpark::variant<int64_t, std::string>;

enum class QregSizeEnum : int { Q };

using QregSizeElement = mpark::variant<std::string, int64_t>;

class ExperimentHeader {
public:
  ExperimentHeader() = default;
  virtual ~ExperimentHeader() = default;

private:
  std::string name;
  std::vector<std::vector<QregSizeElement>> qreg_sizes;
  int64_t n_qubits;
  std::vector<std::vector<QregSizeElement>> qubit_labels;
  int64_t memory_slots;
  std::vector<std::vector<ClbitLabel>> creg_sizes;
  std::vector<std::vector<ClbitLabel>> clbit_labels;

public:
  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const std::vector<std::vector<QregSizeElement>> &get_qreg_sizes() const {
    return qreg_sizes;
  }
  std::vector<std::vector<QregSizeElement>> &get_mutable_qreg_sizes() {
    return qreg_sizes;
  }
  void set_qreg_sizes(const std::vector<std::vector<QregSizeElement>> &value) {
    this->qreg_sizes = value;
  }

  const int64_t &get_n_qubits() const { return n_qubits; }
  int64_t &get_mutable_n_qubits() { return n_qubits; }
  void set_n_qubits(const int64_t &value) { this->n_qubits = value; }

  const std::vector<std::vector<QregSizeElement>> &get_qubit_labels() const {
    return qubit_labels;
  }
  std::vector<std::vector<QregSizeElement>> &get_mutable_qubit_labels() {
    return qubit_labels;
  }
  void
  set_qubit_labels(const std::vector<std::vector<QregSizeElement>> &value) {
    this->qubit_labels = value;
  }

  const int64_t &get_memory_slots() const { return memory_slots; }
  int64_t &get_mutable_memory_slots() { return memory_slots; }
  void set_memory_slots(const int64_t &value) { this->memory_slots = value; }

  const std::vector<std::vector<ClbitLabel>> &get_creg_sizes() const {
    return creg_sizes;
  }
  std::vector<std::vector<ClbitLabel>> &get_mutable_creg_sizes() {
    return creg_sizes;
  }
  void set_creg_sizes(const std::vector<std::vector<ClbitLabel>> &value) {
    this->creg_sizes = value;
  }

  const std::vector<std::vector<ClbitLabel>> &get_clbit_labels() const {
    return clbit_labels;
  }
  std::vector<std::vector<ClbitLabel>> &get_mutable_clbit_labels() {
    return clbit_labels;
  }
  void set_clbit_labels(const std::vector<std::vector<ClbitLabel>> &value) {
    this->clbit_labels = value;
  }
};

struct Bfunc {
  Bfunc(int64_t in_regId, const std::string &in_hexMask,
        const std::string &in_relation = "==",
        const std::string &in_val = "")
      : registerId(in_regId), hex_mask(in_hexMask), relation(in_relation),
        hex_val(in_val.empty() ? in_hexMask : in_val) {}
  int64_t registerId;
  std::string hex_mask;
  std::string relation;
  std::string hex_val;
};

class RegisterAllocator {
  static inline RegisterAllocator *instance = nullptr;
  std::unordered_map<int64_t, int64_t> memoryToRegister;
  int64_t registerId;

  RegisterAllocator() { registerId = 0; }

public:
  static RegisterAllocator *getInstance() {
    if (!instance) {
      instance = new RegisterAllocator;
    }
    return instance;
  }

  std::optional<int64_t> getRegisterId(int64_t in_memoryId) {
    if (memoryToRegister.find(in_memoryId) != memoryToRegister.end()) {
      return memoryToRegister[in_memoryId];
    }
    return std::nullopt;
  }

  int64_t mapMemory(int64_t in_memoryId) {
    // Don't need to generate new mapping;
    // e.g. multiple if conditioned on the same measurement.
    auto iter = memoryToRegister.find(in_memoryId);
    if (iter != memoryToRegister.end()) {
      return iter->second;
    }
    memoryToRegister[in_memoryId] = registerId;
    registerId++;
    return memoryToRegister[in_memoryId];
  }

  int64_t getNextRegister() {
    const auto result = registerId;
    ++registerId;
    return result;
  }

  void reset() {
    memoryToRegister.clear();
    registerId = 0;
  }
};

class Instruction {
public:
  Instruction() = default;
  virtual ~Instruction() = default;
  // Construct a binary function instruction
  // Returns the Bfunc instruction and the register Id to condition the
  // sub-circuit.
  static Instruction createConditionalInst(int64_t memoryId) {
    // Map the memory Id to a register:
    const auto measRegisterId =
        RegisterAllocator::getInstance()->mapMemory(memoryId);
    const int64_t maskVal = 1ULL << measRegisterId;
    const std::string hexMaskStr = "0x" + std::to_string(maskVal);
    const int64_t resultRegisterId =
        RegisterAllocator::getInstance()->getNextRegister();
    Bfunc bFuncObj(resultRegisterId, hexMaskStr);
    Instruction newInst;
    newInst.bfunc = bFuncObj;
    return newInst;
  }

private:
  std::vector<int64_t> qubits;
  std::string name;
  std::vector<double> params;
  std::vector<int64_t> memory;
  // Conditional on a register value
  std::optional<int64_t> conditional;
  // If this is a Bfunc to compute a register value
  std::optional<Bfunc> bfunc;

public:
  const std::vector<int64_t> &get_qubits() const { return qubits; }
  std::vector<int64_t> &get_mutable_qubits() { return qubits; }
  void set_qubits(const std::vector<int64_t> &value) { this->qubits = value; }

  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  std::vector<double> get_params() const { return params; }
  void set_params(std::vector<double> value) { this->params = value; }

  std::vector<int64_t> get_memory() const { return memory; }
  void set_memory(std::vector<int64_t> value) { this->memory = value; }

  std::optional<Bfunc> get_bFunc() const { return bfunc; }
  void set_bFunc(Bfunc value) { this->bfunc = value; }
  bool isBfuc() const { return get_bFunc().has_value(); }

  std::optional<int64_t> get_condition_reg_id() const { return conditional; }
  void set_condition_reg_id(int64_t value) { this->conditional = value; }
};

class Experiment {
public:
  Experiment() = default;
  virtual ~Experiment() = default;

private:
  ExperimentConfig config;
  ExperimentHeader header;
  std::vector<Instruction> instructions;

public:
  const ExperimentConfig &get_config() const { return config; }
  ExperimentConfig &get_mutable_config() { return config; }
  void set_config(const ExperimentConfig &value) { this->config = value; }

  const ExperimentHeader &get_header() const { return header; }
  ExperimentHeader &get_mutable_header() { return header; }
  void set_header(const ExperimentHeader &value) { this->header = value; }

  const std::vector<Instruction> &get_instructions() const {
    return instructions;
  }
  std::vector<Instruction> &get_mutable_instructions() { return instructions; }
  void set_instructions(const std::vector<Instruction> &value) {
    this->instructions = value;
  }
};
class QObjectHeader {
public:
  QObjectHeader() = default;
  virtual ~QObjectHeader() = default;

private:
  std::string backend_version;
  std::string backend_name;

public:
  const std::string &get_backend_version() const { return backend_version; }
  std::string &get_mutable_backend_version() { return backend_version; }
  void set_backend_version(const std::string &value) {
    this->backend_version = value;
  }

  const std::string &get_backend_name() const { return backend_name; }
  std::string &get_mutable_backend_name() { return backend_name; }
  void set_backend_name(const std::string &value) {
    this->backend_name = value;
  }
};

class QObject {
public:
  QObject() = default;
  virtual ~QObject() = default;

private:
  std::string qobj_id;
  std::vector<Experiment> experiments;
  QObjectConfig config;
  std::string type;
  std::string schema_version;
  QObjectHeader header;

public:
  const std::string &get_qobj_id() const { return qobj_id; }
  std::string &get_mutable_qobj_id() { return qobj_id; }
  void set_qobj_id(const std::string &value) { this->qobj_id = value; }

  const std::vector<Experiment> &get_experiments() const { return experiments; }
  std::vector<Experiment> &get_mutable_experiments() { return experiments; }
  void set_experiments(const std::vector<Experiment> &value) {
    this->experiments = value;
  }

  const QObjectConfig &get_config() const { return config; }
  QObjectConfig &get_mutable_config() { return config; }
  void set_config(const QObjectConfig &value) { this->config = value; }

  const std::string &get_type() const { return type; }
  std::string &get_mutable_type() { return type; }
  void set_type(const std::string &value) { this->type = value; }

  const std::string &get_schema_version() const { return schema_version; }
  std::string &get_mutable_schema_version() { return schema_version; }
  void set_schema_version(const std::string &value) {
    this->schema_version = value;
  }

  const QObjectHeader &get_header() const { return header; }
  QObjectHeader &get_mutable_header() { return header; }
  void set_header(const QObjectHeader &value) { this->header = value; }
};

class QObjectRoot {
public:
  QObjectRoot() = default;
  virtual ~QObjectRoot() = default;

private:
  QObject q_object;
  Backend backend;
  int64_t shots;

public:
  const QObject &get_q_object() const { return q_object; }
  QObject &get_mutable_q_object() { return q_object; }
  void set_q_object(const QObject &value) { this->q_object = value; }

  const Backend &get_backend() const { return backend; }
  Backend &get_mutable_backend() { return backend; }
  void set_backend(const Backend &value) { this->backend = value; }

  const int64_t &get_shots() const { return shots; }
  int64_t &get_mutable_shots() { return shots; }
  void set_shots(const int64_t &value) { this->shots = value; }
};

class Data {
public:
  Data() = default;
  virtual ~Data() = default;

private:
  std::map<std::string, int64_t> counts;

public:
  const std::map<std::string, int64_t> &get_counts() const { return counts; }
  std::map<std::string, int64_t> &get_mutable_counts() { return counts; }
  void set_counts(const std::map<std::string, int64_t> &value) {
    this->counts = value;
  }
};

class Result {
public:
  Result() = default;
  virtual ~Result() = default;

private:
  Data data;
  ExperimentHeader header;
  int64_t meas_level;
  int64_t shots;
  bool success;

public:
  const Data &get_data() const { return data; }
  Data &get_mutable_data() { return data; }
  void set_data(const Data &value) { this->data = value; }

  const ExperimentHeader &get_header() const { return header; }
  ExperimentHeader &get_mutable_header() { return header; }
  void set_header(const ExperimentHeader &value) { this->header = value; }

  const int64_t &get_meas_level() const { return meas_level; }
  int64_t &get_mutable_meas_level() { return meas_level; }
  void set_meas_level(const int64_t &value) { this->meas_level = value; }

  const int64_t &get_shots() const { return shots; }
  int64_t &get_mutable_shots() { return shots; }
  void set_shots(const int64_t &value) { this->shots = value; }

  const bool &get_success() const { return success; }
  bool &get_mutable_success() { return success; }
  void set_success(const bool &value) { this->success = value; }
};

class QObjectResult {
public:
  QObjectResult() = default;
  virtual ~QObjectResult() = default;

private:
  std::string backend_name;
  std::string backend_version;
  std::string date;
  std::string execution_id;
  QObjectHeader header;
  std::string job_id;
  std::string qobj_id;
  std::vector<Result> results;
  std::string status;
  bool success;

public:
  const std::string &get_backend_name() const { return backend_name; }
  std::string &get_mutable_backend_name() { return backend_name; }
  void set_backend_name(const std::string &value) {
    this->backend_name = value;
  }

  const std::string &get_backend_version() const { return backend_version; }
  std::string &get_mutable_backend_version() { return backend_version; }
  void set_backend_version(const std::string &value) {
    this->backend_version = value;
  }

  const std::string &get_date() const { return date; }
  std::string &get_mutable_date() { return date; }
  void set_date(const std::string &value) { this->date = value; }

  const std::string &get_execution_id() const { return execution_id; }
  std::string &get_mutable_execution_id() { return execution_id; }
  void set_execution_id(const std::string &value) {
    this->execution_id = value;
  }

  const QObjectHeader &get_header() const { return header; }
  QObjectHeader &get_mutable_header() { return header; }
  void set_header(const QObjectHeader &value) { this->header = value; }

  const std::string &get_job_id() const { return job_id; }
  std::string &get_mutable_job_id() { return job_id; }
  void set_job_id(const std::string &value) { this->job_id = value; }

  const std::string &get_qobj_id() const { return qobj_id; }
  std::string &get_mutable_qobj_id() { return qobj_id; }
  void set_qobj_id(const std::string &value) { this->qobj_id = value; }

  const std::vector<Result> &get_results() const { return results; }
  std::vector<Result> &get_mutable_results() { return results; }
  void set_results(const std::vector<Result> &value) { this->results = value; }

  const std::string &get_status() const { return status; }
  std::string &get_mutable_status() { return status; }
  void set_status(const std::string &value) { this->status = value; }

  const bool &get_success() const { return success; }
  bool &get_mutable_success() { return success; }
  void set_success(const bool &value) { this->success = value; }
};

} // namespace ibm
} // namespace xacc

namespace nlohmann {
void from_json(const json &j, xacc::ibm::Backend &x);
void to_json(json &j, const xacc::ibm::Backend &x);

void from_json(const json &j, xacc::ibm::QObjectConfig &x);
void to_json(json &j, const xacc::ibm::QObjectConfig &x);

void from_json(const json &j, xacc::ibm::ExperimentConfig &x);
void to_json(json &j, const xacc::ibm::ExperimentConfig &x);

void from_json(const json &j, xacc::ibm::ExperimentHeader &x);
void to_json(json &j, const xacc::ibm::ExperimentHeader &x);

void from_json(const json &j, xacc::ibm::Instruction &x);
void to_json(json &j, const xacc::ibm::Instruction &x);

void from_json(const json &j, xacc::ibm::Experiment &x);
void to_json(json &j, const xacc::ibm::Experiment &x);

void from_json(const json &j, xacc::ibm::QObjectHeader &x);
void to_json(json &j, const xacc::ibm::QObjectHeader &x);

void from_json(const json &j, xacc::ibm::QObject &x);
void to_json(json &j, const xacc::ibm::QObject &x);

void from_json(const json &j, xacc::ibm::QObjectRoot &x);
void to_json(json &j, const xacc::ibm::QObjectRoot &x);

void from_json(const json &j, xacc::ibm::QregSizeEnum &x);
void to_json(json &j, const xacc::ibm::QregSizeEnum &x);
void from_json(const json &j, mpark::variant<int64_t, std::string> &x);
void to_json(json &j, const mpark::variant<int64_t, std::string> &x);

void from_json(const json &j, mpark::variant<std::string, int64_t> &x);
void to_json(json &j, const mpark::variant<std::string, int64_t> &x);

void from_json(const json &j, xacc::ibm::Data &x);
void to_json(json &j, const xacc::ibm::Data &x);

void from_json(const json &j, xacc::ibm::Result &x);
void to_json(json &j, const xacc::ibm::Result &x);

void from_json(const json &j, xacc::ibm::QObjectResult &x);
void to_json(json &j, const xacc::ibm::QObjectResult &x);

inline void from_json(const json &j, xacc::ibm::Backend &x) {
  x.set_name(j.at("name").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm::Backend &x) {
  j = json::object();
  j["name"] = x.get_name();
}

inline void from_json(const json &j, xacc::ibm::QObjectConfig &x) {
  x.set_shots(j.at("shots").get<int64_t>());
  x.set_n_qubits(j.at("n_qubits").get<int64_t>());
  x.set_memory_slots(j.at("memory_slots").get<int64_t>());
  x.set_memory(j.at("memory").get<bool>());
  x.set_parameter_binds(j.at("parameter_binds").get<std::vector<json>>());
  if (j.find("meas_lo_freq") != j.end()) {
    x.set_meas_lo_freq(j.at("meas_lo_freq").get<std::vector<json>>());
  }
  x.set_schedule_los(j.at("schedule_los").get<std::vector<json>>());
  if (j.find("qubit_lo_freq") != j.end()) {
    x.set_qubit_lo_freq(j.at("qubit_lo_freq").get<std::vector<json>>());
  }
  if (j.find("qubit_lo_range") != j.end()) {
    x.set_qubit_lo_range(j.at("qubit_lo_range").get<std::vector<json>>());
  }
  if (j.find("meas_lo_range") != j.end()) {
    x.set_meas_lo_range(j.at("meas_lo_range").get<std::vector<json>>());
  }
  x.set_meas_return(j.at("meas_return").get<std::string>());
  x.set_meas_level(j.at("meas_level").get<int64_t>());
  x.set_memory_slot_size(j.at("memory_slot_size").get<int64_t>());
}

inline void to_json(json &j, const xacc::ibm::QObjectConfig &x) {
  j = json::object();
  j["shots"] = x.get_shots();
  j["n_qubits"] = x.get_n_qubits();
  j["memory_slots"] = x.get_memory_slots();
  j["memory"] = x.get_memory();
  j["parameter_binds"] = x.get_parameter_binds();
  if (!x.get_meas_lo_freq().empty()) {
    j["meas_lo_freq"] = x.get_meas_lo_freq();
  }
  if (!x.get_schedule_los().empty()) {
    j["schedule_los"] = x.get_schedule_los();
  }
  if (!x.get_qubit_lo_freq().empty()) {
    j["qubit_lo_freq"] = x.get_qubit_lo_freq();
  }
  if (!x.get_qubit_lo_range().empty()) {
    j["qubit_lo_range"] = x.get_qubit_lo_range();
  }
  if (!x.get_meas_lo_range().empty()) {
    j["meas_lo_range"] = x.get_meas_lo_range();
  }
  j["meas_return"] = x.get_meas_return();
  j["meas_level"] = x.get_meas_level();
  j["memory_slot_size"] = x.get_memory_slot_size();
}

inline void from_json(const json &j, xacc::ibm::ExperimentConfig &x) {
  x.set_n_qubits(j.at("n_qubits").get<int64_t>());
  x.set_memory_slots(j.at("memory_slots").get<int64_t>());
}

inline void to_json(json &j, const xacc::ibm::ExperimentConfig &x) {
  j = json::object();
  j["n_qubits"] = x.get_n_qubits();
  j["memory_slots"] = x.get_memory_slots();
}

inline void from_json(const json &j, xacc::ibm::ExperimentHeader &x) {
  x.set_name(j.at("name").get<std::string>());
  x.set_qreg_sizes(
      j.at("qreg_sizes")
          .get<std::vector<std::vector<xacc::ibm::QregSizeElement>>>());
  x.set_n_qubits(j.at("n_qubits").get<int64_t>());
  x.set_qubit_labels(
      j.at("qubit_labels")
          .get<std::vector<std::vector<xacc::ibm::QregSizeElement>>>());
  x.set_memory_slots(j.at("memory_slots").get<int64_t>());
  x.set_creg_sizes(j.at("creg_sizes")
                       .get<std::vector<std::vector<xacc::ibm::ClbitLabel>>>());
  x.set_clbit_labels(
      j.at("clbit_labels")
          .get<std::vector<std::vector<xacc::ibm::ClbitLabel>>>());
}

inline void to_json(json &j, const xacc::ibm::ExperimentHeader &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["qreg_sizes"] = x.get_qreg_sizes();
  j["n_qubits"] = x.get_n_qubits();
  j["qubit_labels"] = x.get_qubit_labels();
  j["memory_slots"] = x.get_memory_slots();
  j["creg_sizes"] = x.get_creg_sizes();
  j["clbit_labels"] = x.get_clbit_labels();
}

inline void from_json(const json &j, xacc::ibm::Instruction &x) {
  x.set_qubits(j.at("qubits").get<std::vector<int64_t>>());
  x.set_name(j.at("name").get<std::string>());
  if (j.find("params") != j.end()) {
    x.set_params(j.at("params").get<std::vector<double>>());
  }
  if (j.find("memory") != j.end()) {
    x.set_memory(j.at("memory").get<std::vector<int64_t>>());
  }
}

inline void to_json(json &j, const xacc::ibm::Instruction &x) {
  j = json::object();
  if (x.isBfuc()) {
    j["name"] = "bfunc";
    j["register"] = x.get_bFunc()->registerId;
    j["mask"] = x.get_bFunc()->hex_mask;
    j["relation"] = x.get_bFunc()->relation;
    j["val"] = x.get_bFunc()->hex_val;
    return;
  }

  j["qubits"] = x.get_qubits();
  j["name"] = x.get_name();
  if (!x.get_params().empty()) {
    j["params"] = x.get_params();
  }
  if (!x.get_memory().empty()) {
    j["memory"] = x.get_memory();
    // Technically, we only use one memory slot in each Measure Op:
    if (x.get_memory().size() == 1) {
      const auto memoryId = x.get_memory()[0];
      const auto measRegisterId =
          xacc::ibm::RegisterAllocator::getInstance()->getRegisterId(memoryId);
      if (measRegisterId.has_value()) {
        std::vector<int64_t> registerIds;
        registerIds.emplace_back(measRegisterId.value());
        j["register"] = registerIds;
      }
    }
  }
  if (x.get_condition_reg_id().has_value()) {
    j["conditional"] = x.get_condition_reg_id().value();
  }
}

inline void from_json(const json &j, xacc::ibm::Experiment &x) {
  x.set_config(j.at("config").get<xacc::ibm::ExperimentConfig>());
  x.set_header(j.at("header").get<xacc::ibm::ExperimentHeader>());
  x.set_instructions(
      j.at("instructions").get<std::vector<xacc::ibm::Instruction>>());
}

inline void to_json(json &j, const xacc::ibm::Experiment &x) {
  j = json::object();
  j["config"] = x.get_config();
  j["header"] = x.get_header();
  j["instructions"] = x.get_instructions();
}
inline void from_json(const json &j, xacc::ibm::QObjectHeader &x) {
  x.set_backend_version(j.at("backend_version").get<std::string>());
  x.set_backend_name(j.at("backend_name").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm::QObjectHeader &x) {
  j = json::object();
  j["backend_version"] = x.get_backend_version();
  j["backend_name"] = x.get_backend_name();
}

inline void from_json(const json &j, xacc::ibm::QObject &x) {
  x.set_qobj_id(j.at("qobj_id").get<std::string>());
  x.set_experiments(
      j.at("experiments").get<std::vector<xacc::ibm::Experiment>>());
  x.set_config(j.at("config").get<xacc::ibm::QObjectConfig>());
  x.set_type(j.at("type").get<std::string>());
  x.set_schema_version(j.at("schema_version").get<std::string>());
  x.set_header(j.at("header").get<xacc::ibm::QObjectHeader>());
}

inline void to_json(json &j, const xacc::ibm::QObject &x) {
  j = json::object();
  j["qobj_id"] = x.get_qobj_id();
  j["experiments"] = x.get_experiments();
  j["config"] = x.get_config();
  j["type"] = x.get_type();
  j["schema_version"] = x.get_schema_version();
  j["header"] = x.get_header();
}

inline void from_json(const json &j, xacc::ibm::QObjectRoot &x) {
  x.set_q_object(j.at("qObject").get<xacc::ibm::QObject>());
  if (j.find("backend") != j.end()) {
    x.set_backend(j.at("backend").get<xacc::ibm::Backend>());
  }
  x.set_shots(j.at("shots").get<int64_t>());
}

inline void to_json(json &j, const xacc::ibm::QObjectRoot &x) {
  j = json::object();
  j["qObject"] = x.get_q_object();
  j["backend"] = x.get_backend();
  j["shots"] = x.get_shots();
}

inline void from_json(const json &j, xacc::ibm::QregSizeEnum &x) {
  if (j == "q")
    x = xacc::ibm::QregSizeEnum::Q;
  else
    throw "Input JSON does not conform to schema";
}

inline void to_json(json &j, const xacc::ibm::QregSizeEnum &x) {
  switch (x) {
  case xacc::ibm::QregSizeEnum::Q:
    j = "q";
    break;
  default:
    throw "This should not happen";
  }
}
inline void from_json(const json &j, mpark::variant<int64_t, std::string> &x) {
  if (j.is_number_integer())
    x = j.get<int64_t>();
  else if (j.is_string())
    x = j.get<std::string>();
  else
    throw "Could not deserialize";
}

inline void to_json(json &j, const mpark::variant<int64_t, std::string> &x) {
  switch (x.index()) {
  case 0:
    j = mpark::get<int64_t>(x);
    break;
  case 1:
    j = mpark::get<std::string>(x);
    break;
  default:
    throw "Input JSON does not conform to schema";
  }
}

inline void from_json(const json &j, mpark::variant<std::string, int64_t> &x) {
  if (j.is_number_integer())
    x = j.get<int64_t>();
  else if (j.is_string())
    x = j.get<std::string>();
  else
    throw "Could not deserialize";
}

inline void to_json(json &j, const mpark::variant<std::string, int64_t> &x) {
  switch (x.index()) {
  case 0:
    j = mpark::get<std::string>(x);
    break;
  case 1:
    j = mpark::get<int64_t>(x);
    break;
  default:
    throw "Input JSON does not conform to schema";
  }
}

inline void from_json(const json &j, xacc::ibm::Data &x) {
  x.set_counts(j.at("counts").get<std::map<std::string, int64_t>>());
}

inline void to_json(json &j, const xacc::ibm::Data &x) {
  j = json::object();
  j["counts"] = x.get_counts();
}

inline void from_json(const json &j, xacc::ibm::Result &x) {
  x.set_data(j.at("data").get<xacc::ibm::Data>());
  x.set_header(j.at("header").get<xacc::ibm::ExperimentHeader>());
  if (j.find("meas_level") != j.end()) {
    x.set_meas_level(j.at("meas_level").get<int64_t>());
  }
  x.set_shots(j.at("shots").get<int64_t>());
  x.set_success(j.at("success").get<bool>());
}

inline void to_json(json &j, const xacc::ibm::Result &x) {
  j = json::object();
  j["data"] = x.get_data();
  j["header"] = x.get_header();
  j["meas_level"] = x.get_meas_level();
  j["shots"] = x.get_shots();
  j["success"] = x.get_success();
}

inline void from_json(const json &j, xacc::ibm::QObjectResult &x) {
  x.set_backend_name(j.at("backend_name").get<std::string>());
  x.set_backend_version(j.at("backend_version").get<std::string>());
  x.set_date(j.at("date").get<std::string>());
  if (j.find("execution_id") != j.end()) {
    x.set_execution_id(j.at("execution_id").get<std::string>());
  }
  if (j.find("header") != j.end()) {
    x.set_header(j.at("header").get<xacc::ibm::QObjectHeader>());
  }
  x.set_job_id(j.at("job_id").get<std::string>());
  x.set_qobj_id(j.at("qobj_id").get<std::string>());
  x.set_results(j.at("results").get<std::vector<xacc::ibm::Result>>());
  x.set_status(j.at("status").get<std::string>());
  x.set_success(j.at("success").get<bool>());
}

inline void to_json(json &j, const xacc::ibm::QObjectResult &x) {
  j = json::object();
  j["backend_name"] = x.get_backend_name();
  j["backend_version"] = x.get_backend_version();
  j["date"] = x.get_date();
  j["execution_id"] = x.get_execution_id();
  j["header"] = x.get_header();
  j["job_id"] = x.get_job_id();
  j["qobj_id"] = x.get_qobj_id();
  j["results"] = x.get_results();
  j["status"] = x.get_status();
  j["success"] = x.get_success();
}
} // namespace nlohmann

#endif