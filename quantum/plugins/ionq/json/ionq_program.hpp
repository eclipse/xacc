#pragma once

#include "json.hpp"

#include <stdexcept>
#include <regex>
#include <vector>

namespace xacc {
namespace ionq {
    using nlohmann::json;

    template <typename T>
    inline T get_optional(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<T>();
        }
        return T();
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json & j, std::string property) {
        return get_optional<T>(j, property.data());
    }

    class CircuitInstruction {
        public:
        CircuitInstruction() = default;
        virtual ~CircuitInstruction() = default;

        private:
        std::string gate;
        std::int64_t target;
        std::int64_t control = 0;
        double rotation = 0.0;

        public:
        const std::string & get_gate() const { return gate; }
        std::string & get_mutable_gate() { return gate; }
        void set_gate(const std::string & value) { this->gate = value; }

        const int64_t & get_target() const { return target; }
        int64_t & get_mutable_target() { return target; }
        void set_target(const std::int64_t & value) { this->target = value; }

        std::int64_t get_control() const { return control; }
        void set_control(std::int64_t value) { this->control = value; }

        double get_rotation() const { return rotation; }
        void set_rotation(double value) { this->rotation = value; }
    };

    class Body {
        public:
        Body() = default;
        virtual ~Body() = default;

        private:
        std::int64_t qubits;
        std::vector<CircuitInstruction> circuit;

        public:
        const int64_t & get_qubits() const { return qubits; }
        int64_t & get_mutable_qubits() { return qubits; }
        void set_qubits(const int64_t & value) { this->qubits = value; }

        const std::vector<CircuitInstruction> & get_circuit() const { return circuit; }
        std::vector<CircuitInstruction> & get_mutable_circuit() { return circuit; }
        void set_circuit(const std::vector<CircuitInstruction> & value) { this->circuit = value; }
    };

    class IonQProgram {
        public:
        IonQProgram() = default;
        virtual ~IonQProgram() = default;

        private:
        std::string lang = "json";
        std::string target;
        std::int64_t shots;
        Body body;

        public:
        const std::string & get_lang() const { return lang; }
        std::string & get_mutable_lang() { return lang; }
        void set_lang(const std::string & value) { this->lang = value; }

        const std::string & get_target() const { return target; }
        std::string & get_mutable_target() { return target; }
        void set_target(const std::string & value) { this->target = value; }

        const std::int64_t & get_shots() const { return shots; }
        std::int64_t & get_mutable_shots() { return shots; }
        void set_shots(const std::int64_t & value) { this->shots = value; }

        const Body & get_body() const { return body; }
        Body & get_mutable_body() { return body; }
        void set_body(const Body & value) { this->body = value; }
    };
}
}

namespace nlohmann {
    void from_json(const json & j, xacc::ionq::CircuitInstruction & x);
    void to_json(json & j, const xacc::ionq::CircuitInstruction & x);

    void from_json(const json & j, xacc::ionq::Body & x);
    void to_json(json & j, const xacc::ionq::Body & x);

    void from_json(const json & j, xacc::ionq::IonQProgram & x);
    void to_json(json & j, const xacc::ionq::IonQProgram & x);

    inline void from_json(const json & j, xacc::ionq::CircuitInstruction& x) {
        x.set_gate(j.at("gate").get<std::string>());
        x.set_target(j.at("target").get<std::int64_t>());
        x.set_control(xacc::ionq::get_optional<std::int64_t>(j, "control"));
        x.set_rotation(xacc::ionq::get_optional<double>(j, "rotation"));
    }

    inline void to_json(json & j, const xacc::ionq::CircuitInstruction & x) {
        j = json::object();
        j["gate"] = x.get_gate();
        j["target"] = x.get_target();
        if (x.get_gate() == "cnot") {
        j["control"] = x.get_control();
        }
        if (x.get_gate() == "rx" ||x.get_gate() == "ry" || x.get_gate() == "rz") {
        j["rotation"] = x.get_rotation();
        }
    }

    inline void from_json(const json & j, xacc::ionq::Body& x) {
        x.set_qubits(j.at("qubits").get<std::int64_t>());
        x.set_circuit(j.at("circuit").get<std::vector<xacc::ionq::CircuitInstruction>>());
    }

    inline void to_json(json & j, const xacc::ionq::Body & x) {
        j = json::object();
        j["qubits"] = x.get_qubits();
        j["circuit"] = x.get_circuit();
    }

    inline void from_json(const json & j, xacc::ionq::IonQProgram& x) {
        x.set_lang(j.at("lang").get<std::string>());
        x.set_target(j.at("target").get<std::string>());
        x.set_shots(j.at("shots").get<std::int64_t>());
        x.set_body(j.at("body").get<xacc::ionq::Body>());
    }

    inline void to_json(json & j, const xacc::ionq::IonQProgram & x) {
        j = json::object();
        j["lang"] = x.get_lang();
        j["target"] = x.get_target();
        j["shots"] = x.get_shots();
        j["body"] = x.get_body();
    }
}
