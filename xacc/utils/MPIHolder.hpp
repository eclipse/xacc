/*
 * MPIHolder.hpp
 *
 *  Created on: Aug 11, 2017
 *      Author: aqw
 */

#ifndef XACC_UTILS_MPIHOLDER_HPP_
#define XACC_UTILS_MPIHOLDER_HPP_
#ifdef XACC_HAS_MPI
#include "Singleton.hpp"
#include <boost/mpi.hpp>

namespace xacc {
class MPIHolder : public Singleton<MPIHolder> {

protected:

	std::shared_ptr<boost::mpi::environment> env;
	std::shared_ptr<boost::mpi::communicator> communicator;
public:

	void initialize(int argc, char** argv) {
		env = std::make_shared<boost::mpi::environment>(argc, argv);
		communicator = std::make_shared<boost::mpi::communicator>();
	}

	const int rank() {
		return communicator->rank();
	}

	const int size() {
		return communicator->size();
	}

	const boost::mpi::communicator& getCommunicator() {
		return *communicator.get();
	}

	void barrier() {
		communicator->barrier();
	}

};
}

#else
namespace xacc {
class MPIHolder : public Singleton<MPIHolder> {
public:
	void initialize(int argc, char** argv) {}
	const int rank() {}
	const int size() {}
	const void getCommunicator() {}
	void barrier() {}
};
}

#endif

#endif
