#ifndef QUANTUM_AQC_IR_DWAVEIR_HPP_
#define QUANTUM_AQC_IR_DWAVEIR_HPP_

#include "IR.hpp"

namespace xacc {
namespace quantum {
class DWaveIR : public virtual IR {

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


};

}
}

#endif /* QUANTUM_AQC_IR_DWAVEIR_HPP_ */
