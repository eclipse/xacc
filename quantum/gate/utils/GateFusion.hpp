#pragma once
#include <Eigen/Dense>
#include "xacc.hpp"
#include "AllGateVisitor.hpp"

// Implement a simple Gate fusion procedure to compute the total unitary matrix for a given circuit (composite instruction)
using namespace xacc;
using namespace xacc::quantum;
namespace xacc {
namespace quantum {
struct FusionGateItem
{
    FusionGateItem(const Eigen::MatrixXcd& in_gateMat, const std::vector<size_t>& in_qbits):
        matrix(in_gateMat),
        bitIdx(in_qbits)
        {}
    Eigen::MatrixXcd matrix;
    std::vector<size_t> bitIdx; 
};

class GateFuser : public AllGateVisitor
{
public:
    static Eigen::MatrixXcd fuseGates(const std::shared_ptr<xacc::CompositeInstruction> &in_program, int in_dim);

    // CTor
    GateFuser(const std::shared_ptr<xacc::CompositeInstruction> in_program, int in_dim);
    void visitGates();
    // Compute the fused gate matrix given the output dimension
    // Note: the dimension must be greater than all the qubit indices in the Composite 
    Eigen::MatrixXcd calcFusedGate() const;
    // Gate visitor Impl
    void visit(Hadamard& h) override;
    void visit(CNOT& cnot) override;
    void visit(Rz& rz) override;
    void visit(Ry& ry) override;
    void visit(Rx& rx) override;
    void visit(X& x) override;
    void visit(Y& y) override;
    void visit(Z& z) override;
    void visit(CY& cy) override;
    void visit(CZ& cz) override;
    void visit(Swap& s) override;
    void visit(CRZ& crz) override;
    void visit(CH& ch) override;
    void visit(S& s) override;
    void visit(Sdg& sdg) override;
    void visit(T& t) override;
    void visit(Tdg& tdg) override;
    void visit(CPhase& cphase) override;
    void visit(Measure& measure) override;
    void visit(Identity& i) override;
    void visit(U& u) override;
    void visit(IfStmt& ifStmt) override;
private:
    const std::shared_ptr<xacc::CompositeInstruction> &m_program;
    int m_dim;
    std::vector<FusionGateItem> m_gates;
};
} // namespace quantum
} // namespace xacc
