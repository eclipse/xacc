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
#include <iostream>

#include "json.hpp"

#include <stdexcept>
#include <regex>

namespace xacc {
namespace ibm_properties {
    using nlohmann::json;

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }

    enum class GateEnum : int { CX, ID, U1, U2, U3 };

    enum class Name : int { FREQUENCY, GATE_ERROR, GATE_LENGTH, PROB_MEAS0_PREP1, PROB_MEAS1_PREP0, READOUT_ERROR, T1, T2 };

    enum class Unit : int { EMPTY, G_HZ, NS };

    class Qubit {
        public:
        Qubit() = default;
        virtual ~Qubit() = default;

        private:
        std::string date;
        Name name;
        Unit unit;
        double value;

        public:
        const std::string & get_date() const { return date; }
        std::string & get_mutable_date() { return date; }
        void set_date(const std::string & value) { this->date = value; }

        const Name & get_name() const { return name; }
        Name & get_mutable_name() { return name; }
        void set_name(const Name & value) { this->name = value; }

        const Unit & get_unit() const { return unit; }
        Unit & get_mutable_unit() { return unit; }
        void set_unit(const Unit & value) { this->unit = value; }

        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    class GateElement {
        public:
        GateElement() = default;
        virtual ~GateElement() = default;

        private:
        GateEnum gate;
        std::string name;
        std::vector<Qubit> parameters;
        std::vector<int64_t> qubits;

        public:
        const GateEnum & get_gate() const { return gate; }
        GateEnum & get_mutable_gate() { return gate; }
        void set_gate(const GateEnum & value) { this->gate = value; }

        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        const std::vector<Qubit> & get_parameters() const { return parameters; }
        std::vector<Qubit> & get_mutable_parameters() { return parameters; }
        void set_parameters(const std::vector<Qubit> & value) { this->parameters = value; }

        const std::vector<int64_t> & get_qubits() const { return qubits; }
        std::vector<int64_t> & get_mutable_qubits() { return qubits; }
        void set_qubits(const std::vector<int64_t> & value) { this->qubits = value; }
    };

    class Properties {
        public:
        Properties() = default;
        virtual ~Properties() = default;

        private:
        std::string backend_name;
        std::string last_update_date;
        std::string backend_version;
        std::vector<GateElement> gates;
        std::vector<nlohmann::json> general;
        std::vector<std::vector<Qubit>> qubits;

        public:
        const std::string & get_backend_name() const { return backend_name; }
        std::string & get_mutable_backend_name() { return backend_name; }
        void set_backend_name(const std::string & value) { this->backend_name = value; }

        const std::string & get_last_update_date() const { return last_update_date; }
        std::string & get_mutable_last_update_date() { return last_update_date; }
        void set_last_update_date(const std::string & value) { this->last_update_date = value; }

        const std::string & get_backend_version() const { return backend_version; }
        std::string & get_mutable_backend_version() { return backend_version; }
        void set_backend_version(const std::string & value) { this->backend_version = value; }

        const std::vector<GateElement> & get_gates() const { return gates; }
        std::vector<GateElement> & get_mutable_gates() { return gates; }
        void set_gates(const std::vector<GateElement> & value) { this->gates = value; }

        const std::vector<nlohmann::json> & get_general() const { return general; }
        std::vector<nlohmann::json> & get_mutable_general() { return general; }
        void set_general(const std::vector<nlohmann::json> & value) { this->general = value; }

        const std::vector<std::vector<Qubit>> & get_qubits() const { return qubits; }
        std::vector<std::vector<Qubit>> & get_mutable_qubits() { return qubits; }
        void set_qubits(const std::vector<std::vector<Qubit>> & value) { this->qubits = value; }
    };
}
}
namespace nlohmann {
    void from_json(const json & j, xacc::ibm_properties::Qubit & x);
    void to_json(json & j, const xacc::ibm_properties::Qubit & x);

    void from_json(const json & j, xacc::ibm_properties::GateElement & x);
    void to_json(json & j, const xacc::ibm_properties::GateElement & x);

    void from_json(const json & j, xacc::ibm_properties::Properties & x);
    void to_json(json & j, const xacc::ibm_properties::Properties & x);

    void from_json(const json & j, xacc::ibm_properties::GateEnum & x);
    void to_json(json & j, const xacc::ibm_properties::GateEnum & x);

    void from_json(const json & j, xacc::ibm_properties::Name & x);
    void to_json(json & j, const xacc::ibm_properties::Name & x);

    void from_json(const json & j, xacc::ibm_properties::Unit & x);
    void to_json(json & j, const xacc::ibm_properties::Unit & x);

    inline void from_json(const json & j, xacc::ibm_properties::Qubit& x) {
        x.set_date(j.at("date").get<std::string>());
        if (j.find("name") != j.end()) x.set_name(j.at("name").get<xacc::ibm_properties::Name>());
        x.set_unit(xacc::ibm_properties::Unit::EMPTY);//j.at("unit").get<xacc::ibm_properties::Unit>());
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const xacc::ibm_properties::Qubit & x) {
        j = json::object();
        j["date"] = x.get_date();
        j["name"] = x.get_name();
        j["unit"] = x.get_unit();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, xacc::ibm_properties::GateElement& x) {
        x.set_gate(j.at("gate").get<xacc::ibm_properties::GateEnum>());
        if (j.find("name") != j.end())x.set_name(j.at("name").get<std::string>());
        x.set_parameters(j.at("parameters").get<std::vector<xacc::ibm_properties::Qubit>>());
        x.set_qubits(j.at("qubits").get<std::vector<int64_t>>());
    }

    inline void to_json(json & j, const xacc::ibm_properties::GateElement & x) {
        j = json::object();
        j["gate"] = x.get_gate();
        j["name"] = x.get_name();
        j["parameters"] = x.get_parameters();
        j["qubits"] = x.get_qubits();
    }

    inline void from_json(const json & j, xacc::ibm_properties::Properties& x) {
        x.set_backend_name(j.at("backend_name").get<std::string>());
        x.set_last_update_date(j.at("last_update_date").get<std::string>());
        x.set_backend_version(j.at("backend_version").get<std::string>());
        x.set_gates(j.at("gates").get<std::vector<xacc::ibm_properties::GateElement>>());
        x.set_general(j.at("general").get<std::vector<json>>());
        x.set_qubits(j.at("qubits").get<std::vector<std::vector<xacc::ibm_properties::Qubit>>>());
    }

    inline void to_json(json & j, const xacc::ibm_properties::Properties & x) {
        j = json::object();
        j["backend_name"] = x.get_backend_name();
        j["last_update_date"] = x.get_last_update_date();
        j["backend_version"] = x.get_backend_version();
        j["gates"] = x.get_gates();
        j["general"] = x.get_general();
        j["qubits"] = x.get_qubits();
    }

    inline void from_json(const json & j, xacc::ibm_properties::GateEnum & x) {
        if (j == "cx") x = xacc::ibm_properties::GateEnum::CX;
        else if (j == "id") x = xacc::ibm_properties::GateEnum::ID;
        else if (j == "u1") x = xacc::ibm_properties::GateEnum::U1;
        else if (j == "u2") x = xacc::ibm_properties::GateEnum::U2;
        else if (j == "u3") x = xacc::ibm_properties::GateEnum::U3;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const xacc::ibm_properties::GateEnum & x) {
        switch (x) {
            case xacc::ibm_properties::GateEnum::CX: j = "cx"; break;
            case xacc::ibm_properties::GateEnum::ID: j = "id"; break;
            case xacc::ibm_properties::GateEnum::U1: j = "u1"; break;
            case xacc::ibm_properties::GateEnum::U2: j = "u2"; break;
            case xacc::ibm_properties::GateEnum::U3: j = "u3"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, xacc::ibm_properties::Name & x) {
        if (j == "frequency") x = xacc::ibm_properties::Name::FREQUENCY;
        else if (j == "gate_error") x = xacc::ibm_properties::Name::GATE_ERROR;
        else if (j == "gate_length") x = xacc::ibm_properties::Name::GATE_LENGTH;
        else if (j == "prob_meas0_prep1") x = xacc::ibm_properties::Name::PROB_MEAS0_PREP1;
        else if (j == "prob_meas1_prep0") x = xacc::ibm_properties::Name::PROB_MEAS1_PREP0;
        else if (j == "readout_error") x = xacc::ibm_properties::Name::READOUT_ERROR;
        else if (j == "T1") x = xacc::ibm_properties::Name::T1;
        else if (j == "T2") x = xacc::ibm_properties::Name::T2;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const xacc::ibm_properties::Name & x) {
        switch (x) {
            case xacc::ibm_properties::Name::FREQUENCY: j = "frequency"; break;
            case xacc::ibm_properties::Name::GATE_ERROR: j = "gate_error"; break;
            case xacc::ibm_properties::Name::GATE_LENGTH: j = "gate_length"; break;
            case xacc::ibm_properties::Name::PROB_MEAS0_PREP1: j = "prob_meas0_prep1"; break;
            case xacc::ibm_properties::Name::PROB_MEAS1_PREP0: j = "prob_meas1_prep0"; break;
            case xacc::ibm_properties::Name::READOUT_ERROR: j = "readout_error"; break;
            case xacc::ibm_properties::Name::T1: j = "T1"; break;
            case xacc::ibm_properties::Name::T2: j = "T2"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, xacc::ibm_properties::Unit & x) {
        if (j == "") x = xacc::ibm_properties::Unit::EMPTY;
        else if (j == "GHz") x = xacc::ibm_properties::Unit::G_HZ;
        else if (j == "ns") x = xacc::ibm_properties::Unit::NS;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const xacc::ibm_properties::Unit & x) {
        switch (x) {
            case xacc::ibm_properties::Unit::EMPTY: j = ""; break;
            case xacc::ibm_properties::Unit::G_HZ: j = "GHz"; break;
            case xacc::ibm_properties::Unit::NS: j = "ns"; break;
            default: throw "This should not happen";
        }
    }
}
