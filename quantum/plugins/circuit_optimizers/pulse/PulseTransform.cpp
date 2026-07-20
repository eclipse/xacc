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
        
        // Step 1: Retrieve the system dynamics from the Accelerator backend
        const std::string H0 = buffer->getInformation("static-H").as<std::string>();
        const std::vector<std::string> controlOps =  buffer->getInformation("control-H").as<std::vector<std::string>>();
        const std::vector<std::string> controlChannels =  buffer->getInformation("control-channels").as<std::vector<std::string>>();
        assert(controlOps.size() == controlChannels.size());
        const double backendDt = buffer->getInformation("dt").as<double>();
        const std::string hamJsonStr = buffer->getInformation("ham-json").as<std::string>();
        
        // Step 2: Compute the total unitary matrix
        const int dim = calcSysDimension(program);
        const Eigen::MatrixXcd targetUmat = GateFuser::fuseGates(program, dim);
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

        // These parameters are only required for analytical controls (e.g. GOAT)
        std::vector<std::string> controlParams;    
        std::vector<std::string> controlFuncs;    
        std::vector<double> initParams;    

        if (method == "GOAT")
        {
            if (options.keyExists<std::vector<std::string>>("control-params")) 
            {
                controlParams = options.get<std::vector<std::string>>("control-params");
            }
            else
            {
                xacc::error("Missing required field 'control-params'");
            }

            if (options.keyExists<std::vector<std::string>>("control-funcs")) 
            {
                controlFuncs = options.get<std::vector<std::string>>("control-funcs");
            }
            else
            {
                xacc::error("Missing required field 'control-funcs'");
            }

            if (options.keyExists<std::vector<double>>("initial-parameters")) 
            {
                initParams = options.get<std::vector<double>>("initial-parameters");
            }
            else
            {
                xacc::error("Missing required field 'initial-parameters'");
            }

            assert(controlOps.size() == controlFuncs.size());
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
            // Analytical control parameters
            // Note: these params are only required by GOAT
            // we just send on empty params in other cases. 
            // Control parameter (used in the control function)
            std::make_pair("control-params", controlParams),
            // Control funcs
            std::make_pair("control-funcs", controlFuncs),
            // Initial params
            std::make_pair("initial-parameters", initParams),
            //////////////////////////////////////////////////
            // Control Hamiltonian terms: i.e. channels
            std::make_pair("control-H", controlOps),
            // Static Hamiltonian
            std::make_pair("static-H", H0),
            // Max time
            std::make_pair("max-time", tMax),
            std::make_pair("dt", backendDt),
            std::make_pair("hamiltonian-json", hamJsonStr)
        };

        auto optimizer = xacc::getOptimizer("quantum-control", pulseOptimConfigs);
        // Perform pulse IR transformation
        const auto optimResult = optimizer->optimize();
        
        // If analytical control (e.g. GOAT), generate pulses based on optimized analytical form.
        if (method == "GOAT")
        {
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
        }
        // If "GRAPE" (or other time-series methods),
        // we already optimize the pulses as data arrays, 
        // hence just need to make pulses from those arrays.
        else
        {
            program->clear();
            const auto nbControls = controlOps.size();
            if (optimResult.second.size() % nbControls != 0)
            {
                xacc::error("FATAL: Invalid results from pulse optimizer!");
                return;
            }
            // We have flattened the pulses into one array, now de-mux into pulses.
            const int pulseLength = optimResult.second.size()/nbControls;
            // Add optimized pulses
            for (int pulseId = 0; pulseId < nbControls; ++pulseId)
            {
                const std::string pulse_name = method + "_Optim_Pulse_" + std::to_string(pulseId);          
                auto pulse = std::make_shared<xacc::quantum::Pulse>(pulse_name);
                pulse->setChannel(controlChannels[pulseId]);
                
                std::vector<std::vector<double>> samples;
                samples.reserve(pulseLength);

                for (int i = 0; i < pulseLength; ++i)
                {
                    const auto idx = i + pulseId*pulseLength;
                    const std::vector<double> sample { optimResult.second[idx] };
                    samples.emplace_back(sample);
                }

                pulse->setSamples(samples);
                program->addInstruction(pulse);
            }
        }

        // Debug:
        std::cout << "Transformed Composite: \n" << program->toString() << "\n";
    }
}
}
