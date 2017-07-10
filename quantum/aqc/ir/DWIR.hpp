#ifndef QUANTUM_AQC_IR_DWIR_HPP_
#define QUANTUM_AQC_IR_DWIR_HPP_

#include "IR.hpp"
#include "Graph.hpp"

namespace xacc {
namespace quantum {

using DWVertex = XACCVertex<double>;

class DWGraph :public Graph<DWVertex> {
public:

	virtual void read(std::istream& stream) {
	}

	virtual ~DWGraph() {}

};

class DWIR : public virtual IR {

public:
	/**
		 * Return a assembly-like string representation of this
		 * intermediate representation
		 * @return
		 */
		virtual std::string toAssemblyString(const std::string& kernelName, const std::string& accBufferVarName) {
			return "";
		}

		/**
		 * Persist this IR instance to the given
		 * output stream.
		 *
		 * @param outStream
		 */
		virtual void persist(std::ostream& outStream) {
			return;
		}

		/**
		 * Create this IR instance from the given input
		 * stream.
		 *
		 * @param inStream
		 */
		virtual void load(std::istream& inStream) {

		}

		virtual void addKernel(std::shared_ptr<Function> kernel) {

		}

		virtual bool kernelExists(const std::string& name) {
			return false;
		}

		virtual std::shared_ptr<Function> getKernel(const std::string& name) {
		}

		virtual std::vector<std::shared_ptr<Function>> getKernels() {

		}
};

}
}

#endif /* QUANTUM_AQC_IR_DWIR_HPP_ */
