#include "GateFusion.hpp"
#include <numeric>
namespace {
    const std::complex<double> I(0.0, 1.0);
    Eigen::MatrixXcd Xmat = [] {
      Eigen::MatrixXcd temp = Eigen::MatrixXcd::Zero(2, 2);
      temp << 0, 1, 1, 0;
      return temp;
    }();

    Eigen::MatrixXcd Ymat = [] {
      Eigen::MatrixXcd temp = Eigen::MatrixXcd::Zero(2, 2);
      temp << 0, -I, I, 0;
      return temp;
    }();

    Eigen::MatrixXcd Zmat = [] {
      Eigen::MatrixXcd temp = Eigen::MatrixXcd::Zero(2, 2);
      temp << 1, 0, 0, -1;
      return temp;
    }();
   
    Eigen::MatrixXcd getRnMat(double theta, const std::vector<double>& in_n) 
    {
        Eigen::MatrixXcd result(2, 2);
        result = std::cos(theta / 2) * Eigen::MatrixXcd::Identity(2, 2) -
                 I * std::sin(theta / 2) * (in_n[0] * Xmat + in_n[1] * Ymat + in_n[2] * Zmat);

        return result;
    }
    
    // Reverse the list of qubit index:
    // Note: the convention in Controlled-gate, e.g. CNOT, is having [ctrl, target] qubit list,
    // but that is the reverse of the bit index for regular matrix representation.
    std::vector<size_t> reverseBitIdx(const std::vector<size_t>& in_origBit) {
        auto copy = in_origBit;
        std::reverse(copy.begin(), copy.end());
        return copy;
    }
}
namespace xacc {
namespace quantum {
void GateFuser::initialize(const std::shared_ptr<xacc::CompositeInstruction>& in_program)
{
    m_gates.clear();
    InstructionIterator it(in_program);
    while (it.hasNext())
    {
        auto nextInst = it.next();
        if (nextInst->isEnabled())
        {
            nextInst->accept(this);
        }
    }
}

Eigen::MatrixXcd GateFuser::calcFusedGate(int in_dim) const
{
    const auto matSize = 1ULL << in_dim;
    Eigen::MatrixXcd resultMat = Eigen::MatrixXcd::Identity(matSize, matSize);
    // Qubit index list (0->(dim-1))
    std::vector<size_t> index_list(in_dim);
    std::iota(index_list.begin(), index_list.end(), 0);
    
    for (const auto& item : m_gates)
    {
        const auto& idx = item.bitIdx;     			      
        for (size_t k = 0; k < matSize; ++k)
        { 
            // loop over big matrix columns
            // check if column index satisfies control-mask
            // if not: leave it unchanged
            std::vector<std::complex<double>> oldColumn(matSize);
            for (size_t i = 0; i < matSize; ++i)
            {
                oldColumn[i] = resultMat(i, k);
            }

            for (size_t i = 0; i < matSize; ++i)
            {
                size_t local_i = 0;
                for (size_t l = 0; l < idx.size(); ++l)
                {
                    local_i |= ((i >> idx[l]) & 1) << l;
                }
                    
                std::complex<double> res = 0.;
                for (size_t j = 0; j < (1ULL<<idx.size()); ++j)
                {
                    size_t locIdx = i;
                    for (size_t l = 0; l < idx.size(); ++l)
                    {
                        if (((j >> l)&1) != ((i >> idx[l])&1))
                        {
                            locIdx ^= (1ULL << idx[l]);
                        }
                    }
                        
                    res += oldColumn[locIdx] * item.matrix(local_i, j);
                }

                resultMat(i, k) = res;
            }
        }
    }

    return resultMat;
}

void GateFuser::visit(Hadamard& h) 
{
    Eigen::MatrixXcd Hmat{ Eigen::MatrixXcd::Zero(2, 2) }; 
    Hmat << 1 / std::sqrt(2.), 1 / std::sqrt(2.), 1 / std::sqrt(2.), -1 / std::sqrt(2.); 
    m_gates.emplace_back(Hmat, h.bits());
}

void GateFuser::visit(X& x) 
{
    m_gates.emplace_back(Xmat, x.bits());
}

void GateFuser::visit(Y& y) 
{
    m_gates.emplace_back(Ymat, y.bits());
}

void GateFuser::visit(Z& z) 
{
    m_gates.emplace_back(Zmat, z.bits());
}

void GateFuser::visit(CNOT& cnot) 
{
    Eigen::MatrixXcd cnotMat{ Eigen::MatrixXcd::Zero(4, 4) };
    cnotMat << 1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 1,
               0, 0, 1, 0;
    m_gates.emplace_back(cnotMat, reverseBitIdx(cnot.bits()));
}

void GateFuser::visit(Rx& rx) 
{
    const double angle = InstructionParameterToDouble(rx.getParameter(0));
    m_gates.emplace_back(getRnMat(angle, {1, 0, 0}), rx.bits());
}

void GateFuser::visit(Ry& ry) 
{
    const double angle = InstructionParameterToDouble(ry.getParameter(0));
    m_gates.emplace_back(getRnMat(angle, {0, 1, 0}), ry.bits());
}


void GateFuser::visit(Rz& rz) 
{
    const double angle = InstructionParameterToDouble(rz.getParameter(0));
    m_gates.emplace_back(getRnMat(angle, {0, 0, 1}), rz.bits());
}

void GateFuser::visit(S& s) 
{
    Eigen::MatrixXcd sMat{ Eigen::MatrixXcd::Zero(2, 2) };
    sMat << 1, 0, 
            0, I;
    m_gates.emplace_back(sMat, s.bits());
}
void GateFuser::visit(Sdg& sdg) 
{
    Eigen::MatrixXcd sdgMat{ Eigen::MatrixXcd::Zero(2, 2) };
    sdgMat <<  1, 0, 
               0, -I;
    m_gates.emplace_back(sdgMat, sdg.bits());
}

void GateFuser::visit(T& t) 
{
    Eigen::MatrixXcd tMat{ Eigen::MatrixXcd::Zero(2, 2) };
    tMat << 1, 0, 
            0, std::exp(I*M_PI/4.0);
    m_gates.emplace_back(tMat, t.bits());
}

void GateFuser::visit(Tdg& tdg) 
{
    Eigen::MatrixXcd tdgMat{ Eigen::MatrixXcd::Zero(2, 2) };
    tdgMat <<   1, 0, 
                0, std::exp(-I*M_PI/4.0);
    m_gates.emplace_back(tdgMat, tdg.bits());
}

void GateFuser::visit(U& u) 
{
    const auto theta = InstructionParameterToDouble(u.getParameter(0));
    const auto phi = InstructionParameterToDouble(u.getParameter(1));
    const auto lambda = InstructionParameterToDouble(u.getParameter(2));
    Eigen::MatrixXcd uMat { Eigen::MatrixXcd::Zero(2, 2) };
    
    uMat << std::cos(theta / 2),                    -std::exp(I*lambda)*std::sin(theta / 2),
            std::exp(I*phi)*std::sin(theta / 2),    std::exp(I*(phi+lambda))*std::cos(theta/2);
    
    m_gates.emplace_back(uMat, u.bits());
}

void GateFuser::visit(CY& cy) 
{
    Eigen::MatrixXcd cyMat{ Eigen::MatrixXcd::Zero(4, 4) };
    cyMat <<    1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 0, -I,
                0, 0, I, 0;
    m_gates.emplace_back(cyMat, reverseBitIdx(cy.bits()));
}

void GateFuser::visit(CZ& cz) 
{
    Eigen::MatrixXcd czMat{ Eigen::MatrixXcd::Identity(4, 4) };
    czMat(3, 3) = -1;
    m_gates.emplace_back(czMat, reverseBitIdx(cz.bits()));
}

void GateFuser::visit(CRZ& crz) 
{
    const double angle = InstructionParameterToDouble(crz.getParameter(0));
    const auto rzMat = getRnMat(angle, {0, 0, 1});
    
    Eigen::MatrixXcd crzMat{ Eigen::MatrixXcd::Zero(4, 4) };
    crzMat <<  1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, rzMat(0, 0), rzMat(0, 1),
            0, 0, rzMat(1, 0), rzMat(1, 1);
    m_gates.emplace_back(crzMat, reverseBitIdx(crz.bits()));
}

void GateFuser::visit(CH& ch) 
{
    Eigen::MatrixXcd chMat{ Eigen::MatrixXcd::Zero(4, 4) };
    chMat <<    1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1 / std::sqrt(2.), 1 / std::sqrt(2.), 
                0, 0, 1 / std::sqrt(2.), -1 / std::sqrt(2.);
    m_gates.emplace_back(chMat, reverseBitIdx(ch.bits()));
}

void GateFuser::visit(Swap& s) 
{
    Eigen::MatrixXcd swapMat{ Eigen::MatrixXcd::Zero(4, 4) };
    swapMat << 1, 0, 0, 0,
               0, 0, 1, 0,
               0, 1, 0, 0,
               0, 0, 0, 1;
    m_gates.emplace_back(swapMat, s.bits());
}

void GateFuser::visit(CPhase& cphase) 
{
    Eigen::MatrixXcd cphaseMat{ Eigen::MatrixXcd::Identity(4, 4) };
    const double angle = InstructionParameterToDouble(cphase.getParameter(0));
    const std::complex<double> val = std::exp(I*angle);
    cphaseMat(3, 3) = val;
    m_gates.emplace_back(cphaseMat, reverseBitIdx(cphase.bits()));
}

void GateFuser::visit(Measure& measure) 
{
    xacc::error("Cannot fuse CompositeInstruction contains MEASURE!"); 
}

void GateFuser::visit(Identity& i) 
{
    // Nothing to do for Identity matrix 
    return;
}

void GateFuser::visit(IfStmt& ifStmt) 
{
    xacc::error("Unsupported!");
}
} // namespace quantum
} // namespace xacc