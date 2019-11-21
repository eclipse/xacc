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

#include "variant.hpp"
#include "json.hpp"

// #include <boost/optional.hpp>
#include <stdexcept>
#include <regex>

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
namespace ibm_backend {
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

enum class Name : int { CX, ID, MEAS, MEASURE, U1, U2, U3, X };

enum class PhaseEnum : int { P0, P1, P2, THE_1_P0, THE_1_P1, THE_1_P2 };

using PhaseUnion = std::shared_ptr<mpark::variant<double, PhaseEnum>>;

class Sequence {
public:
  Sequence() = default;
  virtual ~Sequence() = default;

private:
  std::shared_ptr<std::string> ch;
  std::string name;
  PhaseUnion phase;
  int64_t t0;
  std::shared_ptr<int64_t> duration;
  std::shared_ptr<std::vector<int64_t>> memory_slot;
  std::shared_ptr<std::vector<int64_t>> qubits;

public:
  std::shared_ptr<std::string> get_ch() const { return ch; }
  void set_ch(std::shared_ptr<std::string> value) { this->ch = value; }

  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  PhaseUnion get_phase() const { return phase; }
  void set_phase(PhaseUnion value) { this->phase = value; }

  const int64_t &get_t0() const { return t0; }
  int64_t &get_mutable_t0() { return t0; }
  void set_t0(const int64_t &value) { this->t0 = value; }

  std::shared_ptr<int64_t> get_duration() const { return duration; }
  void set_duration(std::shared_ptr<int64_t> value) { this->duration = value; }

  std::shared_ptr<std::vector<int64_t>> get_memory_slot() const {
    return memory_slot;
  }
  void set_memory_slot(std::shared_ptr<std::vector<int64_t>> value) {
    this->memory_slot = value;
  }

  std::shared_ptr<std::vector<int64_t>> get_qubits() const { return qubits; }
  void set_qubits(std::shared_ptr<std::vector<int64_t>> value) {
    this->qubits = value;
  }
};

class CmdDef {
public:
  CmdDef() = default;
  virtual ~CmdDef() = default;

private:
  Name name;
  std::vector<int64_t> qubits;
  std::vector<Sequence> sequence;

public:
  const Name &get_name() const { return name; }
  Name &get_mutable_name() { return name; }
  void set_name(const Name &value) { this->name = value; }

  const std::vector<int64_t> &get_qubits() const { return qubits; }
  std::vector<int64_t> &get_mutable_qubits() { return qubits; }
  void set_qubits(const std::vector<int64_t> &value) { this->qubits = value; }

  const std::vector<Sequence> &get_sequence() const { return sequence; }
  std::vector<Sequence> &get_mutable_sequence() { return sequence; }
  void set_sequence(const std::vector<Sequence> &value) {
    this->sequence = value;
  }
};

class Neighborhood {
public:
  Neighborhood() = default;
  virtual ~Neighborhood() = default;

private:
  int64_t channels;
  int64_t qubits;

public:
  const int64_t &get_channels() const { return channels; }
  int64_t &get_mutable_channels() { return channels; }
  void set_channels(const int64_t &value) { this->channels = value; }

  const int64_t &get_qubits() const { return qubits; }
  int64_t &get_mutable_qubits() { return qubits; }
  void set_qubits(const int64_t &value) { this->qubits = value; }
};

class DiscriminatorParams {
public:
  DiscriminatorParams() = default;
  virtual ~DiscriminatorParams() = default;

private:
  std::vector<Neighborhood> neighborhoods;
  bool resample;

public:
  const std::vector<Neighborhood> &get_neighborhoods() const {
    return neighborhoods;
  }
  std::vector<Neighborhood> &get_mutable_neighborhoods() {
    return neighborhoods;
  }
  void set_neighborhoods(const std::vector<Neighborhood> &value) {
    this->neighborhoods = value;
  }

  const bool &get_resample() const { return resample; }
  bool &get_mutable_resample() { return resample; }
  void set_resample(const bool &value) { this->resample = value; }
};

class Discriminator {
public:
  Discriminator() = default;
  virtual ~Discriminator() = default;

private:
  std::string name;
  DiscriminatorParams params;

public:
  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const DiscriminatorParams &get_params() const { return params; }
  DiscriminatorParams &get_mutable_params() { return params; }
  void set_params(const DiscriminatorParams &value) { this->params = value; }
};

class MeasKernelParams {
public:
  MeasKernelParams() = default;
  virtual ~MeasKernelParams() = default;

private:
public:
};

class MeasKernel {
public:
  MeasKernel() = default;
  virtual ~MeasKernel() = default;

private:
  std::string name;
  MeasKernelParams params;

public:
  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const MeasKernelParams &get_params() const { return params; }
  MeasKernelParams &get_mutable_params() { return params; }
  void set_params(const MeasKernelParams &value) { this->params = value; }
};

class PulseLibrary {
public:
  PulseLibrary() = default;
  virtual ~PulseLibrary() = default;

private:
  std::string name;
  std::vector<std::vector<double>> samples;

public:
  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const std::vector<std::vector<double>> &get_samples() const {
    return samples;
  }
  std::vector<std::vector<double>> &get_mutable_samples() { return samples; }
  void set_samples(const std::vector<std::vector<double>> &value) {
    this->samples = value;
  }
};

class Defaults {
public:
  Defaults() = default;
  virtual ~Defaults() = default;

private:
  int64_t buffer;
  std::vector<CmdDef> cmd_def;
  Discriminator discriminator;
  std::vector<double> meas_freq_est;
  MeasKernel meas_kernel;
  std::vector<PulseLibrary> pulse_library;
  std::vector<double> qubit_freq_est;

public:
  const int64_t &get_buffer() const { return buffer; }
  int64_t &get_mutable_buffer() { return buffer; }
  void set_buffer(const int64_t &value) { this->buffer = value; }

  const std::vector<CmdDef> &get_cmd_def() const { return cmd_def; }
  std::vector<CmdDef> &get_mutable_cmd_def() { return cmd_def; }
  void set_cmd_def(const std::vector<CmdDef> &value) { this->cmd_def = value; }

  const Discriminator &get_discriminator() const { return discriminator; }
  Discriminator &get_mutable_discriminator() { return discriminator; }
  void set_discriminator(const Discriminator &value) {
    this->discriminator = value;
  }

  const std::vector<double> &get_meas_freq_est() const { return meas_freq_est; }
  std::vector<double> &get_mutable_meas_freq_est() { return meas_freq_est; }
  void set_meas_freq_est(const std::vector<double> &value) {
    this->meas_freq_est = value;
  }

  const MeasKernel &get_meas_kernel() const { return meas_kernel; }
  MeasKernel &get_mutable_meas_kernel() { return meas_kernel; }
  void set_meas_kernel(const MeasKernel &value) { this->meas_kernel = value; }

  const std::vector<PulseLibrary> &get_pulse_library() const {
    return pulse_library;
  }
  std::vector<PulseLibrary> &get_mutable_pulse_library() {
    return pulse_library;
  }
  void set_pulse_library(const std::vector<PulseLibrary> &value) {
    this->pulse_library = value;
  }

  const std::vector<double> &get_qubit_freq_est() const {
    return qubit_freq_est;
  }
  std::vector<double> &get_mutable_qubit_freq_est() { return qubit_freq_est; }
  void set_qubit_freq_est(const std::vector<double> &value) {
    this->qubit_freq_est = value;
  }
};

enum class Parameter : int { LAMBDA, PHI, THETA };

class Gate {
public:
  Gate() = default;
  virtual ~Gate() = default;

private:
  Name name;
  std::vector<Parameter> parameters;
  std::string qasm_def;
  std::shared_ptr<std::vector<std::vector<int64_t>>> coupling_map;

public:
  const Name &get_name() const { return name; }
  Name &get_mutable_name() { return name; }
  void set_name(const Name &value) { this->name = value; }

  const std::vector<Parameter> &get_parameters() const { return parameters; }
  std::vector<Parameter> &get_mutable_parameters() { return parameters; }
  void set_parameters(const std::vector<Parameter> &value) {
    this->parameters = value;
  }

  const std::string &get_qasm_def() const { return qasm_def; }
  std::string &get_mutable_qasm_def() { return qasm_def; }
  void set_qasm_def(const std::string &value) { this->qasm_def = value; }

  std::shared_ptr<std::vector<std::vector<int64_t>>> get_coupling_map() const {
    return coupling_map;
  }
  void
  set_coupling_map(std::shared_ptr<std::vector<std::vector<int64_t>>> value) {
    this->coupling_map = value;
  }
};

class Hamiltonian {
public:
  Hamiltonian() = default;
  virtual ~Hamiltonian() = default;

private:
  std::string description;
  std::string h_latex;
  std::vector<std::string> h_str;
  std::map<std::string, int64_t> osc;
  std::map<std::string, double> vars;
  std::shared_ptr<std::map<std::string, int64_t>> qub;

public:
  const std::string &get_description() const { return description; }
  std::string &get_mutable_description() { return description; }
  void set_description(const std::string &value) { this->description = value; }

  const std::string &get_h_latex() const { return h_latex; }
  std::string &get_mutable_h_latex() { return h_latex; }
  void set_h_latex(const std::string &value) { this->h_latex = value; }

  const std::vector<std::string> &get_h_str() const { return h_str; }
  std::vector<std::string> &get_mutable_h_str() { return h_str; }
  void set_h_str(const std::vector<std::string> &value) { this->h_str = value; }

  const std::map<std::string, int64_t> &get_osc() const { return osc; }
  std::map<std::string, int64_t> &get_mutable_osc() { return osc; }
  void set_osc(const std::map<std::string, int64_t> &value) {
    this->osc = value;
  }

  const std::map<std::string, double> &get_vars() const { return vars; }
  std::map<std::string, double> &get_mutable_vars() { return vars; }
  void set_vars(const std::map<std::string, double> &value) {
    this->vars = value;
  }

  std::shared_ptr<std::map<std::string, int64_t>> get_qub() const {
    return qub;
  }
  void set_qub(std::shared_ptr<std::map<std::string, int64_t>> value) {
    this->qub = value;
  }
};

class UChannelLo {
public:
  UChannelLo() = default;
  virtual ~UChannelLo() = default;

private:
  int64_t q;
  std::vector<int64_t> scale;

public:
  const int64_t &get_q() const { return q; }
  int64_t &get_mutable_q() { return q; }
  void set_q(const int64_t &value) { this->q = value; }

  const std::vector<int64_t> &get_scale() const { return scale; }
  std::vector<int64_t> &get_mutable_scale() { return scale; }
  void set_scale(const std::vector<int64_t> &value) { this->scale = value; }
};

class SpecificConfiguration {
public:
  SpecificConfiguration() = default;
  virtual ~SpecificConfiguration() = default;

private:
  std::string backend_name;
  bool local;
  std::shared_ptr<std::vector<std::vector<int64_t>>> coupling_map;
  bool conditional;
  bool open_pulse;
  bool allow_q_object;
  int64_t max_experiments;
  std::string backend_version;
  int64_t n_qubits;
  std::vector<std::string> basis_gates;
  std::vector<Gate> gates;
  bool simulator;
  int64_t max_shots;
  bool memory;
  std::shared_ptr<std::vector<nlohmann::json>> acquisition_latency;
  std::shared_ptr<std::vector<nlohmann::json>> conditional_latency;
  std::shared_ptr<bool> credits_required;
  std::shared_ptr<Defaults> defaults;
  std::shared_ptr<std::string> description;
  std::shared_ptr<std::vector<std::string>> discriminators;
  std::shared_ptr<double> dt;
  std::shared_ptr<double> dtm;
  std::shared_ptr<Hamiltonian> hamiltonian;
  std::shared_ptr<std::vector<std::string>> meas_kernels;
  std::shared_ptr<std::vector<int64_t>> meas_levels;
  std::shared_ptr<std::vector<std::vector<double>>> meas_lo_range;
  std::shared_ptr<std::vector<std::vector<int64_t>>> meas_map;
  std::shared_ptr<int64_t> n_registers;
  std::shared_ptr<int64_t> n_uchannels;
  std::shared_ptr<std::string> online_date;
  std::shared_ptr<std::vector<std::vector<double>>> qubit_lo_range;
  std::shared_ptr<std::vector<int64_t>> rep_times;
  std::shared_ptr<std::string> sample_name;
  std::shared_ptr<std::vector<std::vector<UChannelLo>>> u_channel_lo;
  std::shared_ptr<std::string> url;

public:
  const std::string &get_backend_name() const { return backend_name; }
  std::string &get_mutable_backend_name() { return backend_name; }
  void set_backend_name(const std::string &value) {
    this->backend_name = value;
  }

  const bool &get_local() const { return local; }
  bool &get_mutable_local() { return local; }
  void set_local(const bool &value) { this->local = value; }

  std::shared_ptr<std::vector<std::vector<int64_t>>> get_coupling_map() const {
    return coupling_map;
  }
  void
  set_coupling_map(std::shared_ptr<std::vector<std::vector<int64_t>>> value) {
    this->coupling_map = value;
  }

  const bool &get_conditional() const { return conditional; }
  bool &get_mutable_conditional() { return conditional; }
  void set_conditional(const bool &value) { this->conditional = value; }

  const bool &get_open_pulse() const { return open_pulse; }
  bool &get_mutable_open_pulse() { return open_pulse; }
  void set_open_pulse(const bool &value) { this->open_pulse = value; }

  const bool &get_allow_q_object() const { return allow_q_object; }
  bool &get_mutable_allow_q_object() { return allow_q_object; }
  void set_allow_q_object(const bool &value) { this->allow_q_object = value; }

  const int64_t &get_max_experiments() const { return max_experiments; }
  int64_t &get_mutable_max_experiments() { return max_experiments; }
  void set_max_experiments(const int64_t &value) {
    this->max_experiments = value;
  }

  const std::string &get_backend_version() const { return backend_version; }
  std::string &get_mutable_backend_version() { return backend_version; }
  void set_backend_version(const std::string &value) {
    this->backend_version = value;
  }

  const int64_t &get_n_qubits() const { return n_qubits; }
  int64_t &get_mutable_n_qubits() { return n_qubits; }
  void set_n_qubits(const int64_t &value) { this->n_qubits = value; }

  const std::vector<std::string> &get_basis_gates() const {
    return basis_gates;
  }
  std::vector<std::string> &get_mutable_basis_gates() { return basis_gates; }
  void set_basis_gates(const std::vector<std::string> &value) {
    this->basis_gates = value;
  }

  const std::vector<Gate> &get_gates() const { return gates; }
  std::vector<Gate> &get_mutable_gates() { return gates; }
  void set_gates(const std::vector<Gate> &value) { this->gates = value; }

  const bool &get_simulator() const { return simulator; }
  bool &get_mutable_simulator() { return simulator; }
  void set_simulator(const bool &value) { this->simulator = value; }

  const int64_t &get_max_shots() const { return max_shots; }
  int64_t &get_mutable_max_shots() { return max_shots; }
  void set_max_shots(const int64_t &value) { this->max_shots = value; }

  const bool &get_memory() const { return memory; }
  bool &get_mutable_memory() { return memory; }
  void set_memory(const bool &value) { this->memory = value; }

  std::shared_ptr<std::vector<nlohmann::json>> get_acquisition_latency() const {
    return acquisition_latency;
  }
  void
  set_acquisition_latency(std::shared_ptr<std::vector<nlohmann::json>> value) {
    this->acquisition_latency = value;
  }

  std::shared_ptr<std::vector<nlohmann::json>> get_conditional_latency() const {
    return conditional_latency;
  }
  void
  set_conditional_latency(std::shared_ptr<std::vector<nlohmann::json>> value) {
    this->conditional_latency = value;
  }

  std::shared_ptr<bool> get_credits_required() const {
    return credits_required;
  }
  void set_credits_required(std::shared_ptr<bool> value) {
    this->credits_required = value;
  }

  std::shared_ptr<Defaults> get_defaults() const { return defaults; }
  void set_defaults(std::shared_ptr<Defaults> value) { this->defaults = value; }

  std::shared_ptr<std::string> get_description() const { return description; }
  void set_description(std::shared_ptr<std::string> value) {
    this->description = value;
  }

  std::shared_ptr<std::vector<std::string>> get_discriminators() const {
    return discriminators;
  }
  void set_discriminators(std::shared_ptr<std::vector<std::string>> value) {
    this->discriminators = value;
  }

  std::shared_ptr<double> get_dt() const { return dt; }
  void set_dt(std::shared_ptr<double> value) { this->dt = value; }

  std::shared_ptr<double> get_dtm() const { return dtm; }
  void set_dtm(std::shared_ptr<double> value) { this->dtm = value; }

  std::shared_ptr<Hamiltonian> get_hamiltonian() const { return hamiltonian; }
  void set_hamiltonian(std::shared_ptr<Hamiltonian> value) {
    this->hamiltonian = value;
  }

  std::shared_ptr<std::vector<std::string>> get_meas_kernels() const {
    return meas_kernels;
  }
  void set_meas_kernels(std::shared_ptr<std::vector<std::string>> value) {
    this->meas_kernels = value;
  }

  std::shared_ptr<std::vector<int64_t>> get_meas_levels() const {
    return meas_levels;
  }
  void set_meas_levels(std::shared_ptr<std::vector<int64_t>> value) {
    this->meas_levels = value;
  }

  std::shared_ptr<std::vector<std::vector<double>>> get_meas_lo_range() const {
    return meas_lo_range;
  }
  void
  set_meas_lo_range(std::shared_ptr<std::vector<std::vector<double>>> value) {
    this->meas_lo_range = value;
  }

  std::shared_ptr<std::vector<std::vector<int64_t>>> get_meas_map() const {
    return meas_map;
  }
  void set_meas_map(std::shared_ptr<std::vector<std::vector<int64_t>>> value) {
    this->meas_map = value;
  }

  std::shared_ptr<int64_t> get_n_registers() const { return n_registers; }
  void set_n_registers(std::shared_ptr<int64_t> value) {
    this->n_registers = value;
  }

  std::shared_ptr<int64_t> get_n_uchannels() const { return n_uchannels; }
  void set_n_uchannels(std::shared_ptr<int64_t> value) {
    this->n_uchannels = value;
  }

  std::shared_ptr<std::string> get_online_date() const { return online_date; }
  void set_online_date(std::shared_ptr<std::string> value) {
    this->online_date = value;
  }

  std::shared_ptr<std::vector<std::vector<double>>> get_qubit_lo_range() const {
    return qubit_lo_range;
  }
  void
  set_qubit_lo_range(std::shared_ptr<std::vector<std::vector<double>>> value) {
    this->qubit_lo_range = value;
  }

  std::shared_ptr<std::vector<int64_t>> get_rep_times() const {
    return rep_times;
  }
  void set_rep_times(std::shared_ptr<std::vector<int64_t>> value) {
    this->rep_times = value;
  }

  std::shared_ptr<std::string> get_sample_name() const { return sample_name; }
  void set_sample_name(std::shared_ptr<std::string> value) {
    this->sample_name = value;
  }

  std::shared_ptr<std::vector<std::vector<UChannelLo>>>
  get_u_channel_lo() const {
    return u_channel_lo;
  }
  void set_u_channel_lo(
      std::shared_ptr<std::vector<std::vector<UChannelLo>>> value) {
    this->u_channel_lo = value;
  }

  std::shared_ptr<std::string> get_url() const { return url; }
  void set_url(std::shared_ptr<std::string> value) { this->url = value; }
};

enum class Unit : int { G_HZ, M_K, NS, S };

class Temperature {
public:
  Temperature() = default;
  virtual ~Temperature() = default;

private:
  std::string date;
  Unit unit;
  double value;

public:
  const std::string &get_date() const { return date; }
  std::string &get_mutable_date() { return date; }
  void set_date(const std::string &value) { this->date = value; }

  const Unit &get_unit() const { return unit; }
  Unit &get_mutable_unit() { return unit; }
  void set_unit(const Unit &value) { this->unit = value; }

  const double &get_value() const { return value; }
  double &get_mutable_value() { return value; }
  void set_value(const double &value) { this->value = value; }
};

class FridgeParameters {
public:
  FridgeParameters() = default;
  virtual ~FridgeParameters() = default;

private:
  Temperature temperature;
  std::string cooldown_date;

public:
  const Temperature &get_temperature() const { return temperature; }
  Temperature &get_mutable_temperature() { return temperature; }
  void set_temperature(const Temperature &value) { this->temperature = value; }

  const std::string &get_cooldown_date() const { return cooldown_date; }
  std::string &get_mutable_cooldown_date() { return cooldown_date; }
  void set_cooldown_date(const std::string &value) {
    this->cooldown_date = value;
  }
};

class Error {
public:
  Error() = default;
  virtual ~Error() = default;

private:
  std::string date;
  double value;

public:
  const std::string &get_date() const { return date; }
  std::string &get_mutable_date() { return date; }
  void set_date(const std::string &value) { this->date = value; }

  const double &get_value() const { return value; }
  double &get_mutable_value() { return value; }
  void set_value(const double &value) { this->value = value; }
};

class MultiQubitGate {
public:
  MultiQubitGate() = default;
  virtual ~MultiQubitGate() = default;

private:
  Error gate_error;
  std::string name;
  std::vector<int64_t> qubits;
  Name type;

public:
  const Error &get_gate_error() const { return gate_error; }
  Error &get_mutable_gate_error() { return gate_error; }
  void set_gate_error(const Error &value) { this->gate_error = value; }

  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const std::vector<int64_t> &get_qubits() const { return qubits; }
  std::vector<int64_t> &get_mutable_qubits() { return qubits; }
  void set_qubits(const std::vector<int64_t> &value) { this->qubits = value; }

  const Name &get_type() const { return type; }
  Name &get_mutable_type() { return type; }
  void set_type(const Name &value) { this->type = value; }
};

class Qubit {
public:
  Qubit() = default;
  virtual ~Qubit() = default;

private:
  Temperature t1;
  Temperature t2;
  Temperature buffer;
  Temperature frequency;
  Temperature gate_time;
  std::string name;
  Error gate_error;
  Error readout_error;

public:
  const Temperature &get_t1() const { return t1; }
  Temperature &get_mutable_t1() { return t1; }
  void set_t1(const Temperature &value) { this->t1 = value; }

  const Temperature &get_t2() const { return t2; }
  Temperature &get_mutable_t2() { return t2; }
  void set_t2(const Temperature &value) { this->t2 = value; }

  const Temperature &get_buffer() const { return buffer; }
  Temperature &get_mutable_buffer() { return buffer; }
  void set_buffer(const Temperature &value) { this->buffer = value; }

  const Temperature &get_frequency() const { return frequency; }
  Temperature &get_mutable_frequency() { return frequency; }
  void set_frequency(const Temperature &value) { this->frequency = value; }

  const Temperature &get_gate_time() const { return gate_time; }
  Temperature &get_mutable_gate_time() { return gate_time; }
  void set_gate_time(const Temperature &value) { this->gate_time = value; }

  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const Error &get_gate_error() const { return gate_error; }
  Error &get_mutable_gate_error() { return gate_error; }
  void set_gate_error(const Error &value) { this->gate_error = value; }

  const Error &get_readout_error() const { return readout_error; }
  Error &get_mutable_readout_error() { return readout_error; }
  void set_readout_error(const Error &value) { this->readout_error = value; }
};

class Stats {
public:
  Stats() = default;
  virtual ~Stats() = default;

private:
  std::string last_update_date;
  FridgeParameters fridge_parameters;
  std::vector<Qubit> qubits;
  std::vector<MultiQubitGate> multi_qubit_gates;
  std::string url;

public:
  const std::string &get_last_update_date() const { return last_update_date; }
  std::string &get_mutable_last_update_date() { return last_update_date; }
  void set_last_update_date(const std::string &value) {
    this->last_update_date = value;
  }

  const FridgeParameters &get_fridge_parameters() const {
    return fridge_parameters;
  }
  FridgeParameters &get_mutable_fridge_parameters() {
    return fridge_parameters;
  }
  void set_fridge_parameters(const FridgeParameters &value) {
    this->fridge_parameters = value;
  }

  const std::vector<Qubit> &get_qubits() const { return qubits; }
  std::vector<Qubit> &get_mutable_qubits() { return qubits; }
  void set_qubits(const std::vector<Qubit> &value) { this->qubits = value; }

  const std::vector<MultiQubitGate> &get_multi_qubit_gates() const {
    return multi_qubit_gates;
  }
  std::vector<MultiQubitGate> &get_mutable_multi_qubit_gates() {
    return multi_qubit_gates;
  }
  void set_multi_qubit_gates(const std::vector<MultiQubitGate> &value) {
    this->multi_qubit_gates = value;
  }

  const std::string &get_url() const { return url; }
  std::string &get_mutable_url() { return url; }
  void set_url(const std::string &value) { this->url = value; }
};

class Configuration {
public:
  Configuration() = default;
  virtual ~Configuration() = default;

private:
  std::shared_ptr<std::string> host;
  std::shared_ptr<std::string> port;
  std::shared_ptr<std::string> password;
  nlohmann::json db_number;
  std::shared_ptr<std::string> url;
  std::shared_ptr<std::string> api_token;
  std::string kind;

public:
  std::shared_ptr<std::string> get_host() const { return host; }
  void set_host(std::shared_ptr<std::string> value) { this->host = value; }

  std::shared_ptr<std::string> get_port() const { return port; }
  void set_port(std::shared_ptr<std::string> value) { this->port = value; }

  std::shared_ptr<std::string> get_password() const { return password; }
  void set_password(std::shared_ptr<std::string> value) {
    this->password = value;
  }

  const nlohmann::json &get_db_number() const { return db_number; }
  nlohmann::json &get_mutable_db_number() { return db_number; }
  void set_db_number(const nlohmann::json &value) { this->db_number = value; }

  std::shared_ptr<std::string> get_url() const { return url; }
  void set_url(std::shared_ptr<std::string> value) { this->url = value; }

  std::shared_ptr<std::string> get_api_token() const { return api_token; }
  void set_api_token(std::shared_ptr<std::string> value) {
    this->api_token = value;
  }

  const std::string &get_kind() const { return kind; }
  std::string &get_mutable_kind() { return kind; }
  void set_kind(const std::string &value) { this->kind = value; }
};

class Queues {
public:
  Queues() = default;
  virtual ~Queues() = default;

private:
  std::string tasks;
  std::string results;
  std::shared_ptr<std::string> management;
  nlohmann::json has_admin_queue;
  nlohmann::json has_priority_queue;

public:
  const std::string &get_tasks() const { return tasks; }
  std::string &get_mutable_tasks() { return tasks; }
  void set_tasks(const std::string &value) { this->tasks = value; }

  const std::string &get_results() const { return results; }
  std::string &get_mutable_results() { return results; }
  void set_results(const std::string &value) { this->results = value; }

  std::shared_ptr<std::string> get_management() const { return management; }
  void set_management(std::shared_ptr<std::string> value) {
    this->management = value;
  }

  const nlohmann::json &get_has_admin_queue() const { return has_admin_queue; }
  nlohmann::json &get_mutable_has_admin_queue() { return has_admin_queue; }
  void set_has_admin_queue(const nlohmann::json &value) {
    this->has_admin_queue = value;
  }

  const nlohmann::json &get_has_priority_queue() const {
    return has_priority_queue;
  }
  nlohmann::json &get_mutable_has_priority_queue() {
    return has_priority_queue;
  }
  void set_has_priority_queue(const nlohmann::json &value) {
    this->has_priority_queue = value;
  }
};

class Status {
public:
  Status() = default;
  virtual ~Status() = default;

private:
  std::string device;
  std::string queue;

public:
  const std::string &get_device() const { return device; }
  std::string &get_mutable_device() { return device; }
  void set_device(const std::string &value) { this->device = value; }

  const std::string &get_queue() const { return queue; }
  std::string &get_mutable_queue() { return queue; }
  void set_queue(const std::string &value) { this->queue = value; }
};

class Attributes {
public:
  Attributes() = default;
  virtual ~Attributes() = default;

private:
  Queues queues;
  Configuration configuration;
  Status status;

public:
  const Queues &get_queues() const { return queues; }
  Queues &get_mutable_queues() { return queues; }
  void set_queues(const Queues &value) { this->queues = value; }

  const Configuration &get_configuration() const { return configuration; }
  Configuration &get_mutable_configuration() { return configuration; }
  void set_configuration(const Configuration &value) {
    this->configuration = value;
  }

  const Status &get_status() const { return status; }
  Status &get_mutable_status() { return status; }
  void set_status(const Status &value) { this->status = value; }
};

class Description {
public:
  Description() = default;
  virtual ~Description() = default;

private:
  std::string en;

public:
  const std::string &get_en() const { return en; }
  std::string &get_mutable_en() { return en; }
  void set_en(const std::string &value) { this->en = value; }
};

class TopologyTopology {
public:
  TopologyTopology() = default;
  virtual ~TopologyTopology() = default;

private:
  std::string qasm_header;
  std::vector<int64_t> adjacency_matrix;

public:
  const std::string &get_qasm_header() const { return qasm_header; }
  std::string &get_mutable_qasm_header() { return qasm_header; }
  void set_qasm_header(const std::string &value) { this->qasm_header = value; }

  const std::vector<int64_t> &get_adjacency_matrix() const {
    return adjacency_matrix;
  }
  std::vector<int64_t> &get_mutable_adjacency_matrix() {
    return adjacency_matrix;
  }
  void set_adjacency_matrix(const std::vector<int64_t> &value) {
    this->adjacency_matrix = value;
  }
};

class BackendTopology {
public:
  BackendTopology() = default;
  virtual ~BackendTopology() = default;

private:
  Description description;
  Description name;
  TopologyTopology topology;
  int64_t qubits;
  std::vector<std::string> execution_types;
  Attributes attributes;
  bool topology_default;
  bool disabled;
  bool is_simulator;
  bool allow_q_object;
  bool allow_open_pulse;
  bool deleted;
  std::string id;
  bool qconsole;
  bool is_hidden;
  std::shared_ptr<bool> allow_object_storage;
  std::shared_ptr<bool> allow_q_circuit;

public:
  const Description &get_description() const { return description; }
  Description &get_mutable_description() { return description; }
  void set_description(const Description &value) { this->description = value; }

  const Description &get_name() const { return name; }
  Description &get_mutable_name() { return name; }
  void set_name(const Description &value) { this->name = value; }

  const TopologyTopology &get_topology() const { return topology; }
  TopologyTopology &get_mutable_topology() { return topology; }
  void set_topology(const TopologyTopology &value) { this->topology = value; }

  const int64_t &get_qubits() const { return qubits; }
  int64_t &get_mutable_qubits() { return qubits; }
  void set_qubits(const int64_t &value) { this->qubits = value; }

  const std::vector<std::string> &get_execution_types() const {
    return execution_types;
  }
  std::vector<std::string> &get_mutable_execution_types() {
    return execution_types;
  }
  void set_execution_types(const std::vector<std::string> &value) {
    this->execution_types = value;
  }

  const Attributes &get_attributes() const { return attributes; }
  Attributes &get_mutable_attributes() { return attributes; }
  void set_attributes(const Attributes &value) { this->attributes = value; }

  const bool &get_topology_default() const { return topology_default; }
  bool &get_mutable_topology_default() { return topology_default; }
  void set_topology_default(const bool &value) {
    this->topology_default = value;
  }

  const bool &get_disabled() const { return disabled; }
  bool &get_mutable_disabled() { return disabled; }
  void set_disabled(const bool &value) { this->disabled = value; }

  const bool &get_is_simulator() const { return is_simulator; }
  bool &get_mutable_is_simulator() { return is_simulator; }
  void set_is_simulator(const bool &value) { this->is_simulator = value; }

  const bool &get_allow_q_object() const { return allow_q_object; }
  bool &get_mutable_allow_q_object() { return allow_q_object; }
  void set_allow_q_object(const bool &value) { this->allow_q_object = value; }

  const bool &get_allow_open_pulse() const { return allow_open_pulse; }
  bool &get_mutable_allow_open_pulse() { return allow_open_pulse; }
  void set_allow_open_pulse(const bool &value) {
    this->allow_open_pulse = value;
  }

  const bool &get_deleted() const { return deleted; }
  bool &get_mutable_deleted() { return deleted; }
  void set_deleted(const bool &value) { this->deleted = value; }

  const std::string &get_id() const { return id; }
  std::string &get_mutable_id() { return id; }
  void set_id(const std::string &value) { this->id = value; }

  const bool &get_qconsole() const { return qconsole; }
  bool &get_mutable_qconsole() { return qconsole; }
  void set_qconsole(const bool &value) { this->qconsole = value; }

  const bool &get_is_hidden() const { return is_hidden; }
  bool &get_mutable_is_hidden() { return is_hidden; }
  void set_is_hidden(const bool &value) { this->is_hidden = value; }

  std::shared_ptr<bool> get_allow_object_storage() const {
    return allow_object_storage;
  }
  void set_allow_object_storage(std::shared_ptr<bool> value) {
    this->allow_object_storage = value;
  }

  std::shared_ptr<bool> get_allow_q_circuit() const { return allow_q_circuit; }
  void set_allow_q_circuit(std::shared_ptr<bool> value) {
    this->allow_q_circuit = value;
  }
};

class Backend {
public:
  Backend() = default;
  virtual ~Backend() = default;

private:
  std::string name;
  std::string status;
  std::string serial_number;
  std::string type;
  std::string gate_set;
  std::string online_date;
  bool deleted;
  SpecificConfiguration specific_configuration;
  std::string id;
  std::string topology_id;
  bool qconsole;
  BackendTopology topology;
  std::shared_ptr<std::string> version;
  std::shared_ptr<std::string> description;
  std::shared_ptr<std::string> doc_url;
  std::shared_ptr<std::string> basis_gates;
  std::shared_ptr<std::string> chip_name;
  std::shared_ptr<bool> stats_public;
  std::shared_ptr<Stats> stats;
  nlohmann::json attributes;
  std::shared_ptr<bool> allow_object_storage;

public:
  const std::string &get_name() const { return name; }
  std::string &get_mutable_name() { return name; }
  void set_name(const std::string &value) { this->name = value; }

  const std::string &get_status() const { return status; }
  std::string &get_mutable_status() { return status; }
  void set_status(const std::string &value) { this->status = value; }

  const std::string &get_serial_number() const { return serial_number; }
  std::string &get_mutable_serial_number() { return serial_number; }
  void set_serial_number(const std::string &value) {
    this->serial_number = value;
  }

  const std::string &get_type() const { return type; }
  std::string &get_mutable_type() { return type; }
  void set_type(const std::string &value) { this->type = value; }

  const std::string &get_gate_set() const { return gate_set; }
  std::string &get_mutable_gate_set() { return gate_set; }
  void set_gate_set(const std::string &value) { this->gate_set = value; }

  const std::string &get_online_date() const { return online_date; }
  std::string &get_mutable_online_date() { return online_date; }
  void set_online_date(const std::string &value) { this->online_date = value; }

  const bool &get_deleted() const { return deleted; }
  bool &get_mutable_deleted() { return deleted; }
  void set_deleted(const bool &value) { this->deleted = value; }

  const SpecificConfiguration &get_specific_configuration() const {
    return specific_configuration;
  }
  SpecificConfiguration &get_mutable_specific_configuration() {
    return specific_configuration;
  }
  void set_specific_configuration(const SpecificConfiguration &value) {
    this->specific_configuration = value;
  }

  const std::string &get_id() const { return id; }
  std::string &get_mutable_id() { return id; }
  void set_id(const std::string &value) { this->id = value; }

  const std::string &get_topology_id() const { return topology_id; }
  std::string &get_mutable_topology_id() { return topology_id; }
  void set_topology_id(const std::string &value) { this->topology_id = value; }

  const bool &get_qconsole() const { return qconsole; }
  bool &get_mutable_qconsole() { return qconsole; }
  void set_qconsole(const bool &value) { this->qconsole = value; }

  const BackendTopology &get_topology() const { return topology; }
  BackendTopology &get_mutable_topology() { return topology; }
  void set_topology(const BackendTopology &value) { this->topology = value; }

  std::shared_ptr<std::string> get_version() const { return version; }
  void set_version(std::shared_ptr<std::string> value) {
    this->version = value;
  }

  std::shared_ptr<std::string> get_description() const { return description; }
  void set_description(std::shared_ptr<std::string> value) {
    this->description = value;
  }

  std::shared_ptr<std::string> get_doc_url() const { return doc_url; }
  void set_doc_url(std::shared_ptr<std::string> value) {
    this->doc_url = value;
  }

  std::shared_ptr<std::string> get_basis_gates() const { return basis_gates; }
  void set_basis_gates(std::shared_ptr<std::string> value) {
    this->basis_gates = value;
  }

  std::shared_ptr<std::string> get_chip_name() const { return chip_name; }
  void set_chip_name(std::shared_ptr<std::string> value) {
    this->chip_name = value;
  }

  std::shared_ptr<bool> get_stats_public() const { return stats_public; }
  void set_stats_public(std::shared_ptr<bool> value) {
    this->stats_public = value;
  }

  std::shared_ptr<Stats> get_stats() const { return stats; }
  void set_stats(std::shared_ptr<Stats> value) { this->stats = value; }

  const nlohmann::json &get_attributes() const { return attributes; }
  nlohmann::json &get_mutable_attributes() { return attributes; }
  void set_attributes(const nlohmann::json &value) { this->attributes = value; }

  std::shared_ptr<bool> get_allow_object_storage() const {
    return allow_object_storage;
  }
  void set_allow_object_storage(std::shared_ptr<bool> value) {
    this->allow_object_storage = value;
  }
};

class Backends {
public:
  Backends() = default;
  virtual ~Backends() = default;

private:
  std::vector<Backend> backends;

public:
  const std::vector<Backend> &get_backends() const { return backends; }
  std::vector<Backend> &get_mutable_backends() { return backends; }
  void set_backends(const std::vector<Backend> &value) {
    this->backends = value;
  }
};
} // namespace ibm_backend
} // namespace xacc

namespace nlohmann {
void from_json(const json &j, xacc::ibm_backend::Sequence &x);
void to_json(json &j, const xacc::ibm_backend::Sequence &x);

void from_json(const json &j, xacc::ibm_backend::CmdDef &x);
void to_json(json &j, const xacc::ibm_backend::CmdDef &x);

void from_json(const json &j, xacc::ibm_backend::Neighborhood &x);
void to_json(json &j, const xacc::ibm_backend::Neighborhood &x);

void from_json(const json &j, xacc::ibm_backend::DiscriminatorParams &x);
void to_json(json &j, const xacc::ibm_backend::DiscriminatorParams &x);

void from_json(const json &j, xacc::ibm_backend::Discriminator &x);
void to_json(json &j, const xacc::ibm_backend::Discriminator &x);

void from_json(const json &j, xacc::ibm_backend::MeasKernelParams &x);
void to_json(json &j, const xacc::ibm_backend::MeasKernelParams &x);

void from_json(const json &j, xacc::ibm_backend::MeasKernel &x);
void to_json(json &j, const xacc::ibm_backend::MeasKernel &x);

void from_json(const json &j, xacc::ibm_backend::PulseLibrary &x);
void to_json(json &j, const xacc::ibm_backend::PulseLibrary &x);

void from_json(const json &j, xacc::ibm_backend::Defaults &x);
void to_json(json &j, const xacc::ibm_backend::Defaults &x);

void from_json(const json &j, xacc::ibm_backend::Gate &x);
void to_json(json &j, const xacc::ibm_backend::Gate &x);

void from_json(const json &j, xacc::ibm_backend::Hamiltonian &x);
void to_json(json &j, const xacc::ibm_backend::Hamiltonian &x);

void from_json(const json &j, xacc::ibm_backend::UChannelLo &x);
void to_json(json &j, const xacc::ibm_backend::UChannelLo &x);

void from_json(const json &j, xacc::ibm_backend::SpecificConfiguration &x);
void to_json(json &j, const xacc::ibm_backend::SpecificConfiguration &x);

void from_json(const json &j, xacc::ibm_backend::Temperature &x);
void to_json(json &j, const xacc::ibm_backend::Temperature &x);

void from_json(const json &j, xacc::ibm_backend::FridgeParameters &x);
void to_json(json &j, const xacc::ibm_backend::FridgeParameters &x);

void from_json(const json &j, xacc::ibm_backend::Error &x);
void to_json(json &j, const xacc::ibm_backend::Error &x);

void from_json(const json &j, xacc::ibm_backend::MultiQubitGate &x);
void to_json(json &j, const xacc::ibm_backend::MultiQubitGate &x);

void from_json(const json &j, xacc::ibm_backend::Qubit &x);
void to_json(json &j, const xacc::ibm_backend::Qubit &x);

void from_json(const json &j, xacc::ibm_backend::Stats &x);
void to_json(json &j, const xacc::ibm_backend::Stats &x);

void from_json(const json &j, xacc::ibm_backend::Configuration &x);
void to_json(json &j, const xacc::ibm_backend::Configuration &x);

void from_json(const json &j, xacc::ibm_backend::Queues &x);
void to_json(json &j, const xacc::ibm_backend::Queues &x);

void from_json(const json &j, xacc::ibm_backend::Status &x);
void to_json(json &j, const xacc::ibm_backend::Status &x);

void from_json(const json &j, xacc::ibm_backend::Attributes &x);
void to_json(json &j, const xacc::ibm_backend::Attributes &x);

void from_json(const json &j, xacc::ibm_backend::Description &x);
void to_json(json &j, const xacc::ibm_backend::Description &x);

void from_json(const json &j, xacc::ibm_backend::TopologyTopology &x);
void to_json(json &j, const xacc::ibm_backend::TopologyTopology &x);

void from_json(const json &j, xacc::ibm_backend::BackendTopology &x);
void to_json(json &j, const xacc::ibm_backend::BackendTopology &x);

void from_json(const json &j, xacc::ibm_backend::Backend &x);
void to_json(json &j, const xacc::ibm_backend::Backend &x);

void from_json(const json &j, xacc::ibm_backend::Backends &x);
void to_json(json &j, const xacc::ibm_backend::Backends &x);

void from_json(const json &j, xacc::ibm_backend::Name &x);
void to_json(json &j, const xacc::ibm_backend::Name &x);

void from_json(const json &j, xacc::ibm_backend::PhaseEnum &x);
void to_json(json &j, const xacc::ibm_backend::PhaseEnum &x);

void from_json(const json &j, xacc::ibm_backend::Parameter &x);
void to_json(json &j, const xacc::ibm_backend::Parameter &x);

void from_json(const json &j, xacc::ibm_backend::Unit &x);
void to_json(json &j, const xacc::ibm_backend::Unit &x);
void from_json(const json &j, mpark::variant<double, xacc::ibm_backend::PhaseEnum> &x);
void to_json(json &j, const mpark::variant<double, xacc::ibm_backend::PhaseEnum> &x);

inline void from_json(const json &j, xacc::ibm_backend::Sequence &x) {
  x.set_ch(xacc::ibm_backend::get_optional<std::string>(j, "ch"));
  x.set_name(j.at("name").get<std::string>());
  x.set_phase(
      xacc::ibm_backend::get_optional<mpark::variant<double, xacc::ibm_backend::PhaseEnum>>(
          j, "phase"));
  x.set_t0(j.at("t0").get<int64_t>());
  x.set_duration(xacc::ibm_backend::get_optional<int64_t>(j, "duration"));
  x.set_memory_slot(
      xacc::ibm_backend::get_optional<std::vector<int64_t>>(j, "memory_slot"));
  x.set_qubits(xacc::ibm_backend::get_optional<std::vector<int64_t>>(j, "qubits"));
}

inline void to_json(json &j, const xacc::ibm_backend::Sequence &x) {
  j = json::object();
  j["ch"] = x.get_ch();
  j["name"] = x.get_name();
  j["phase"] = x.get_phase();
  j["t0"] = x.get_t0();
  j["duration"] = x.get_duration();
  j["memory_slot"] = x.get_memory_slot();
  j["qubits"] = x.get_qubits();
}

inline void from_json(const json &j, xacc::ibm_backend::CmdDef &x) {
  x.set_name(j.at("name").get<xacc::ibm_backend::Name>());
  x.set_qubits(j.at("qubits").get<std::vector<int64_t>>());
  x.set_sequence(j.at("sequence").get<std::vector<xacc::ibm_backend::Sequence>>());
}

inline void to_json(json &j, const xacc::ibm_backend::CmdDef &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["qubits"] = x.get_qubits();
  j["sequence"] = x.get_sequence();
}

inline void from_json(const json &j, xacc::ibm_backend::Neighborhood &x) {
  x.set_channels(j.at("channels").get<int64_t>());
  x.set_qubits(j.at("qubits").get<int64_t>());
}

inline void to_json(json &j, const xacc::ibm_backend::Neighborhood &x) {
  j = json::object();
  j["channels"] = x.get_channels();
  j["qubits"] = x.get_qubits();
}

inline void from_json(const json &j, xacc::ibm_backend::DiscriminatorParams &x) {
  x.set_neighborhoods(
      j.at("neighborhoods").get<std::vector<xacc::ibm_backend::Neighborhood>>());
  x.set_resample(j.at("resample").get<bool>());
}

inline void to_json(json &j, const xacc::ibm_backend::DiscriminatorParams &x) {
  j = json::object();
  j["neighborhoods"] = x.get_neighborhoods();
  j["resample"] = x.get_resample();
}

inline void from_json(const json &j, xacc::ibm_backend::Discriminator &x) {
  x.set_name(j.at("name").get<std::string>());
  x.set_params(j.at("params").get<xacc::ibm_backend::DiscriminatorParams>());
}

inline void to_json(json &j, const xacc::ibm_backend::Discriminator &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["params"] = x.get_params();
}

inline void from_json(const json &j, xacc::ibm_backend::MeasKernelParams &x) {}

inline void to_json(json &j, const xacc::ibm_backend::MeasKernelParams &x) {
  j = json::object();
}

inline void from_json(const json &j, xacc::ibm_backend::MeasKernel &x) {
  x.set_name(j.at("name").get<std::string>());
  x.set_params(j.at("params").get<xacc::ibm_backend::MeasKernelParams>());
}

inline void to_json(json &j, const xacc::ibm_backend::MeasKernel &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["params"] = x.get_params();
}

inline void from_json(const json &j, xacc::ibm_backend::PulseLibrary &x) {
  x.set_name(j.at("name").get<std::string>());
  x.set_samples(j.at("samples").get<std::vector<std::vector<double>>>());
}

inline void to_json(json &j, const xacc::ibm_backend::PulseLibrary &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["samples"] = x.get_samples();
}

inline void from_json(const json &j, xacc::ibm_backend::Defaults &x) {
  x.set_buffer(j.at("buffer").get<int64_t>());
  x.set_cmd_def(j.at("cmd_def").get<std::vector<xacc::ibm_backend::CmdDef>>());
  x.set_discriminator(j.at("discriminator").get<xacc::ibm_backend::Discriminator>());
  x.set_meas_freq_est(j.at("meas_freq_est").get<std::vector<double>>());
  x.set_meas_kernel(j.at("meas_kernel").get<xacc::ibm_backend::MeasKernel>());
  x.set_pulse_library(
      j.at("pulse_library").get<std::vector<xacc::ibm_backend::PulseLibrary>>());
  x.set_qubit_freq_est(j.at("qubit_freq_est").get<std::vector<double>>());
}

inline void to_json(json &j, const xacc::ibm_backend::Defaults &x) {
  j = json::object();
  j["buffer"] = x.get_buffer();
  j["cmd_def"] = x.get_cmd_def();
  j["discriminator"] = x.get_discriminator();
  j["meas_freq_est"] = x.get_meas_freq_est();
  j["meas_kernel"] = x.get_meas_kernel();
  j["pulse_library"] = x.get_pulse_library();
  j["qubit_freq_est"] = x.get_qubit_freq_est();
}

inline void from_json(const json &j, xacc::ibm_backend::Gate &x) {
  x.set_name(j.at("name").get<xacc::ibm_backend::Name>());
  x.set_parameters(j.at("parameters").get<std::vector<xacc::ibm_backend::Parameter>>());
  x.set_qasm_def(j.at("qasm_def").get<std::string>());
  x.set_coupling_map(xacc::ibm_backend::get_optional<std::vector<std::vector<int64_t>>>(
      j, "coupling_map"));
}

inline void to_json(json &j, const xacc::ibm_backend::Gate &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["parameters"] = x.get_parameters();
  j["qasm_def"] = x.get_qasm_def();
  j["coupling_map"] = x.get_coupling_map();
}

inline void from_json(const json &j, xacc::ibm_backend::Hamiltonian &x) {
  x.set_description(j.at("description").get<std::string>());
  x.set_h_latex(j.at("h_latex").get<std::string>());
  x.set_h_str(j.at("h_str").get<std::vector<std::string>>());
  x.set_osc(j.at("osc").get<std::map<std::string, int64_t>>());
  x.set_vars(j.at("vars").get<std::map<std::string, double>>());
  x.set_qub(xacc::ibm_backend::get_optional<std::map<std::string, int64_t>>(j, "qub"));
}

inline void to_json(json &j, const xacc::ibm_backend::Hamiltonian &x) {
  j = json::object();
  j["description"] = x.get_description();
  j["h_latex"] = x.get_h_latex();
  j["h_str"] = x.get_h_str();
  j["osc"] = x.get_osc();
  j["vars"] = x.get_vars();
  j["qub"] = x.get_qub();
}

inline void from_json(const json &j, xacc::ibm_backend::UChannelLo &x) {
  x.set_q(j.at("q").get<int64_t>());
  x.set_scale(j.at("scale").get<std::vector<int64_t>>());
}

inline void to_json(json &j, const xacc::ibm_backend::UChannelLo &x) {
  j = json::object();
  j["q"] = x.get_q();
  j["scale"] = x.get_scale();
}

inline void from_json(const json &j, xacc::ibm_backend::SpecificConfiguration &x) {
  x.set_backend_name(j.at("backend_name").get<std::string>());
  x.set_local(j.at("local").get<bool>());
  x.set_coupling_map(xacc::ibm_backend::get_optional<std::vector<std::vector<int64_t>>>(
      j, "coupling_map"));
  x.set_conditional(j.at("conditional").get<bool>());
  x.set_open_pulse(j.at("open_pulse").get<bool>());
  x.set_allow_q_object(j.at("allow_q_object").get<bool>());
  x.set_max_experiments(j.at("max_experiments").get<int64_t>());
  x.set_backend_version(j.at("backend_version").get<std::string>());
  x.set_n_qubits(j.at("n_qubits").get<int64_t>());
  x.set_basis_gates(j.at("basis_gates").get<std::vector<std::string>>());
  x.set_gates(j.at("gates").get<std::vector<xacc::ibm_backend::Gate>>());
  x.set_simulator(j.at("simulator").get<bool>());
  x.set_max_shots(j.at("max_shots").get<int64_t>());
  x.set_memory(j.at("memory").get<bool>());
  x.set_acquisition_latency(
      xacc::ibm_backend::get_optional<std::vector<json>>(j, "acquisition_latency"));
  x.set_conditional_latency(
      xacc::ibm_backend::get_optional<std::vector<json>>(j, "conditional_latency"));
  x.set_credits_required(xacc::ibm_backend::get_optional<bool>(j, "credits_required"));
  x.set_defaults(xacc::ibm_backend::get_optional<xacc::ibm_backend::Defaults>(j, "defaults"));
  x.set_description(xacc::ibm_backend::get_optional<std::string>(j, "description"));
  x.set_discriminators(
      xacc::ibm_backend::get_optional<std::vector<std::string>>(j, "discriminators"));
  x.set_dt(xacc::ibm_backend::get_optional<double>(j, "dt"));
  x.set_dtm(xacc::ibm_backend::get_optional<double>(j, "dtm"));
  x.set_hamiltonian(
      xacc::ibm_backend::get_optional<xacc::ibm_backend::Hamiltonian>(j, "hamiltonian"));
  x.set_meas_kernels(
      xacc::ibm_backend::get_optional<std::vector<std::string>>(j, "meas_kernels"));
  x.set_meas_levels(
      xacc::ibm_backend::get_optional<std::vector<int64_t>>(j, "meas_levels"));
  x.set_meas_lo_range(xacc::ibm_backend::get_optional<std::vector<std::vector<double>>>(
      j, "meas_lo_range"));
  x.set_meas_map(xacc::ibm_backend::get_optional<std::vector<std::vector<int64_t>>>(
      j, "meas_map"));
  x.set_n_registers(xacc::ibm_backend::get_optional<int64_t>(j, "n_registers"));
  x.set_n_uchannels(xacc::ibm_backend::get_optional<int64_t>(j, "n_uchannels"));
  x.set_online_date(xacc::ibm_backend::get_optional<std::string>(j, "online_date"));
  x.set_qubit_lo_range(
      xacc::ibm_backend::get_optional<std::vector<std::vector<double>>>(
          j, "qubit_lo_range"));
  x.set_rep_times(
      xacc::ibm_backend::get_optional<std::vector<int64_t>>(j, "rep_times"));
  x.set_sample_name(xacc::ibm_backend::get_optional<std::string>(j, "sample_name"));
  x.set_u_channel_lo(
      xacc::ibm_backend::get_optional<std::vector<std::vector<xacc::ibm_backend::UChannelLo>>>(
          j, "u_channel_lo"));
  x.set_url(xacc::ibm_backend::get_optional<std::string>(j, "url"));
}

inline void to_json(json &j, const xacc::ibm_backend::SpecificConfiguration &x) {
  j = json::object();
  j["backend_name"] = x.get_backend_name();
  j["local"] = x.get_local();
  j["coupling_map"] = x.get_coupling_map();
  j["conditional"] = x.get_conditional();
  j["open_pulse"] = x.get_open_pulse();
  j["allow_q_object"] = x.get_allow_q_object();
  j["max_experiments"] = x.get_max_experiments();
  j["backend_version"] = x.get_backend_version();
  j["n_qubits"] = x.get_n_qubits();
  j["basis_gates"] = x.get_basis_gates();
  j["gates"] = x.get_gates();
  j["simulator"] = x.get_simulator();
  j["max_shots"] = x.get_max_shots();
  j["memory"] = x.get_memory();
  j["acquisition_latency"] = x.get_acquisition_latency();
  j["conditional_latency"] = x.get_conditional_latency();
  j["credits_required"] = x.get_credits_required();
  j["defaults"] = x.get_defaults();
  j["description"] = x.get_description();
  j["discriminators"] = x.get_discriminators();
  j["dt"] = x.get_dt();
  j["dtm"] = x.get_dtm();
  j["hamiltonian"] = x.get_hamiltonian();
  j["meas_kernels"] = x.get_meas_kernels();
  j["meas_levels"] = x.get_meas_levels();
  j["meas_lo_range"] = x.get_meas_lo_range();
  j["meas_map"] = x.get_meas_map();
  j["n_registers"] = x.get_n_registers();
  j["n_uchannels"] = x.get_n_uchannels();
  j["online_date"] = x.get_online_date();
  j["qubit_lo_range"] = x.get_qubit_lo_range();
  j["rep_times"] = x.get_rep_times();
  j["sample_name"] = x.get_sample_name();
  j["u_channel_lo"] = x.get_u_channel_lo();
  j["url"] = x.get_url();
}

inline void from_json(const json &j, xacc::ibm_backend::Temperature &x) {
  x.set_date(j.at("date").get<std::string>());
  x.set_unit(j.at("unit").get<xacc::ibm_backend::Unit>());
  x.set_value(j.at("value").get<double>());
}

inline void to_json(json &j, const xacc::ibm_backend::Temperature &x) {
  j = json::object();
  j["date"] = x.get_date();
  j["unit"] = x.get_unit();
  j["value"] = x.get_value();
}

inline void from_json(const json &j, xacc::ibm_backend::FridgeParameters &x) {
  x.set_temperature(j.at("Temperature").get<xacc::ibm_backend::Temperature>());
  x.set_cooldown_date(j.at("cooldownDate").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_backend::FridgeParameters &x) {
  j = json::object();
  j["Temperature"] = x.get_temperature();
  j["cooldownDate"] = x.get_cooldown_date();
}

inline void from_json(const json &j, xacc::ibm_backend::Error &x) {
  x.set_date(j.at("date").get<std::string>());
  x.set_value(j.at("value").get<double>());
}

inline void to_json(json &j, const xacc::ibm_backend::Error &x) {
  j = json::object();
  j["date"] = x.get_date();
  j["value"] = x.get_value();
}

inline void from_json(const json &j, xacc::ibm_backend::MultiQubitGate &x) {
  x.set_gate_error(j.at("gateError").get<xacc::ibm_backend::Error>());
  x.set_name(j.at("name").get<std::string>());
  x.set_qubits(j.at("qubits").get<std::vector<int64_t>>());
  x.set_type(j.at("type").get<xacc::ibm_backend::Name>());
}

inline void to_json(json &j, const xacc::ibm_backend::MultiQubitGate &x) {
  j = json::object();
  j["gateError"] = x.get_gate_error();
  j["name"] = x.get_name();
  j["qubits"] = x.get_qubits();
  j["type"] = x.get_type();
}

inline void from_json(const json &j, xacc::ibm_backend::Qubit &x) {
  x.set_t1(j.at("T1").get<xacc::ibm_backend::Temperature>());
  x.set_t2(j.at("T2").get<xacc::ibm_backend::Temperature>());
  x.set_buffer(j.at("buffer").get<xacc::ibm_backend::Temperature>());
  x.set_frequency(j.at("frequency").get<xacc::ibm_backend::Temperature>());
  x.set_gate_time(j.at("gateTime").get<xacc::ibm_backend::Temperature>());
  x.set_name(j.at("name").get<std::string>());
  x.set_gate_error(j.at("gateError").get<xacc::ibm_backend::Error>());
  x.set_readout_error(j.at("readoutError").get<xacc::ibm_backend::Error>());
}

inline void to_json(json &j, const xacc::ibm_backend::Qubit &x) {
  j = json::object();
  j["T1"] = x.get_t1();
  j["T2"] = x.get_t2();
  j["buffer"] = x.get_buffer();
  j["frequency"] = x.get_frequency();
  j["gateTime"] = x.get_gate_time();
  j["name"] = x.get_name();
  j["gateError"] = x.get_gate_error();
  j["readoutError"] = x.get_readout_error();
}

inline void from_json(const json &j, xacc::ibm_backend::Stats &x) {
  x.set_last_update_date(j.at("lastUpdateDate").get<std::string>());
  x.set_fridge_parameters(
      j.at("fridgeParameters").get<xacc::ibm_backend::FridgeParameters>());
  x.set_qubits(j.at("qubits").get<std::vector<xacc::ibm_backend::Qubit>>());
  x.set_multi_qubit_gates(
      j.at("multiQubitGates").get<std::vector<xacc::ibm_backend::MultiQubitGate>>());
  x.set_url(j.at("url").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_backend::Stats &x) {
  j = json::object();
  j["lastUpdateDate"] = x.get_last_update_date();
  j["fridgeParameters"] = x.get_fridge_parameters();
  j["qubits"] = x.get_qubits();
  j["multiQubitGates"] = x.get_multi_qubit_gates();
  j["url"] = x.get_url();
}

inline void from_json(const json &j, xacc::ibm_backend::Configuration &x) {
  x.set_host(xacc::ibm_backend::get_optional<std::string>(j, "host"));
  x.set_port(xacc::ibm_backend::get_optional<std::string>(j, "port"));
  x.set_password(xacc::ibm_backend::get_optional<std::string>(j, "password"));
  x.set_db_number(xacc::ibm_backend::get_untyped(j, "dbNumber"));
  x.set_url(xacc::ibm_backend::get_optional<std::string>(j, "url"));
  x.set_api_token(xacc::ibm_backend::get_optional<std::string>(j, "apiToken"));
  x.set_kind(j.at("kind").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_backend::Configuration &x) {
  j = json::object();
  j["host"] = x.get_host();
  j["port"] = x.get_port();
  j["password"] = x.get_password();
  j["dbNumber"] = x.get_db_number();
  j["url"] = x.get_url();
  j["apiToken"] = x.get_api_token();
  j["kind"] = x.get_kind();
}

inline void from_json(const json &j, xacc::ibm_backend::Queues &x) {
  x.set_tasks(j.at("tasks").get<std::string>());
  x.set_results(j.at("results").get<std::string>());
  x.set_management(xacc::ibm_backend::get_optional<std::string>(j, "management"));
  x.set_has_admin_queue(xacc::ibm_backend::get_untyped(j, "hasAdminQueue"));
  x.set_has_priority_queue(xacc::ibm_backend::get_untyped(j, "hasPriorityQueue"));
}

inline void to_json(json &j, const xacc::ibm_backend::Queues &x) {
  j = json::object();
  j["tasks"] = x.get_tasks();
  j["results"] = x.get_results();
  j["management"] = x.get_management();
  j["hasAdminQueue"] = x.get_has_admin_queue();
  j["hasPriorityQueue"] = x.get_has_priority_queue();
}

inline void from_json(const json &j, xacc::ibm_backend::Status &x) {
  x.set_device(j.at("device").get<std::string>());
  x.set_queue(j.at("queue").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_backend::Status &x) {
  j = json::object();
  j["device"] = x.get_device();
  j["queue"] = x.get_queue();
}

inline void from_json(const json &j, xacc::ibm_backend::Attributes &x) {
  x.set_queues(j.at("queues").get<xacc::ibm_backend::Queues>());
  x.set_configuration(j.at("configuration").get<xacc::ibm_backend::Configuration>());
  x.set_status(j.at("status").get<xacc::ibm_backend::Status>());
}

inline void to_json(json &j, const xacc::ibm_backend::Attributes &x) {
  j = json::object();
  j["queues"] = x.get_queues();
  j["configuration"] = x.get_configuration();
  j["status"] = x.get_status();
}

inline void from_json(const json &j, xacc::ibm_backend::Description &x) {
  x.set_en(j.at("en").get<std::string>());
}

inline void to_json(json &j, const xacc::ibm_backend::Description &x) {
  j = json::object();
  j["en"] = x.get_en();
}

inline void from_json(const json &j, xacc::ibm_backend::TopologyTopology &x) {
  x.set_qasm_header(j.at("qasmHeader").get<std::string>());
  x.set_adjacency_matrix(j.at("adjacencyMatrix").get<std::vector<int64_t>>());
}

inline void to_json(json &j, const xacc::ibm_backend::TopologyTopology &x) {
  j = json::object();
  j["qasmHeader"] = x.get_qasm_header();
  j["adjacencyMatrix"] = x.get_adjacency_matrix();
}

inline void from_json(const json &j, xacc::ibm_backend::BackendTopology &x) {
  x.set_description(j.at("description").get<xacc::ibm_backend::Description>());
  x.set_name(j.at("name").get<xacc::ibm_backend::Description>());
  x.set_topology(j.at("topology").get<xacc::ibm_backend::TopologyTopology>());
  x.set_qubits(j.at("qubits").get<int64_t>());
  x.set_execution_types(j.at("executionTypes").get<std::vector<std::string>>());
  x.set_attributes(j.at("attributes").get<xacc::ibm_backend::Attributes>());
  x.set_topology_default(j.at("default").get<bool>());
  x.set_disabled(j.at("disabled").get<bool>());
  x.set_is_simulator(j.at("isSimulator").get<bool>());
  x.set_allow_q_object(j.at("allowQObject").get<bool>());
  x.set_allow_open_pulse(j.at("allowOpenPulse").get<bool>());
  x.set_deleted(j.at("deleted").get<bool>());
  x.set_id(j.at("id").get<std::string>());
  if (j.find("qconsole") != j.end()) {
  x.set_qconsole(j.at("qconsole").get<bool>());
  }
  x.set_is_hidden(j.at("isHidden").get<bool>());
  x.set_allow_object_storage(
      xacc::ibm_backend::get_optional<bool>(j, "allowObjectStorage"));
  x.set_allow_q_circuit(xacc::ibm_backend::get_optional<bool>(j, "allowQCircuit"));
}

inline void to_json(json &j, const xacc::ibm_backend::BackendTopology &x) {
  j = json::object();
  j["description"] = x.get_description();
  j["name"] = x.get_name();
  j["topology"] = x.get_topology();
  j["qubits"] = x.get_qubits();
  j["executionTypes"] = x.get_execution_types();
  j["attributes"] = x.get_attributes();
  j["default"] = x.get_topology_default();
  j["disabled"] = x.get_disabled();
  j["isSimulator"] = x.get_is_simulator();
  j["allowQObject"] = x.get_allow_q_object();
  j["allowOpenPulse"] = x.get_allow_open_pulse();
  j["deleted"] = x.get_deleted();
  j["id"] = x.get_id();
  j["qconsole"] = x.get_qconsole();
  j["isHidden"] = x.get_is_hidden();
  j["allowObjectStorage"] = x.get_allow_object_storage();
  j["allowQCircuit"] = x.get_allow_q_circuit();
}

inline void from_json(const json &j, xacc::ibm_backend::Backend &x) {
  x.set_name(j.at("name").get<std::string>());
  x.set_status(j.at("status").get<std::string>());
  if (j.find("serialNumber") != j.end()) {x.set_serial_number(j.at("serialNumber").get<std::string>());}
  if (j.find("type") != j.end()) {x.set_type(j.at("type").get<std::string>());}
  if (j.find("gateSet") != j.end()) {x.set_gate_set(j.at("gateSet").get<std::string>());}
  x.set_online_date(j.at("onlineDate").get<std::string>());
  x.set_deleted(j.at("deleted").get<bool>());
  x.set_specific_configuration(
      j.at("specificConfiguration").get<xacc::ibm_backend::SpecificConfiguration>());
  if (j.find("id") != j.end()) {x.set_id(j.at("id").get<std::string>());}
  if (j.find("topologyId") != j.end()) {x.set_topology_id(j.at("topologyId").get<std::string>());}
  if (j.find("qconsole") != j.end()) x.set_qconsole(j.at("qconsole").get<bool>());
  if (j.find("topology") != j.end()) {x.set_topology(j.at("topology").get<xacc::ibm_backend::BackendTopology>());}
  x.set_version(xacc::ibm_backend::get_optional<std::string>(j, "version"));
  x.set_description(xacc::ibm_backend::get_optional<std::string>(j, "description"));
  x.set_doc_url(xacc::ibm_backend::get_optional<std::string>(j, "docUrl"));
  x.set_basis_gates(xacc::ibm_backend::get_optional<std::string>(j, "basisGates"));
  x.set_chip_name(xacc::ibm_backend::get_optional<std::string>(j, "chipName"));
  x.set_stats_public(xacc::ibm_backend::get_optional<bool>(j, "statsPublic"));
  x.set_stats(xacc::ibm_backend::get_optional<xacc::ibm_backend::Stats>(j, "stats"));
  x.set_attributes(xacc::ibm_backend::get_untyped(j, "attributes"));
  x.set_allow_object_storage(
      xacc::ibm_backend::get_optional<bool>(j, "allowObjectStorage"));
}

inline void to_json(json &j, const xacc::ibm_backend::Backend &x) {
  j = json::object();
  j["name"] = x.get_name();
  j["status"] = x.get_status();
  j["serialNumber"] = x.get_serial_number();
  j["type"] = x.get_type();
  j["gateSet"] = x.get_gate_set();
  j["onlineDate"] = x.get_online_date();
  j["deleted"] = x.get_deleted();
  j["specificConfiguration"] = x.get_specific_configuration();
  j["id"] = x.get_id();
  j["topologyId"] = x.get_topology_id();
  j["qconsole"] = x.get_qconsole();
  j["topology"] = x.get_topology();
  j["version"] = x.get_version();
  j["description"] = x.get_description();
  j["docUrl"] = x.get_doc_url();
  j["basisGates"] = x.get_basis_gates();
  j["chipName"] = x.get_chip_name();
  j["statsPublic"] = x.get_stats_public();
  j["stats"] = x.get_stats();
  j["attributes"] = x.get_attributes();
  j["allowObjectStorage"] = x.get_allow_object_storage();
}

inline void from_json(const json &j, xacc::ibm_backend::Backends &x) {
  x.set_backends(j.at("backends").get<std::vector<xacc::ibm_backend::Backend>>());
}

inline void to_json(json &j, const xacc::ibm_backend::Backends &x) {
  j = json::object();
  j["backends"] = x.get_backends();
}

inline void from_json(const json &j, xacc::ibm_backend::Name &x) {
  if (j == "cx")
    x = xacc::ibm_backend::Name::CX;
  else if (j == "id")
    x = xacc::ibm_backend::Name::ID;
  else if (j == "MEAS")
    x = xacc::ibm_backend::Name::MEAS;
  else if (j == "measure")
    x = xacc::ibm_backend::Name::MEASURE;
  else if (j == "u1")
    x = xacc::ibm_backend::Name::U1;
  else if (j == "u2")
    x = xacc::ibm_backend::Name::U2;
  else if (j == "u3")
    x = xacc::ibm_backend::Name::U3;
  else if (j == "x")
    x = xacc::ibm_backend::Name::X;
  else
    throw "Input JSON does not conform to schema";
}

inline void to_json(json &j, const xacc::ibm_backend::Name &x) {
  switch (x) {
  case xacc::ibm_backend::Name::CX:
    j = "cx";
    break;
  case xacc::ibm_backend::Name::ID:
    j = "id";
    break;
  case xacc::ibm_backend::Name::MEAS:
    j = "MEAS";
    break;
  case xacc::ibm_backend::Name::MEASURE:
    j = "measure";
    break;
  case xacc::ibm_backend::Name::U1:
    j = "u1";
    break;
  case xacc::ibm_backend::Name::U2:
    j = "u2";
    break;
  case xacc::ibm_backend::Name::U3:
    j = "u3";
    break;
  case xacc::ibm_backend::Name::X:
    j = "x";
    break;
  default:
    throw "This should not happen";
  }
}

inline void from_json(const json &j, xacc::ibm_backend::PhaseEnum &x) {
  if (j == "-(P0)")
    x = xacc::ibm_backend::PhaseEnum::P0;
  else if (j == "-(P1)")
    x = xacc::ibm_backend::PhaseEnum::P1;
  else if (j == "-(P2)")
    x = xacc::ibm_backend::PhaseEnum::P2;
  else if (j == "1*(-(P0))")
    x = xacc::ibm_backend::PhaseEnum::THE_1_P0;
  else if (j == "1*(-(P1))")
    x = xacc::ibm_backend::PhaseEnum::THE_1_P1;
  else if (j == "1*(-(P2))")
    x = xacc::ibm_backend::PhaseEnum::THE_1_P2;
  else
    throw "Input JSON does not conform to schema";
}

inline void to_json(json &j, const xacc::ibm_backend::PhaseEnum &x) {
  switch (x) {
  case xacc::ibm_backend::PhaseEnum::P0:
    j = "-(P0)";
    break;
  case xacc::ibm_backend::PhaseEnum::P1:
    j = "-(P1)";
    break;
  case xacc::ibm_backend::PhaseEnum::P2:
    j = "-(P2)";
    break;
  case xacc::ibm_backend::PhaseEnum::THE_1_P0:
    j = "1*(-(P0))";
    break;
  case xacc::ibm_backend::PhaseEnum::THE_1_P1:
    j = "1*(-(P1))";
    break;
  case xacc::ibm_backend::PhaseEnum::THE_1_P2:
    j = "1*(-(P2))";
    break;
  default:
    throw "This should not happen";
  }
}

inline void from_json(const json &j, xacc::ibm_backend::Parameter &x) {
  if (j == "lambda")
    x = xacc::ibm_backend::Parameter::LAMBDA;
  else if (j == "phi")
    x = xacc::ibm_backend::Parameter::PHI;
  else if (j == "theta")
    x = xacc::ibm_backend::Parameter::THETA;
  else
    throw "Input JSON does not conform to schema";
}

inline void to_json(json &j, const xacc::ibm_backend::Parameter &x) {
  switch (x) {
  case xacc::ibm_backend::Parameter::LAMBDA:
    j = "lambda";
    break;
  case xacc::ibm_backend::Parameter::PHI:
    j = "phi";
    break;
  case xacc::ibm_backend::Parameter::THETA:
    j = "theta";
    break;
  default:
    throw "This should not happen";
  }
}

inline void from_json(const json &j, xacc::ibm_backend::Unit &x) {
  if (j == "GHz")
    x = xacc::ibm_backend::Unit::G_HZ;
  else if (j == "mK")
    x = xacc::ibm_backend::Unit::M_K;
  else if (j == "ns")
    x = xacc::ibm_backend::Unit::NS;
  else if (j == "µs")
    x = xacc::ibm_backend::Unit::S;
  else
    throw "Input JSON does not conform to schema";
}

inline void to_json(json &j, const xacc::ibm_backend::Unit &x) {
  switch (x) {
  case xacc::ibm_backend::Unit::G_HZ:
    j = "GHz";
    break;
  case xacc::ibm_backend::Unit::M_K:
    j = "mK";
    break;
  case xacc::ibm_backend::Unit::NS:
    j = "ns";
    break;
  case xacc::ibm_backend::Unit::S:
    j = "µs";
    break;
  default:
    throw "This should not happen";
  }
}
inline void from_json(const json &j,
                      mpark::variant<double, xacc::ibm_backend::PhaseEnum> &x) {
  if (j.is_number())
    x = j.get<double>();
  else if (j.is_string())
    x = j.get<xacc::ibm_backend::PhaseEnum>();
  else
    throw "Could not deserialize";
}

inline void to_json(json &j,
                    const mpark::variant<double, xacc::ibm_backend::PhaseEnum> &x) {
  switch (x.index()) {
  case 0:
    j = mpark::get<double>(x);
    break;
  case 1:
    j = mpark::get<xacc::ibm_backend::PhaseEnum>(x);
    break;
  default:
    throw "Input JSON does not conform to schema";
  }
}
} // namespace nlohmann
