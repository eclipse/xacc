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
        
    auto gateName = ctx->gatename->getText();
    if (ctx->explist()->exp().size() > 0) {
        auto paramStr = ctx->explist()->exp(0);
        std::vector<int> qubits;
        std::vector<InstructionParameter> params;
        for (int i = 0; i < ctx->explist()->exp().size(); i++) {
            if (i == 0 && (ctx->explist()->exp(i)->real() != nullptr || 
                            ctx->explist()->exp(i)->id() != nullptr)) {
                // we have a parameter for a parameterized gate
                auto str = ctx->explist()->exp(i)->getText();
                auto param = is_double(str) ? InstructionParameter(std::stod(str)) : InstructionParameter(str);
                params.push_back(param);
            } else {
                qubits.push_back(std::stoi(ctx->explist()->exp(i)->INT()->getText()));
            }
        }

        std::shared_ptr<Instruction> gate;
        if (gateName == "Measure") {
            std::vector<int> measurebit {qubits[0]};
            InstructionParameter p1(qubits[1]);
            gate = provider->createInstruction("Measure", measurebit);
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


// void DWQMIListener::enterAnnealdecl(DWQMIParser::AnnealdeclContext * ctx) {
//     if (!foundAnneal) {

//         std::vector<InstructionParameter> params;
//         auto taStr = ctx->ta()->getText();
//         auto tpStr = ctx->tp()->getText();
//         auto tqStr = ctx->tq()->getText();
        
//         auto is_double = [](const std::string& s) -> bool
// 	    {
// 	        try {
// 		        std::stod(s);
//     	    } catch(std::exception& e) {
// 	    	    return false;
// 	        }
//     	    return true;
// 	    };
    
//         auto taparam = is_double(taStr) ? InstructionParameter(std::stod(taStr)) : InstructionParameter(taStr);
//         auto tpparam = is_double(tpStr) ? InstructionParameter(std::stod(tpStr)) : InstructionParameter(tpStr);
//         auto tqparam = is_double(tqStr) ? InstructionParameter(std::stod(tqStr)) : InstructionParameter(tqStr);

//         if(taparam.which() == 3 && !contains(functionVarNames, boost::get<std::string>(taparam)))
//             xacc::error(boost::get<std::string>(taparam) + " is an invalid kernel parameter (does not exist in kernel arg list)");
 
//         if(tpparam.which() == 3 && !contains(functionVarNames, boost::get<std::string>(tpparam)))
//             xacc::error(boost::get<std::string>(tpparam) + " is an invalid kernel parameter (does not exist in kernel arg list)");
 
//         if(tqparam.which() == 3 && !contains(functionVarNames, boost::get<std::string>(tqparam)))
//             xacc::error(boost::get<std::string>(tqparam) + " is an invalid kernel parameter (does not exist in kernel arg list)");
            
//         std::string direction = "forward";
//         if (ctx->direction() != nullptr) {
//             std::cout << ctx->direction()->getText() << "\n";
//             if (ctx->direction()->forward() == nullptr) {
//                 direction = "reverse";
//             } 
//         }

//         auto anneal = std::make_shared<Anneal>(taparam, tpparam, tqparam, InstructionParameter(direction));
//         f->addInstruction(anneal);
//         foundAnneal = true;
//     } else {
//         xacc::error("Error - You can only provide one anneal instruction.");
//     }
//     return;
// }
  
// void DWQMIListener::enterInst(dwqmi::DWQMIParser::InstContext *ctx) {

//     int bit1, bit2;
    
// 	auto is_double = [](const std::string& s) -> bool
// 	{
// 	    try {
// 		    std::stod(s);
// 	    } catch(std::exception& e) {
// 		    return false;
// 	    }
// 	    return true;
// 	};

//     try {
//         bit1 = std::stoi(ctx->INT(0)->getText());
//         bit2 = std::stoi(ctx->INT(1)->getText());
//     } catch(std::exception& e) {
//         xacc::error("Invalid qubit indices: " + ctx->getText());
//     }

//     if (bit1 > maxBitIdx) maxBitIdx = bit1;
//     if (bit2 > maxBitIdx) maxBitIdx = bit2;

//     std::string valStr;
//     if (ctx->real() == nullptr) {
//         valStr = ctx->id()->getText();
//     } else {
//         valStr = ctx->real()->getText();
//     }
//     auto param = is_double(valStr) ? InstructionParameter(std::stod(valStr)) : InstructionParameter(valStr);
    
//     if(param.which() == 3 && !contains(functionVarNames, boost::get<std::string>(param)))
//         xacc::error(boost::get<std::string>(param) + " is an invalid kernel parameter (does not exist in kernel arg list)");


// 	auto instruction = std::make_shared<DWQMI>(std::stoi(ctx->INT(0)->getText()), 
//                                                 std::stoi(ctx->INT(1)->getText()), 
//                                                 param);
        
//     f->addInstruction(instruction);
//     return;
// }

}
}
