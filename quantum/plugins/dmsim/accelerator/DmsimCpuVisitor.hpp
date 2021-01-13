#pragma once

#include "AllGateVisitor.hpp"
#include "util_cpu.h"
#include "dmsim_cpu_omp.hpp"

using namespace xacc;
using namespace xacc::quantum;
using namespace DMSim;

namespace xacc{
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
    void visit(Hadamard& h) {
        auto bits = h.bits(); 
        auto h_bit = bits[0];
        m_sim->append(Simulation::H(h_bit));
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
}