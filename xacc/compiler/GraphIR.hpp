#ifndef QUANTUM_GRAPHIR_HPP_
#define QUANTUM_GRAPHIR_HPP_

#include "Compiler.hpp"

namespace xacc {

template<typename DerivedGraph>
class GraphIR : public IR {

public:

	virtual std::string toString() {return "";}

};
}


#endif
