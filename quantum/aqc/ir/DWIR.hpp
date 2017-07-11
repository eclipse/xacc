#ifndef QUANTUM_AQC_IR_DWIR_HPP_
#define QUANTUM_AQC_IR_DWIR_HPP_

#include "IR.hpp"
#include "DWGraph.hpp"

namespace xacc {
namespace quantum {

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
			kernels.push_back(kernel);

		}

		virtual bool kernelExists(const std::string& name) {
			return std::any_of(kernels.cbegin(), kernels.cend(),
					[=](std::shared_ptr<Function> i) {return i->getName() == name;});
		}

		virtual std::shared_ptr<Function> getKernel(const std::string& name) {
			for (auto f : kernels) {
				if (f->getName() == name) {
					return f;
				}
			}
			XACCError("Invalid kernel name.");
		}

		virtual std::vector<std::shared_ptr<Function>> getKernels() {
			return kernels;
		}

protected:

		/**
			 * Reference to this QIR's list of quantum functions
			 */
			std::vector<std::shared_ptr<Function>> kernels;
};

}
}

#endif /* QUANTUM_AQC_IR_DWIR_HPP_ */
