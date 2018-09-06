#include "CircuitOptimizer.hpp"
#include "GateIR.hpp"
#include "GateFunction.hpp"
#include "GraphProvider.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "CNOT.hpp"
#include "Rz.hpp"
namespace xacc {
namespace quantum {

std::shared_ptr<IR> CircuitOptimizer::transform(std::shared_ptr<IR> ir) {

	if (!xacc::optionExists("circuit-opt-silent")) {
        xacc::info("Executing XACC Circuit Optimizer.");
    }

	std::shared_ptr<IRProvider> irProvider = xacc::getService<IRProvider>("gate");
    auto gateir = std::dynamic_pointer_cast<GateIR>(ir);
    if (!gateir) xacc::error("Invalid IR instance passed to Circuit Optimizer, must be gate.");
    
    auto isRotation = [](const std::string inst) {
        return inst == "Rz" || inst == "Ry" || inst == "Rx";
    };

    auto ipToDouble = [](xacc::InstructionParameter p) {
        if (p.which() == 0) {
            return (double) boost::get<int>(p);
        } else if (p.which() == 1) {
            return boost::get<double>(p);
        } else if (p.which() == 2) {
            return (double) boost::get<float>(p);
        } else {
            std::stringstream s;
            s << p;
            xacc::error("CircuitOptimizer: invalid gate parameter " + std::to_string(p.which()) + ", " + s.str());
        }
        return 0.0;
    };
    
    int nTries = 2;
    
    for (auto& k : gateir->getKernels()) {
        for (int j = 0; j < nTries; ++j) {     
        auto gateFunction = std::dynamic_pointer_cast<GateFunction>(k);
        
        // Remove any zero rotations (Rz(theta<1e-12))
        for (int i = 0; i < gateFunction->nInstructions(); i++) {
            auto inst = gateFunction->getInstruction(i);
            if (isRotation(inst->name())) {
                auto param = inst->getParameter(0);
                double val = ipToDouble(param);
 
                if (std::fabs(val) < 1e-12) {
                    inst->disable();
                }
            }
        }

        // Remove all CNOT(p,q) CNOT(p,q) Pairs
        while(true) {
            bool modified = false;
            gateFunction = std::dynamic_pointer_cast<GateFunction>(gateFunction->enabledView());
            auto graphView = gateFunction->toGraph();
            for (int i = 1; i < graphView.order()-2; i++) {
                auto node = graphView.getVertex(i);
                if (node.name() == "CNOT" 
                        && gateFunction->getInstruction(node.id()-1)->isEnabled()) {
                    auto neighbors = graphView.getNeighborList(node.id());
                    std::vector<int> nAsVec(neighbors.begin(), neighbors.end());
                    
                    if (nAsVec[0] == nAsVec[1]) {
                        gateFunction->getInstruction(node.id()-1)->disable();
                        gateFunction->getInstruction(nAsVec[0]-1)->disable();
                        modified = true;
                        break;
                    }
                }
            }
            
            if (!modified) break;
        }

        while(true) {
            bool modified = false;
            gateFunction = std::dynamic_pointer_cast<GateFunction>(gateFunction->enabledView());
            auto graphView = gateFunction->toGraph();
            
            for (int i = 1; i < graphView.order()-2; ++i) {
                auto node = graphView.getVertex(i);

                auto adj = graphView.getNeighborList(node.id());
                std::vector<int> nAsVec(adj.begin(), adj.end());

                if (adj.size() == 1) {
                    auto nextNode = graphView.getVertex(nAsVec[0]);
                    if (node.name() == "H" && nextNode.name() == "H") {
                        gateFunction->getInstruction(node.id()-1)->disable();
                        gateFunction->getInstruction(nAsVec[0]-1)->disable();
                        modified = true;
                        break;
                    } else if (isRotation(node.name()) && isRotation(nextNode.name())
                                && node.name() == nextNode.name()) {
                        auto val1 = ipToDouble(gateFunction->getInstruction(node.id()-1)->getParameter(0));
                        auto val2 = ipToDouble(gateFunction->getInstruction(nextNode.id()-1)->getParameter(0));

                        if (std::fabs(val1+val2) < 1e-12) {
                            gateFunction->getInstruction(node.id()-1)->disable();
                            gateFunction->getInstruction(nextNode.id()-1)->disable();
                            modified = true;
                            break;
                        } 
                    }
                }
            }

            if (!modified) break;
        }
        }
    }


	return ir;
}


}
}
