#pragma once

#include "AllGateVisitor.hpp"
#include "util_cpu.h"
#include "dmsim_cpu_omp.hpp"

using namespace xacc;
using namespace xacc::quantum;
using namespace DMSim;

namespace xacc{
namespace quantum{
class DmsimCpuVisitor : public AllGateVisitor, public OptionsProvider, public xacc::Cloneable<DmsimCpuVisitor> {
public:
    DmsimCpuVisitor(int nq) : m_sim(std::make_shared<Simulation>(nq, 1)) {
        m_qubits = nq;
    }
    void visit(CNOT& cx) {
        auto bits = cx.bits();
        std::size_t ctrl_bit = bits[0];
        std::size_t trgt_bit = bits[1];
        m_sim->append(Simulation::CX(ctrl_bit, trgt_bit));
    }
    void visit(CY& cy) {
        auto bits = cy.bits();
        std::size_t ctrl_bit = bits[0];
        std::size_t trgt_bit = bits[1];
        m_sim->append(Simulation::CY(ctrl_bit, trgt_bit));
    }
    void visit(CZ& cz) {
        auto bits = cz.bits();
        std::size_t ctrl_bit = bits[0];
        std::size_t trgt_bit = bits[1];
        m_sim->append(Simulation::CZ(ctrl_bit, trgt_bit));
    }
    void visit(Hadamard& h) {
        auto bits = h.bits(); 
        auto h_bit = bits[0];
        m_sim->append(Simulation::H(h_bit));
    }
    void visit(X& x) {
        auto bits = x.bits();
        auto x_bit = bits[0];
        m_sim->append(Simulation::X(x_bit));
    }
    void visit(Y& y) {
        auto bits = y.bits();
        auto y_bit = bits[0];
        m_sim->append(Simulation::Y(y_bit));
    }
    void visit(Z& z) {
        auto bits = z.bits();
        auto z_bit = bits[0];
        m_sim->append(Simulation::Z(z_bit));
    }
    void visit(Rz& rz) {
        auto bits = rz.bits();
        auto rz_bit = bits[0];
        const auto angleTheta = InstructionParameterToDouble(rz.getParameter(0));
        m_sim->append(Simulation::RZ(angleTheta, rz_bit));
    }
    void visit(Rx& rx) {
        auto bits = rx.bits();
        auto rx_bit = bits[0];
        const auto angleTheta = InstructionParameterToDouble(rx.getParameter(0));
        m_sim->append(Simulation::RX(angleTheta, rx_bit));
    }
    void visit(Ry& ry) {
        auto bits = ry.bits();
        auto ry_bit = bits[0];
        const auto angleTheta = InstructionParameterToDouble(ry.getParameter(0));
        m_sim->append(Simulation::RY(angleTheta, ry_bit));
    }
    void visit(Swap& s)
    {
        m_sim->append(Simulation::SWAP(s.bits()[0], s.bits()[1]));
    }
    void visit(CRZ& crz)
    {
        const auto angleTheta = InstructionParameterToDouble(crz.getParameter(0));
        m_sim->append(Simulation::CRZ(angleTheta, crz.bits()[0], crz.bits()[1]));
    }
    void visit(CH& ch)
    {
        m_sim->append(Simulation::CH(ch.bits()[0], ch.bits()[1]));
    }
    void visit(S& s)
    {
        m_sim->append(Simulation::S(s.bits()[0]));
    }
    void visit(T& t)
    {
        m_sim->append(Simulation::T(t.bits()[0]));
    }
    void visit(Tdg& tdg)
    {
        m_sim->append(Simulation::TDG(tdg.bits()[0]));
    }  
    void visit(U& u)
    {
        const auto theta = InstructionParameterToDouble(u.getParameter(0));
        const auto phi = InstructionParameterToDouble(u.getParameter(1));
        const auto lambda = InstructionParameterToDouble(u.getParameter(2));
        m_sim->append(Simulation::U3(u.bits()[0], theta, phi, lambda));
    }


    void execute(std::shared_ptr<AcceleratorBuffer> buffer, int m_shots) {
        // Upload to GPU to be ready for execution
        m_sim->upload();
        // Run the simulation
        m_sim->sim();
        // Measurements
        auto* measurement = m_sim->measure(m_shots);
        // Persist Measurement Results to Buffer
        for (int i=0; i<m_shots; i++) {
            std::stringstream bitString;
            for (int j=m_qubits-1; j>=0; j--) {
                auto bit_int = ((measurement[i]>>j)&1);
                std::string bit = to_string(bit_int);
                bitString << bit;
            }
            buffer->appendMeasurement(bitString.str());
        }
        buffer->print(std::cout);
    } 

    virtual std::shared_ptr<DmsimCpuVisitor> clone() { return std::make_shared<DmsimCpuVisitor>(m_qubits); }

private:
    // std::shared_ptr<AcceleratorBuffer> m_buffer;
    std::shared_ptr<Simulation> m_sim;
    int m_qubits;
    // int m_ncpus;
};
} // namespace quantum
} // namespace xacc