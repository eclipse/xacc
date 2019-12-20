#include "PhasePolynomialRepresentation.hpp"
#include "Instruction.hpp"
#include <cassert>

namespace {
    // Combine two affine func:
    // e.g. After "CNOT q1, q2; X q1;", if we do CNOT q1, q2 
    // we need to combine (XOR) these two funcs f1 = q2 + q1; f2 = 1 + q1;    
    BoolAffineFuncType combineAffineFunc(const BoolAffineFuncType& in_func1, const BoolAffineFuncType& in_func2) {
        assert(in_func1.size() == in_func2.size());
        BoolAffineFuncType result(in_func1.size());
        for (size_t i = 0; i < in_func1.size(); ++i) {
            // XOR each of the terms
            result[i] = (in_func1[i] != in_func2[i]);
        }

        return result;
    }
}

using xacc::Instruction;

namespace xacc { namespace quantum {
    const BoolAffineFuncType PhasePolynomialRep::EMPTY_AFFINE_FUNC_LIST = {};
    const std::vector<Instruction*> PhasePolynomialRep::EMPTY_GATE_LIST = {};

    PhasePolynomialRep::PhasePolynomialRep(int in_nQbits, const std::vector<std::shared_ptr<Instruction>>& in_subCircuit):
        m_nbQubits(in_nQbits) 
    {
        // This sub-circuit must only contain X, CNOT, or Rz gates.
        assert(std::find_if(in_subCircuit.begin(), in_subCircuit.end(), [](const std::shared_ptr<Instruction>& in_gatePtr){
            return in_gatePtr->name() != "X" && in_gatePtr->name() != "Rz" && in_gatePtr->name() != "CNOT";
        }) == in_subCircuit.end());

        // Running boolean sequences (left to right) on a particular qubit line
        std::unordered_map<int, BoolAffineFuncType> currentBooleanSequencePerQubit;
       
        // Initialize the affine function to k_i for all qubit wires.
        for (size_t i = 0; i < in_nQbits; ++i) {
            // Note: the affine function has an extra k0 term.
            BoolAffineFuncType affineFunc(in_nQbits + 1, false);
            // Set the term associated with that qubit to 1 (true)
            affineFunc[i + 1] = true;
            currentBooleanSequencePerQubit[i] = std::move(affineFunc);
        }

        for (const auto& gate: in_subCircuit) {
            if (gate->name() == "X") {
                auto& curentBooleanSequence = currentBooleanSequencePerQubit[gate->bits()[0]];
                // flip the offset k0 term
                curentBooleanSequence[0] = !curentBooleanSequence[0];
            }
            
            if (gate->name() == "CNOT") {
                const auto& controlAffineFunc = currentBooleanSequencePerQubit[gate->bits()[0]];
                auto& targetAffineFunc = currentBooleanSequencePerQubit[gate->bits()[1]];
                targetAffineFunc = combineAffineFunc(controlAffineFunc, targetAffineFunc);    
            }

            if (gate->name() == "Rz") {
                auto& curentBooleanSequence = currentBooleanSequencePerQubit[gate->bits()[0]];
                // Save the expression
                m_affineFuncs[gate.get()] = curentBooleanSequence;
                m_affineFuncToGates[curentBooleanSequence].emplace_back(gate.get());
            }
        }
    }

    const BoolAffineFuncType& PhasePolynomialRep::getAffineFunctionOfRzGate(Instruction* in_gatePtr) const { 
        const auto iter = m_affineFuncs.find(in_gatePtr);
        if (iter !=  m_affineFuncs.cend()) {
            return iter->second;
        }
        else {
            return EMPTY_AFFINE_FUNC_LIST;
        }
    }
    
    const std::vector<Instruction*>& PhasePolynomialRep::getRzGatesWithAffineFunction(const BoolAffineFuncType& in_func) const { 
        const auto iter = m_affineFuncToGates.find(in_func);
        if (iter !=  m_affineFuncToGates.cend()) {
            return iter->second;
        }
        else {
            return EMPTY_GATE_LIST;
        }
    }

    bool PhasePolynomialRep::hasMergableGates() const {
        for (auto iter = m_affineFuncToGates.cbegin(); iter != m_affineFuncToGates.cend(); ++iter) {
            if (iter->second.size() > 1) {
                return true;
            }
        }

        return false;
    }
} // namespace xacc
} // namespace quantum