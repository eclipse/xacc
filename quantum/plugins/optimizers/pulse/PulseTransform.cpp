#include "PulseTransform.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "GateFusion.hpp"
#include <math.h> 
#include "exprtk.hpp"
#include "Pulse.hpp"

using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

namespace {
    int calcSysDimension(std::shared_ptr<CompositeInstruction> in_program)
    {
        int result = 1;
        // Walk the IR tree, and visit each node
        InstructionIterator it(in_program);
        while (it.hasNext())
        {
            auto nextInst = it.next();
            for (const auto& bitIdx : nextInst->bits())
            {
                if (bitIdx + 1 > result)
                {
                    result = bitIdx + 1;
                }
            }
        }

        return result;
    }

    std::vector<std::vector<double>> pulseFunc(const std::string& in_functionString, double in_tMax, double in_dt)
    {
        std::vector<std::vector<double>> result;
        expression_t expression;
        parser_t parser;
        symbol_table_t symbol_table;
        double g_time = 0.0;
        symbol_table.add_variable("t", g_time);
        expression.register_symbol_table(symbol_table);
        parser.compile(in_functionString, expression);
        
        while (g_time < in_tMax)
        {
            const std::complex<double> exprVal = expression.value();
            result.emplace_back(std::vector<double>{exprVal.real(), exprVal.imag()});
            g_time += in_dt;
        }
        return result;
    }
}

namespace xacc {
namespace quantum {
    // Note: the input options param include:
    // (1) Method of optimization ('method': string)
    // currently, only support 'GOAT'
    // (2) 'control-params': list of all control parameters to be optimized 
    // (3) 'initial-parameters': initial values for those params
    // (4) 'control-funcs': list of control functions
    // This must match the number and order of channels available.
    // Uncontrolled channels can be left as empty strings.
    // (5) 'max-time': time duration for the pulse optimizer when doing pulse optimization.
    void PulseTransform::apply(std::shared_ptr<CompositeInstruction> program, 
                                const std::shared_ptr<Accelerator> accelerator,
                                const HeterogeneousMap& options)
    {
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
        // Step 1: Retrieve the system dynamics from the Accelerator backend
        const std::string H0 = buffer->getInformation("static-H").as<std::string>();
        const std::vector<std::string> controlOps =  buffer->getInformation("control-H").as<std::vector<std::string>>();
        const std::vector<std::string> controlChannels =  buffer->getInformation("control-channels").as<std::vector<std::string>>();
        assert(controlOps.size() == controlChannels.size());
        const double backendDt = buffer->getInformation("dt").as<double>();
        
        // Step 2: Compute the total unitary matrix
        auto fuser = xacc::getService<GateFuser>("default");
        fuser->initialize(program);
        const int dim = calcSysDimension(program);
        const Eigen::MatrixXcd targetUmat = fuser->calcFusedGate(dim);
        std::cout << "\nTarget Unitary matrix: \n" << targetUmat << "\n";

        // Step 3: Get the required params
        std::string method;
        if (options.stringExists("method")) 
        {
            method = options.getString("method");
        }
        else
        {
            return xacc::error("Missing required field 'method'");
        }

        std::vector<std::string> controlParams;    
        if (options.keyExists<std::vector<std::string>>("control-params")) 
        {
            controlParams = options.get<std::vector<std::string>>("control-params");
        }
        else
        {
            xacc::error("Missing required field 'control-params'");
        }

        std::vector<std::string> controlFuncs;    
        if (options.keyExists<std::vector<std::string>>("control-funcs")) 
        {
            controlFuncs = options.get<std::vector<std::string>>("control-funcs");
        }
        else
        {
            xacc::error("Missing required field 'control-funcs'");
        }

        std::vector<double> initParams;    
        if (options.keyExists<std::vector<double>>("initial-parameters")) 
        {
            initParams = options.get<std::vector<double>>("initial-parameters");
        }
        else
        {
            xacc::error("Missing required field 'initial-parameters'");
        }

        double tMax = 0.0;
        if (options.keyExists<double>("max-time")) 
        {
            tMax = options.get<double>("max-time");
        }
        else
        {
            return xacc::error("Missing required field 'max-time'");
        }

        assert(controlOps.size() == controlFuncs.size());

        std::cout << "Perform pulse optimization:\n";
        
        // Step 4: contruct the optimization configs for the Quantum Optimal Control plugin:
        // Passing Eigen matrix around (b/w different xacc plugins) via Heterogenous map can be
        // problematic (e.g. templated type in different compilation units).
        // Hence, for safety, we just pass a flatten std::vector here.
        std::vector<std::complex<double>> flattenUMat(targetUmat.size());
        for (int i = 0; i < targetUmat.rows(); ++i)
        {
            for (int j = 0; j < targetUmat.cols(); ++j)
            {
                const int idx = i * targetUmat.rows() + j;
                flattenUMat[idx] = targetUmat(i, j);
            }
        }
        
        xacc::HeterogeneousMap pulseOptimConfigs {
            std::make_pair("method", method),
            std::make_pair("dimension", dim),
            // Target unitary: the one calculated from the CompositeInstruction
            std::make_pair("target-U", flattenUMat),
            // Control parameter (used in the control function)
            std::make_pair("control-params", controlParams),
            // Control funcs
            std::make_pair("control-funcs", controlFuncs),
            // Control Hamiltonian terms: i.e. channels
            std::make_pair("control-H", controlOps),
            // Static Hamiltonian
            std::make_pair("static-H", H0),
            // Initial params
            std::make_pair("initial-parameters", initParams),
            // Max time
            std::make_pair("max-time", tMax)
        };

        auto optimizer = xacc::getOptimizer("quantum-control", pulseOptimConfigs);
        // Perform pulse IR transformation
        const auto optimResult = optimizer->optimize();
        
        // Step 5: construct the *analytical* form of optimal control functions
        // i.e. replace each control-params variable with its optimal value
        std::vector<std::string> resolvedFuncs;
            
        const auto replaceAllVarOccurrences = [](const std::string& in_func, 
                                                    const std::string& in_varName, 
                                                    double in_varVal)->std::string {
            size_t pos = 0;
            std::string resultStr = in_func;
            while ((pos = resultStr.find(in_varName, pos)) != std::string::npos) 
            {
                const std::string replace = std::to_string(in_varVal);
                resultStr.replace(pos, in_varName.length(), replace);
                pos += replace.length();
            }
            return resultStr;
        };

        for (const auto& ctrlFn : controlFuncs)
        {
            assert(optimResult.second.size() == controlParams.size());
            std::string evaledFunc = ctrlFn;
            // Replace all variables
            for (size_t i = 0; i < controlParams.size(); ++i)
            {
                evaledFunc = replaceAllVarOccurrences(evaledFunc, controlParams[i], optimResult.second[i]);
            }

            std::cout << "Evaled control function: " << evaledFunc << "\n";
            resolvedFuncs.emplace_back(evaledFunc);
        }

        // Step 6: Transform the input composite
        // Clear all instructions (gates)
        program->clear();
        int id = 0;
        // Add optimized pulses
        for (const auto& pulseFn : resolvedFuncs)
        {
            std::cout << "Add pulse function: " << pulseFn << "\n";
            const std::string pulse_name = "Optim_Pulse_" + std::to_string(id);          
            auto pulse = std::make_shared<xacc::quantum::Pulse>(pulse_name);
            pulse->setChannel(controlChannels[id]);
            pulse->setSamples(pulseFunc(pulseFn, tMax, backendDt));
            program->addInstruction(pulse);
            ++id;
        }

        // Debug:
        std::cout << "Transformed Composite: \n" << program->toString() << "\n";
    }
}
}