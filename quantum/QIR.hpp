/*
 * QIR.hpp
 *
 *  Created on: Apr 12, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_QIR_HPP_
#define QUANTUM_QIR_HPP_

#include "IR.hpp"

template<typename VertexType>
class QIR : public virtual xacc::Graph<VertexType> {
public:
	virtual void generateGraph() = 0;
	virtual ~QIR() {}
};

#endif /* QUANTUM_QIR_HPP_ */
