#include "GateFusion.hpp"
namespace {
    const std::complex<double> I(0.0, 1.0);
    Eigen::MatrixXcd Xmat{ Eigen::MatrixXcd::Zero(2, 2) }; 
    Eigen::MatrixXcd Ymat{ Eigen::MatrixXcd::Zero(2, 2) }; 
    Eigen::MatrixXcd Zmat{ Eigen::MatrixXcd::Zero(2, 2) }; 
    
    void initStaticMat()
    {
        static bool wasInit = false;
        if (!wasInit)
        {
            Xmat << 0, 1, 1, 0;
            Ymat << 0, -I, I, 0;
            Zmat << 1, 0, 0, -1;
            wasInit = true;
        }
    }
   
    Eigen::MatrixXcd getRnMat(double theta, const std::vector<double>& in_n) 
    {
        initStaticMat();
        Eigen::MatrixXcd result(2, 2);
        result = std::cos(theta / 2) * Eigen::MatrixXcd::Identity(2, 2) -
                 I * std::sin(theta / 2) * (in_n[0] * Xmat + in_n[1] * Ymat + in_n[2] * Zmat);

        return result;
    }
}

void GateFuser::initialize(const std::shared_ptr<xacc::CompositeInstruction>& in_program)
{
    initStaticMat();
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
    
    std::vector<size_t> index_list;
    for (int i = 0; i < in_dim; ++i)
    {
        index_list.push_back(i);
    }	

    for (auto& item : m_gates)
    {
        const auto& idx = item.bitIdx;
        std::vector<size_t> idx2mat(idx.size());
        for (size_t i = 0; i < idx.size(); ++i)
        {
            idx2mat[i] = static_cast<size_t>(
                ((std::equal_range(index_list.begin(), index_list.end(), idx[i])).first - index_list.begin()));
        }
			      
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
                    local_i |= ((i >> idx2mat[l]) & 1) << l;
                }
                    
                    
                std::complex<double> res = 0.;
                for (size_t j = 0; j < (1ULL<<idx.size()); ++j)
                {
                    size_t locIdx = i;
                    for (size_t l = 0; l < idx.size(); ++l)
                    {
                        if (((j >> l)&1) != ((i >> idx2mat[l])&1))
                        {
                            locIdx ^= (1ULL << idx2mat[l]);
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
    m_gates.emplace_back(cnotMat, cnot.bits());
}
void GateFuser::visit(Rz& rz) 
{
    //TODO
}
void GateFuser::visit(Ry& ry) 
{
    //TODO
}
void GateFuser::visit(Rx& rx) 
{
    //TODO

}

void GateFuser::visit(CY& cy) 
{
    //TODO

}
void GateFuser::visit(CZ& cz) 
{
    //TODO

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
void GateFuser::visit(CRZ& crz) 
{
    //TODO

}
void GateFuser::visit(CH& ch) 
{
    //TODO

}
void GateFuser::visit(S& s) 
{
    //TODO

}
void GateFuser::visit(Sdg& sdg) 
{
    //TODO

}
void GateFuser::visit(T& t) 
{
    //TODO

}
void GateFuser::visit(Tdg& tdg) 
{
    //TODO

}
void GateFuser::visit(CPhase& cphase) 
{
    Eigen::MatrixXcd cphaseMat{ Eigen::MatrixXcd::Identity(4, 4) };
    const double angle = InstructionParameterToDouble(cphase.getParameter(0));
    const std::complex<double> val = std::exp(I*angle);
    cphaseMat(3, 3) = val;
    m_gates.emplace_back(cphaseMat, cphase.bits());
}
void GateFuser::visit(Measure& measure) 
{
    //TODO

}
void GateFuser::visit(Identity& i) 
{
    //TODO

}
void GateFuser::visit(U& u) 
{
    //TODO

}
void GateFuser::visit(IfStmt& ifStmt) 
{
    xacc::error("Unsupported!");
}