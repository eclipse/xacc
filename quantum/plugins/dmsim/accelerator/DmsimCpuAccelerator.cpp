#include "DmsimCpuAccelerator.hpp"
#include "xacc_plugin.hpp"

namespace xacc{

void DmsimCpuAccelerator::initialize(const HeterogeneousMap &params){
    // m_sim = sim(10,8); // hard-coding 10 qubits, 8 CPU's for now. (m_nqubits, m_ncpus)
   //  m_visitor = std::make_shared<DmsimCpuVisitor>();
    m_shots = 1024;
    // if (params.keyExists<int>("shots"))
    // {
    //     m_shots = params.get<int>("shots");
    //     if (m_shots < 1)
    //     {
    //         xacc::error("Invalid 'shots' parameter.");
    //     }
    // }
    // if (params.keyExists<int>("qubits"))
    // {
    //     m_nqubits = params.get<int>("qubits");
    // }
    // if (params.keyExists<int>("ncpus"))
    // {
    //     m_ncpus = params.get<int>("ncpus");
    // }
}

void DmsimCpuAccelerator::updateConfiguration(const HeterogeneousMap &config){
    std::cout << "Pass\n";
}

const std::vector<std::string> DmsimCpuAccelerator::configurationKeys() {
    return std::vector<string> {};
}

HeterogeneousMap DmsimCpuAccelerator::getProperties() {
    HeterogeneousMap m;
    return m;
}

void DmsimCpuAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> CompositeInstruction) {
    //
}

void DmsimCpuAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> CompositeInstruction) {
    
    auto visitor = std::make_shared<DmsimCpuVisitor>(buffer->size()); 
    InstructionIterator iter(CompositeInstruction);
    while(iter.hasNext()) {
    auto instruction = iter.next();
    if (!instruction->isComposite()) {
        instruction->accept(visitor);
    }
    }
    visitor->execute(buffer, m_shots);
}

} // namespace xacc
REGISTER_PLUGIN(xacc::DmsimCpuAccelerator, xacc::Accelerator)