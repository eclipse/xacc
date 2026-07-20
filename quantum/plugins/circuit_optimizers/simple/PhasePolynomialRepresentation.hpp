#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

// A boolean affine function is a vector of ki's boolean coefficients.
using BoolAffineFuncType = std::vector<bool>;

namespace xacc { 
    class Instruction;    
namespace quantum {
// This is the helper to construct a *Phase Polynomial*
// representation of a sub-circuit consists of {CNOT, NOT, Rz} gates.
// This representation allows us to perform optimization of quantum circuits.
// In particular, Rz gates that can be merged/combined will be revealed in this representation.
// A phase polynominal expression is an affine function:
// f = k0 + k1q1 + k2q2 + ...; where ki is {0, 1} and + represents a boolean XOR.
// Each Rz gate in the sub-circuit will be associated with an affine function. 
class PhasePolynomialRep {
public:
    // The sub-circuit of {CNOT, NOT, Rz} gates 
    // that we want to create the PhasePolynomialRep.
    PhasePolynomialRep(int in_nQbits, const std::vector<std::shared_ptr<Instruction>>& in_subCircuit);
    // The affine function of a Rz gate is basically a boolean expression of qubits.
    // e.g. q1 XOR q2 (q1 + q2), NOT q1 (1 + q1), etc.
    const BoolAffineFuncType& getAffineFunctionOfRzGate(Instruction* in_gatePtr) const;     
    const std::vector<Instruction*>& getRzGatesWithAffineFunction(const BoolAffineFuncType& in_func) const;
    bool hasMergableGates() const;

    static const BoolAffineFuncType EMPTY_AFFINE_FUNC_LIST;
    static const std::vector<Instruction*> EMPTY_GATE_LIST;
private:
    int m_nbQubits;
    // Affine function associated with the Rz gate.
    std::unordered_map<Instruction*, BoolAffineFuncType> m_affineFuncs;
    std::unordered_map<BoolAffineFuncType, std::vector<Instruction*>> m_affineFuncToGates;
};
} // end namespace quantum
} // end namespace xacc
