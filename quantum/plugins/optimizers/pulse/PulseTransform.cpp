#include "PulseTransform.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {
    void PulseTransform::apply(std::shared_ptr<CompositeInstruction> program, 
                                const std::shared_ptr<Accelerator> accelerator,
                                const HeterogeneousMap& options)
    {
        std::cout << "HOWDY: Pulse IR Transform!\n";
        if (program == nullptr || accelerator == nullptr)
        {
            xacc::error("Pulse IR transformation requires a composite instruction and a pulse-capable backend accelerator.");
            return;
        }
        // Note: this is currently only compatible with QuaC accelerator
        // Allocate a dummy buffer to retrieve the system dynamics that is needed
        // for the pulse IR transform.
        auto buffer = xacc::qalloc(1);
        buffer->addExtraInfo("ir-transform", true);
        accelerator->execute(buffer, nullptr);
        // Debug
        buffer->print();
    }
}
}