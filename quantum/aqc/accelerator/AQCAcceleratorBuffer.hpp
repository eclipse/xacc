#ifndef QUANTUM_AQC_ACCELERATOR_AQCACCELERATORBUFFER_HPP_
#define QUANTUM_AQC_ACCELERATOR_AQCACCELERATORBUFFER_HPP_


#include "AcceleratorBuffer.hpp"

namespace xacc {
namespace quantum {

class AQCAcceleratorBuffer : public AcceleratorBuffer {

protected:

	std::map<int, std::list<int>> embedding;

	std::vector<double> energies;

public:

	AQCAcceleratorBuffer(const std::string& str, const int N) :
			AcceleratorBuffer(str, N) {
	}

	template<typename ... Indices>
	AQCAcceleratorBuffer(const std::string& str, int firstIndex,
			Indices ... indices) :
			AcceleratorBuffer(str, firstIndex, indices...) {
	}

	void setEmbedding(std::map<int, std::list<int>> emb) {
		embedding = emb;
	}

	std::map<int, std::list<int>> getEmbedding() {
		return embedding;
	}

};

}
}
#endif /* QUANTUM_AQC_ACCELERATOR_AQCACCELERATORBUFFER_HPP_ */
