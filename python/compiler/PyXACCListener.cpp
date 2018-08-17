/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <iostream>
#include <IRProvider.hpp>
#include "PyXACCListener.hpp"
#include "XACC.hpp"
#include "GateFunction.hpp"
#include "IRGenerator.hpp"

using namespace pyxacc;

namespace xacc {
namespace quantum {

PyXACCListener::PyXACCListener() {
    provider = xacc::getService<IRProvider>("gate");
}

std::shared_ptr<Function> PyXACCListener::getKernel() {
	return f;
}

void PyXACCListener::enterXacckernel(PyXACCIRParser::XacckernelContext * ctx) {
    std::vector<InstructionParameter> params;
    for (int i = 0; i < ctx->param().size(); i++) {
        params.push_back(InstructionParameter(ctx->param(static_cast<size_t>(i))->getText()));
        functionVariableNames.push_back(ctx->param(static_cast<size_t>(i))->getText());
    }
    f = provider->createFunction(ctx->kernelname->getText(), {}, params);
}

void PyXACCListener::enterUop(PyXACCIRParser::UopContext * ctx) {
     auto is_double = [](const std::string& s) -> bool
	    {
	        try {
		        std::stod(s);
    	    } catch(std::exception& e) {
	    	    return false;
	        }
    	    return true;
	    };
      auto is_int = [](const std::string& s) -> bool
	    {
	        try {
		        std::stoi(s);
    	    } catch(std::exception& e) {
	    	    return false;
	        }
    	    return true;
	    };
       
    auto gateName = ctx->gatename->getText();
    boost::trim(gateName);
    if (gateName == "CX") {gateName = "CNOT";}
    else if (gateName == "MEASURE") {gateName = "Measure";}
    
    if (gateName == "xacc") {

        auto generatorName = ctx->explist()->exp(0)->getText();
        std::map<std::string, InstructionParameter> params;
        for (int i = 1; i < ctx->explist()->exp().size(); i++) {
            auto key = ctx->explist()->exp(i)->id()->getText();
            auto valStr = ctx->explist()->exp(i)->exp(0)->getText();
            if (is_int(valStr)) {
                params.insert({key, InstructionParameter(std::stoi(valStr))});
            } else if (is_double(valStr)) {
                params.insert({key, InstructionParameter(std::stod(valStr))});
            } else {
                params.insert({key, InstructionParameter(valStr)});
            }
        }

        // Add the Function's InstructionParameters
        for (int i = 0; i < f->nParameters(); i++) {
            params.insert({"param_"+std::to_string(i),f->getParameter(i)});
        }
        
        auto generator = xacc::getService<xacc::IRGenerator>(generatorName);
        auto genF = generator->generate(params);
        
        for (auto i : genF->getInstructions()) {
            f->addInstruction(i);
        }
        
    } else if (ctx->explist()->exp().size() > 0) {
        auto paramStr = ctx->explist()->exp(0);
        std::vector<int> qubits;
        std::vector<InstructionParameter> params;
        for (int i = 0; i < ctx->explist()->exp().size(); i++) {
            // First arg could be a real value (3.14), a string (id), or a general expression (0.5*t0)
            if (i == 0 && (ctx->explist()->exp(i)->real() != nullptr || 
                            ctx->explist()->exp(i)->id() != nullptr ||
                            !ctx->explist()->exp(i)->exp().empty())) {
                // we have a parameter for a parameterized gate
                auto tmp = ctx->explist()->exp(i);
                if (tmp->real() != nullptr || tmp->id() != nullptr) {
                    auto str = ctx->explist()->exp(i)->getText();
                    auto param = is_double(str) ? InstructionParameter(std::stod(str)) : InstructionParameter(str);
                    params.push_back(param);
                } else {
                    params.push_back(InstructionParameter(tmp->getText()));
                }
            } else {
                qubits.push_back(std::stoi(ctx->explist()->exp(i)->INT()->getText()));
            }
        }

        std::shared_ptr<Instruction> gate;
        if (gateName == "Measure") {
            std::vector<int> measurebit {qubits[0]};
            InstructionParameter p1(qubits[1]);
            gate = provider->createInstruction(gateName, measurebit);
            gate->setParameter(0, p1);
        } else {   

            gate = provider->createInstruction(gateName, qubits);
            int count = 0;
            for (auto p : params) {
                gate->setParameter(count, p);
                count++;
            }
        }

        f->addInstruction(gate);

    } else {
        xacc::error("Only permitting gates with 1 parameter for now.");
    }
    
}

}
}
