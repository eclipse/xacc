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
#pragma once

#include "json.hpp"

#include <stdexcept>
#include <regex>
#include <optional>

namespace xacc {
namespace ibm_pulse {
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
inline T get_optional(const json &j, const char *property) {
  if (j.find(property) != j.end()) {
    return j.at(property).get<T>();
  }
  return T();
}

template <typename T>
inline T get_optional(const json &j, std::string property) {
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

class PulseLibrary {
public:
  PulseLibrary() = default;
  virtual ~PulseLibrary() = default;

private:
  std::vector<std::vector<double>> samples;
  std::string name;

public:
  const std::vector<std::vector<double>> &get_samples() const {
    return samples;
  }
  std::vector<std::vector<double>> &get_mutable_samples() { return samples; }
  void set_samples(const std::vector<std::vector<double>> &value) {
    this->samples = value;
  }

  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }
};

class Config {
public:
  Config() = default;
  virtual ~Config() = default;

private:
  int64_t meas_level;
  int64_t memory_slots;
  std::vector<double> meas_lo_freq;
  std::vector<PulseLibrary> pulse_library;
  std::string meas_return;
  std::vector<double> qubit_lo_freq;
  int64_t rep_time;
  std::optional<int64_t> max_credits;
  int64_t shots;
  int64_t memory_slot_size;
  bool memory;
  std::vector<std::string> parametric_pulses;

public:
  const int64_t &get_meas_level() const { return meas_level; }
  int64_t &get_mutable_meas_level() { return meas_level; }
  void set_meas_level(const int64_t &value) { this->meas_level = value; }

  const int64_t &get_memory_slots() const { return memory_slots; }
  int64_t &get_mutable_memory_slots() { return memory_slots; }
  void set_memory_slots(const int64_t &value) { this->memory_slots = value; }

  const std::vector<double> &get_meas_lo_freq() const { return meas_lo_freq; }
  std::vector<double> &get_mutable_meas_lo_freq() { return meas_lo_freq; }
  void set_meas_lo_freq(const std::vector<double> &value) {
    this->meas_lo_freq = value;
  }

  const std::vector<PulseLibrary> &get_pulse_library() const {
    return pulse_library;
  }
  std::vector<PulseLibrary> &get_mutable_pulse_library() {
    return pulse_library;
  }
  void set_pulse_library(const std::vector<PulseLibrary> &value) {
    this->pulse_library = value;
  }

  const std::string &get_meas_return() const { return meas_return; }
  std::string &get_mutable_meas_return() { return meas_return; }
  void set_meas_return(const std::string &value) { this->meas_return = value; }

  const std::vector<double> &get_qubit_lo_freq() const { return qubit_lo_freq; }
  std::vector<double> &get_mutable_qubit_lo_freq() { return qubit_lo_freq; }
  void set_qubit_lo_freq(const std::vector<double> &value) {
    this->qubit_lo_freq = value;
  }

  const int64_t &get_rep_time() const { return rep_time; }
  int64_t &get_mutable_rep_time() { return rep_time; }
  void set_rep_time(const int64_t &value) { this->rep_time = value; }

  const std::optional<int64_t> &get_max_credits() const { return max_credits; }
  std::optional<int64_t> &get_mutable_max_credits() { return max_credits; }
  void set_max_credits(const int64_t &value) { this->max_credits = value; }

  const int64_t &get_shots() const { return shots; }
  int64_t &get_mutable_shots() { return shots; }
  void set_shots(const int64_t &value) { this->shots = value; }

  const int64_t &get_memory_slot_size() const { return memory_slot_size; }
  int64_t &get_mutable_memory_slot_size() { return memory_slot_size; }
  void set_memory_slot_size(const int64_t &value) {
    this->memory_slot_size = value;
  }

  const bool &get_memory() const { return memory; }
  bool &get_mutable_memory() { return memory; }
  void set_memory(const bool &value) { this->memory = value; }

  void set_parametric_pulses(const std::vector<std::string> &pulseShapeList) {
    this->parametric_pulses = pulseShapeList;
  }
  std::vector<std::string> get_parametric_pulses() const {
    return parametric_pulses;
  }
};

class ExperimentHeader {
public:
  ExperimentHeader() = default;
  virtual ~ExperimentHeader() = default;

private:
  std::string name;
  int64_t memory_slots;

public:
  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const int64_t &get_memory_slots() const { return memory_slots; }
  int64_t &get_mutable_memory_slots() { return memory_slots; }
  void set_memory_slots(const int64_t &value) { this->memory_slots = value; }
};

class Instruction {
public:
  Instruction() = default;
  virtual ~Instruction() = default;

private:
  std::string ch;
  std::string name;
  int64_t t0;
  double phase;
  std::vector<int64_t> memory_slot;
  int64_t duration;
  std::vector<int64_t> qubits;
  std::string pulse_shape;
  json pulse_params;
public:
  std::string get_ch() const { return ch; }
  void set_ch(std::string value) { this->ch = value; }

  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const int64_t &get_t0() const { return t0; }
  int64_t &get_mutable_t0() { return t0; }
  void set_t0(const int64_t &value) { this->t0 = value; }

  double get_phase() const { return phase; }
  void set_phase(double value) { this->phase = value; }

  std::vector<int64_t> get_memory_slot() const { return memory_slot; }
  void set_memory_slot(std::vector<int64_t> value) {
    this->memory_slot = value;
  }

  int64_t get_duration() const { return duration; }
  void set_duration(int64_t value) { this->duration = value; }

  std::vector<int64_t> get_qubits() const { return qubits; }
  void set_qubits(std::vector<int64_t> value) { this->qubits = value; }

  std::string get_pulse_shape() const { return pulse_shape; }
  void set_pulse_shape(const std::string &value) { this->pulse_shape = value; }

  json get_pulse_params() const { return pulse_params; }
  void set_pulse_params(const std::string &jsonStr) {
    this->pulse_params = json::parse(jsonStr);
  }
};

class Experiment {
public:
  Experiment() = default;
  virtual ~Experiment() = default;

private:
  std::vector<Instruction> instructions;
  ExperimentHeader header;

public:
  const std::vector<Instruction> &get_instructions() const {
    return instructions;
  }
  std::vector<Instruction> &get_mutable_instructions() { return instructions; }
  void set_instructions(const std::vector<Instruction> &value) {
    this->instructions = value;
  }

  const ExperimentHeader &get_header() const { return header; }
  ExperimentHeader &get_mutable_header() { return header; }
  void set_header(const ExperimentHeader &value) { this->header = value; }
};

class QObjectHeader {
public:
  QObjectHeader() = default;
  virtual ~QObjectHeader() = default;

private:
  std::string backend_name;
  std::string backend_version;

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
};

class QObject {
public:
  QObject() = default;
  virtual ~QObject() = default;

private:
  std::string type;
  std::string schema_version;
  std::string qobj_id;
  QObjectHeader header;
  std::vector<Experiment> experiments;
  Config config;

public:
  const std::string &get_type() const { return type; }
  std::string &get_mutable_type() { return type; }
  void set_type(const std::string &value) { this->type = value; }

  const std::string &get_schema_version() const { return schema_version; }
  std::string &get_mutable_schema_version() { return schema_version; }
  void set_schema_version(const std::string &value) {
    this->schema_version = value;
  }

  const std::string &get_qobj_id() const { return qobj_id; }
  std::string &get_mutable_qobj_id() { return qobj_id; }
  void set_qobj_id(const std::string &value) { this->qobj_id = value; }

  const QObjectHeader &get_header() const { return header; }
  QObjectHeader &get_mutable_header() { return header; }
  void set_header(const QObjectHeader &value) { this->header = value; }

  const std::vector<Experiment> &get_experiments() const { return experiments; }
  std::vector<Experiment> &get_mutable_experiments() { return experiments; }
  void set_experiments(const std::vector<Experiment> &value) {
    this->experiments = value;
  }

  const Config &get_config() const { return config; }
  Config &get_mutable_config() { return config; }
  void set_config(const Config &value) { this->config = value; }
};

class PulseQObject {
public:
  PulseQObject() = default;
  virtual ~PulseQObject() = default;

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
} // namespace ibm_pulse
} // namespace xacc

namespace nlohmann {
void from_json(const json &j, xacc::ibm_pulse::Backend &x);
void to_json(json &j, const xacc::ibm_pulse::Backend &x);

void from_json(const json &j, xacc::ibm_pulse::PulseLibrary &x);
void to_json(json &j, const xacc::ibm_pulse::PulseLibrary &x);

void from_json(const json &j, xacc::ibm_pulse::Config &x);
void to_json(json &j, const xacc::ibm_pulse::Config &x);

void from_json(const json &j, xacc::ibm_pulse::ExperimentHeader &x);
void to_json(json &j, const xacc::ibm_pulse::ExperimentHeader &x);

void from_json(const json &j, xacc::ibm_pulse::Instruction &x);
void to_json(json &j, const xacc::ibm_pulse::Instruction &x);

void from_json(const json &j, xacc::ibm_pulse::Experiment &x);
void to_json(json &j, const xacc::ibm_pulse::Experiment &x);

void from_json(const json &j, xacc::ibm_pulse::QObjectHeader &x);
void to_json(json &j, const xacc::ibm_pulse::QObjectHeader &x);

void from_json(const json &j, xacc::ibm_pulse::QObject &x);
void to_json(json &j, const xacc::ibm_pulse::QObject &x);

void from_json(const json &j, xacc::ibm_pulse::PulseQObject &x);
void to_json(json &j, const xacc::ibm_pulse::PulseQObject &x);

inline void from_json(const json &j, xacc::ibm_pulse::Backend &x) {
  x.set_name(j.at("name").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_pulse::Backend &x) {
  j = json::object();
  j["name"] = x.get_name();
}

inline void from_json(const json &j, xacc::ibm_pulse::PulseLibrary &x) {
  x.set_samples(j.at("samples").get<std::vector<std::vector<double>>>());
  x.set_name(j.at("name").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_pulse::PulseLibrary &x) {
  j = json::object();
  j["samples"] = x.get_samples();
  j["name"] = x.get_name();
}

inline void from_json(const json &j, xacc::ibm_pulse::Config &x) {
  x.set_meas_level(j.at("meas_level").get<int64_t>());
  x.set_memory_slots(j.at("memory_slots").get<int64_t>());
  x.set_meas_lo_freq(j.at("meas_lo_freq").get<std::vector<double>>());
  x.set_pulse_library(
      j.at("pulse_library").get<std::vector<xacc::ibm_pulse::PulseLibrary>>());
  x.set_meas_return(j.at("meas_return").get<std::string>());
  x.set_qubit_lo_freq(j.at("qubit_lo_freq").get<std::vector<double>>());
  x.set_rep_time(j.at("rep_time").get<int64_t>());
  x.set_max_credits(j.at("max_credits").get<int64_t>());
  x.set_shots(j.at("shots").get<int64_t>());
  x.set_memory_slot_size(j.at("memory_slot_size").get<int64_t>());
  x.set_memory(j.at("memory").get<bool>());
}

inline void to_json(json &j, const xacc::ibm_pulse::Config &x) {
  j = json::object();
  j["meas_level"] = x.get_meas_level();
  j["memory_slots"] = x.get_memory_slots();
  j["meas_lo_freq"] = x.get_meas_lo_freq();
  j["pulse_library"] = x.get_pulse_library();
  j["meas_return"] = x.get_meas_return();
  j["qubit_lo_freq"] = x.get_qubit_lo_freq();
  j["rep_time"] = x.get_rep_time();
  if (x.get_max_credits().has_value()) {
    j["max_credits"] = x.get_max_credits().value();
  }
  j["shots"] = x.get_shots();
  j["memory_slot_size"] = x.get_memory_slot_size();
  j["memory"] = x.get_memory();
  j["parametric_pulses"] = x.get_parametric_pulses();
}

inline void from_json(const json &j, xacc::ibm_pulse::ExperimentHeader &x) {
  x.set_name(j.at("name").get<std::string>());
  x.set_memory_slots(j.at("memory_slots").get<int64_t>());
}

inline void to_json(json &j, const xacc::ibm_pulse::ExperimentHeader &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["memory_slots"] = x.get_memory_slots();
}

inline void from_json(const json &j, xacc::ibm_pulse::Instruction &x) {
  x.set_ch(xacc::ibm_pulse::get_optional<std::string>(j, "ch"));
  x.set_name(j.at("name").get<std::string>());
  x.set_t0(j.at("t0").get<int64_t>());
  x.set_phase(xacc::ibm_pulse::get_optional<double>(j, "phase"));
  x.set_memory_slot(
      xacc::ibm_pulse::get_optional<std::vector<int64_t>>(j, "memory_slot"));
  x.set_duration(xacc::ibm_pulse::get_optional<int64_t>(j, "duration"));
  x.set_qubits(
      xacc::ibm_pulse::get_optional<std::vector<int64_t>>(j, "qubits"));
}

inline void to_json(json &j, const xacc::ibm_pulse::Instruction &x) {
  j = json::object();

  j["name"] = x.get_name();
  j["t0"] = x.get_t0();
  if (x.get_name() == "fc") {
    j["phase"] = x.get_phase();
  }
  if (x.get_name() == "acquire") {
    j["duration"] = x.get_duration();
    j["memory_slot"] = x.get_memory_slot();
    j["qubits"] = x.get_qubits();
  } else {
    j["ch"] = x.get_ch();
  }
  if (x.get_name() == "parametric_pulse") {
    j["pulse_shape"] = x.get_pulse_shape();
    j["parameters"] = x.get_pulse_params();
  }

  if (x.get_name() == "delay") {
    j["duration"] = x.get_duration();
  }
}

inline void from_json(const json &j, xacc::ibm_pulse::Experiment &x) {
  x.set_instructions(
      j.at("instructions").get<std::vector<xacc::ibm_pulse::Instruction>>());
  x.set_header(j.at("header").get<xacc::ibm_pulse::ExperimentHeader>());
}

inline void to_json(json &j, const xacc::ibm_pulse::Experiment &x) {
  j = json::object();
  j["instructions"] = x.get_instructions();
  j["header"] = x.get_header();
}

inline void from_json(const json &j, xacc::ibm_pulse::QObjectHeader &x) {
  x.set_backend_name(j.at("backend_name").get<std::string>());
  x.set_backend_version(j.at("backend_version").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_pulse::QObjectHeader &x) {
  j = json::object();
  j["backend_name"] = x.get_backend_name();
  j["backend_version"] = x.get_backend_version();
}

inline void from_json(const json &j, xacc::ibm_pulse::QObject &x) {
  x.set_type(j.at("type").get<std::string>());
  x.set_schema_version(j.at("schema_version").get<std::string>());
  x.set_qobj_id(j.at("qobj_id").get<std::string>());
  x.set_header(j.at("header").get<xacc::ibm_pulse::QObjectHeader>());
  x.set_experiments(
      j.at("experiments").get<std::vector<xacc::ibm_pulse::Experiment>>());
  x.set_config(j.at("config").get<xacc::ibm_pulse::Config>());
}

inline void to_json(json &j, const xacc::ibm_pulse::QObject &x) {
  j = json::object();
  j["type"] = x.get_type();
  j["schema_version"] = x.get_schema_version();
  j["qobj_id"] = x.get_qobj_id();
  j["header"] = x.get_header();
  j["experiments"] = x.get_experiments();
  j["config"] = x.get_config();
}

inline void from_json(const json &j, xacc::ibm_pulse::PulseQObject &x) {
  x.set_q_object(j.at("qObject").get<xacc::ibm_pulse::QObject>());
  x.set_backend(j.at("backend").get<xacc::ibm_pulse::Backend>());
  x.set_shots(j.at("shots").get<int64_t>());
}

inline void to_json(json &j, const xacc::ibm_pulse::PulseQObject &x) {
  j = json::object();
  j["qObject"] = x.get_q_object();
  j["backend"] = x.get_backend();
  j["shots"] = x.get_shots();
}
} // namespace nlohmann
