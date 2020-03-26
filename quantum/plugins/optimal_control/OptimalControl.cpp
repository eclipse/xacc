#include "OptimalControl.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace xacc {

// Preliminary specs of the Optimal Control options:
// ***REQUIRE*** { "method" : string}
// The control method that we want to use.
// Available options: "GOAT", "GRAPE", etc.
// =============================================
void ControlOptimizer::setOptions(const HeterogeneousMap& in_options)
{
    if (!in_options.stringExists("method")) 
    {
        xacc::error("Please specify a 'method' for quantum optimal control!\n");
        return; 
    }

    // This will throw if the method is not supported.
    m_pulseOptim = xacc::getOptimizer(in_options.getString("method"));
    m_pulseOptim->setOptions(in_options);
}

OptResult ControlOptimizer::optimize()
{
    return m_pulseOptim->optimize();
}

OptResult ControlOptimizer::optimize(OptFunction& function) 
{
   xacc::warning("OptFunction is not required for quantum optimal control. It will be ignored.\n");
   return optimize();
}
}
