#ifndef XACC_ACCELERATOR_EXECUTIONQUEUE_HPP_
#define XACC_ACCELERATOR_EXECUTIONQUEUE_HPP_

#include "Kernel.hpp"

namespace xacc {
/**
 *
 */
class ExecutionQueue {

protected:

	std::queue<Kernel> execQueue;

public:

	enum execType {
		ASYNC, SYNC
	};

	void enqueueKernel(Kernel k) {
		execQueue.push(k);
	}

	template<typename T>
	T execute(std::string, execType type, std::string accExecOpts) {

	}
};
}

#endif
