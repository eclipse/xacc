/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_IRTOGRAPHVISITOR_HPP_
#define QUANTUM_IRTOGRAPHVISITOR_HPP_

#include "AllGateVisitor.hpp"
#include "GateFunction.hpp"
#include "Graph.hpp"

namespace xacc {
namespace quantum {

class IRToGraphVisitor: public AllGateVisitor {

protected:

	Graph<CircuitNode, Directed> graph;

    std::map<int, CircuitNode> qubitToLastNode;
        
    int id = 0;
    
    void addSingleQubitGate(GateInstruction& inst) {
        auto bit = inst.bits()[0];
        auto lastNode = qubitToLastNode[bit];
        auto lastBit = lastNode.bits()[0];

        id++;
                    
        CircuitNode newNode(inst.name(), 
                            0, 
                            id, inst.bits(), 
                            true, 
                            std::vector<std::string>{});
                    
        graph.addVertex(newNode);
        graph.addEdge(lastNode.id(), newNode.id(), 1);

        qubitToLastNode[bit] = newNode;
    }

    void addTwoQubitGate(GateInstruction& inst) {
        auto srcbit = inst.bits()[0];
        auto tgtbit = inst.bits()[1];

        auto lastsrcnodeid = qubitToLastNode[srcbit].id();
        auto lasttgtnodeid = qubitToLastNode[tgtbit].id();
        
        id++;
        CircuitNode newNode(inst.name(), 
                            0, 
                            id, inst.bits(), 
                            true, 
                            std::vector<std::string>{});
     
        graph.addVertex(newNode);
        graph.addEdge(lastsrcnodeid, id, 1); 
        graph.addEdge(lasttgtnodeid, id, 1); 
        
        qubitToLastNode[srcbit] = newNode;
        qubitToLastNode[tgtbit] = newNode; 
    }
    
public:

	IRToGraphVisitor(const int nQubits) {
        std::vector<int> allQbitIds(nQubits) ; 
        std::iota(std::begin(allQbitIds), std::end(allQbitIds), 0); 
        CircuitNode initNode("InitialState", 0, 0, allQbitIds, true, std::vector<std::string>{});
        for (int i = 0; i < nQubits; i++) {
            qubitToLastNode.insert({i,initNode});
        }
        graph.addVertex(initNode);
    }

    Graph<CircuitNode, Directed> getGraph() {
        CircuitNode finalNode;
	    std::get<0>(finalNode.properties) = "FinalState";
	    std::get<1>(finalNode.properties) = 0;
	    std::get<2>(finalNode.properties) = id+1;
        std::get<3>(finalNode.properties) = {};
	    std::get<4>(finalNode.properties) = true;
	    graph.addVertex(finalNode);

        for (auto& kv : qubitToLastNode) {
            graph.addEdge(kv.second.id(), finalNode.id(), 1.0);
        }
        
        // std::vector<double> distances;
        // std::vector<int> paths;
        
        // // Set the layers with a BFS.
        // graph.computeShortestPath(0, distances, paths);

        // std::cout << graph.order() << ", " << distances.size() << ", PATHS FROM 0 to \n";
        // for (int i = 1; i < distances.size()-1; ++i) {
        //     std::get<1>(graph.getVertex(i).properties) = distances[i];
        //     std::cout << i << ", " << distances[i] << "\n";
        // }

        return graph;
    }
    
	void visit(Hadamard& h) {
        addSingleQubitGate(h);
	}

	void visit(Identity& i) {
	}

	void visit(CZ& cz) {
        addTwoQubitGate(cz);
	}

	void visit(CNOT& cn) {
        addTwoQubitGate(cn);
	}

	void visit(X& x) {
        addSingleQubitGate(x);
	}

	void visit(Y& y) {
        addSingleQubitGate(y);
	}

	void visit(Z& z) {
        addSingleQubitGate(z);
	}

	void visit(Measure& m) {
        addSingleQubitGate(m);
	}

	void visit(ConditionalFunction& c) {
		// nothing
	}

	void visit(Rx& rx) {
        addSingleQubitGate(rx);
	}

	void visit(Ry& ry) {
        addSingleQubitGate(ry);
	}

	void visit(Rz& rz) {
        addSingleQubitGate(rz);
	}

	void visit(CPhase& cp) {
        addTwoQubitGate(cp);
	}

	void visit(Swap& s) {
        addTwoQubitGate(s);
	}

	void visit(GateFunction& f) {
        // nothing
	}

	virtual ~IRToGraphVisitor() {}
};
}
}

#endif