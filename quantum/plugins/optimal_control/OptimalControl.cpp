#include "OptimalControl.hpp"

namespace xacc {

// Preliminary specs of the Optimal Control options:
// ***REQUIRE*** { "method" : string}
// The control method that we want to use.
// Available options: "GOAT"
// The followings are method-specific options
// ============= GOAT ==========================
// - Required: { "dimension" : int } : system dimension (number of qubits)
// 
// - Required: { "target-U" : string } : Pauli observable type string
//  e.g. "X", "X1Z2", etc.
//
// - Optional: { "static-H" : string} : Pauli observable observable type string
//  describing time-indedependent (static) Hamiltonian
//  e.g. 5.1234 Z1Z2
//
// - Required: { "control-params" : vector<string> } : list of control parameters
//  e.g. { "gamma1", "gamma2", ... }
//
// - Required: { "control-funcs" : vector<string> }  : control functions 
//  e.g. { "sin(gamma1*t)", "exp(gamma2^2)", ... }
//  We support most basic functions (ExprTk) and "t" is an implicitly known param (time)
//
// - Required: { "control-H" : vector<string> } : list of Hamiltonian terms corresponding to the control functions.
//  Note: "control-H" and "control-funcs" must have the *same* number of elements
//
//  and "control-params" must include all possible params that may appear in those "control-funcs".
//
// - Required: { "init-params" : vector<double> }: initial values for control parameters
//
// - Required: { "max-time" : double }: max control time horizon
//
// - Optional: { "optimizer" : string }: can be "ml-pack" or "default"
// =============================================
void ControlOptimizer::setOptions(const HeterogeneousMap& in_options)
{
    const auto fatalError = [](const std::string& in_fieldName){
        xacc::error("Fatal: '" + in_fieldName + "' field is required.");
    };

    if (!in_options.stringExists("method") || in_options.getString("method") != "GOAT") 
    {
        fatalError("method");
    }

    int dimension = 0;
    if (in_options.keyExists<int>("dimension")) 
    {
        dimension = in_options.get<int>("dimension");
    }
    else
    {
        fatalError("dimension");
    }

    std::string targetU;
    if (in_options.stringExists("target-U")) 
    {
        targetU = in_options.getString("target-U");
    }
    else
    {
        fatalError("target-U");
    }

    // H0 is optional
    std::string H0;
    if (in_options.stringExists("static-H")) 
    {
        H0 = in_options.getString("static-H");
    }

    std::vector<std::string> controlParams;    
    if (in_options.keyExists<std::vector<std::string>>("control-params")) 
    {
        controlParams = in_options.get<std::vector<std::string>>("control-params");
    }
    else
    {
        fatalError("control-params");
    }

    std::vector<std::string> controlFuncs;    
    if (in_options.keyExists<std::vector<std::string>>("control-funcs")) 
    {
        controlFuncs = in_options.get<std::vector<std::string>>("control-funcs");
    }
    else
    {
        fatalError("control-funcs");
    }

    std::vector<std::string> controlOps;    
    if (in_options.keyExists<std::vector<std::string>>("control-H")) 
    {
        controlOps = in_options.get<std::vector<std::string>>("control-H");
    }
    else
    {
        fatalError("control-H");
    }


    std::vector<double> initParams;    
    if (in_options.keyExists<std::vector<double>>("init-params")) 
    {
        initParams = in_options.get<std::vector<double>>("init-params");
    }
    else
    {
        fatalError("init-params");
    }

    double tMax = 0.0;
    if (in_options.keyExists<double>("max-time")) 
    {
        tMax = in_options.get<double>("max-time");
    }
    else
    {
        fatalError("max-time");
    }

    std::string optimizer;
    if (in_options.stringExists("optimizer")) 
    {
        optimizer = in_options.getString("optimizer");
    }

    // Validate options:
    if (targetU.empty())
    {
        xacc::error("No target Hamiltonian was specified.");
    }

    if (dimension < 1 || dimension > 10)
    {
        xacc::error("Invalid system dimension.");
    }

    if (controlFuncs.size() != controlOps.size())
    {
        xacc::error("The number of control functions must match the number of time-dependent Hamiltonian terms.");
    }

    if (controlFuncs.empty())
    {
        xacc::error("No control terms were specified.");
    }

    if (initParams.size() != controlParams.size())
    {
        xacc::error("The number of initial values must match the number control parameters.");
    }

    if (initParams.empty())
    {
        xacc::error("No control parameters were specified.");
    }

    if (tMax < 0.0)
    {
        xacc::error("Invalid max time parameter.");
    }

    if (!optimizer.empty() && optimizer != "default" && optimizer != "ml-pack")
    {
        xacc::error("Invalid optimizer.");
    }
    
    std::unique_ptr<IGradientStepper> gradientOptimizer = [](const std::string& in_optimizerName) -> std::unique_ptr<IGradientStepper> {
        if (in_optimizerName == "ml-pack") 
        {
            return std::make_unique<GOAT_PulseOptim::MLPackGradientStepper>();
        }
        
        return std::make_unique<GOAT_PulseOptim::DefaultGradientStepper>();
    }(optimizer);
   
    // We have a valid set of paramters here.
    m_hamiltonian = std::make_unique<GoatHamiltonian>();
    m_hamiltonian->construct(dimension, H0, controlOps, controlFuncs, controlParams);
    const std::complex<double> I(0.0, 1.0);
    const Matrix targetUmat = GOAT_PulseOptim::constructMatrixFromPauliString(targetU, dimension) * (-I);
    // All parameters have been validated: construct the GOAT pulse optimizer
    m_goatOptimizer = std::make_unique<GOAT_PulseOptim>(targetUmat, m_hamiltonian->hamiltonian, m_hamiltonian->dHda, initParams, tMax, nullptr, std::move(gradientOptimizer));
    m_initialized = true;
}

OptResult ControlOptimizer::optimize(OptFunction& function) 
{
    if (!m_initialized)
    {
        xacc::error("Fatal: Optimal Control Module must be initialized before use!");
    }
    const auto result = m_goatOptimizer->optimize();

    return std::make_pair(result.finalCost, result.optParams);
}
}
