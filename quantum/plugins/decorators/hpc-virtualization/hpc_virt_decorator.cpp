/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "hpc_virt_decorator.hpp"
#include "InstructionIterator.hpp"
#include "Utils.hpp"
#include "xacc.hpp"

#include <boost/mpi.hpp>
#include <boost/mpi/collectives/all_gather.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <mpi.h>

namespace xacc {
namespace quantum {

void HPCVirtDecorator::initialize(const HeterogeneousMap &params) {
  decoratedAccelerator->initialize(params);

  if (params.keyExists<int>("n-virtual-qpus")) {
    if (qpuComm && n_virtual_qpus != params.get<int>("n-virtual-qpus")) {
      // We don't support changing the number of virtual QPU's
      // i.e. between xacc::Initialize and xacc::Finalize,
      // we must use an HPCVirtDecorator with a consistent number of virtual
      // QPU's.
      xacc::error(
          "Dynamically changing the number of virtual QPU's is not supported.");
    }
    n_virtual_qpus = params.get<int>("n-virtual-qpus");
  }
}

void HPCVirtDecorator::updateConfiguration(const HeterogeneousMap &config) {
  decoratedAccelerator->updateConfiguration(config);
}

void HPCVirtDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  if (decoratedAccelerator)
    decoratedAccelerator->execute(buffer, function);

  return;
}

void HPCVirtDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  // The goal here is the following:
  // Assume we have an HPC system with a set of
  // compute ranks M, and we partition that set of ranks into
  // communicator sub-groups, each with a dedicated virtual QPU available.
  // Also we are given a vector of observable terms
  // (vec of CompositeInstructions) of size N (N>>1),
  // we wish to evaluate the <O> for each by partitioning
  // their quantum execution across the node sub-groups.

  using namespace boost;
  int initialized;
  MPI_Initialized(&initialized);
  if (!initialized) {
    MPI_Init(&xacc::argc, &xacc::argv);
  }

  mpi::communicator world;

  // Get the rank and size in the original communicator
  int world_rank = world.rank(), world_size = world.size();

  if (world_size < n_virtual_qpus) {
    // The number of MPI processes is less than the number of requested virtual
    // QPUs, just execute as if there is only one virtual QPU and give the QPU
    // the whole MPI_COMM_WORLD.
    void *qpu_comm_ptr = reinterpret_cast<void *>((MPI_Comm)world);
    if (!qpuComm) {
      qpuComm = std::make_shared<boost::mpi::communicator>(world);
    }
    decoratedAccelerator->updateConfiguration(
        {{"mpi-communicator", qpu_comm_ptr}});
    // just execute
    decoratedAccelerator->execute(buffer, functions);
    return;
  }

  // Get the color for this rank
  int color = world_rank % n_virtual_qpus;

  // Split the communicator based on the color and use the
  // original rank for ordering
  if (!qpuComm) {
    // Splits MPI_COMM_WORLD into sub-communicators if not already.
    qpuComm = std::make_shared<boost::mpi::communicator>(
        world.split(color, world_rank));
  }
  auto qpu_comm = *qpuComm;

  // current rank now has a color to indicate which sub-comm it belongs to
  // Give that sub communicator to the accelerator
  void *qpu_comm_ptr = reinterpret_cast<void *>((MPI_Comm)qpu_comm);
  decoratedAccelerator->updateConfiguration(
      {{"mpi-communicator", qpu_comm_ptr}});

  // Everybody split the CompositeInstructions vector into n_virtual_qpu
  // segments
  auto split_vecs = split_vector(functions, n_virtual_qpus);

  // Get the segment corresponding to this color
  auto my_circuits = split_vecs[color];

  // Create a local buffer and execute
  auto my_buffer = xacc::qalloc(buffer->size());
  decoratedAccelerator->execute(my_buffer, my_circuits);

  // Create a mapping of all local buffer names to the buffer
  std::map<std::string, std::shared_ptr<AcceleratorBuffer>>
      local_name_to_buffer;
  for (auto &child : my_buffer->getChildren()) {
    local_name_to_buffer.insert({child->name(), child});
  }

  // Every sub-group compute local energy
  double local_energy = 0.0;
  for (auto &my_circuit : my_circuits) {
    local_energy +=
        std::real(my_circuit->getCoefficient()) *
        local_name_to_buffer[my_circuit->name()]->getExpectationValueZ();
  }
  
  // for all rank 0s on qpu_comms, split
  // and add them to a new communicator, then
  // all_gather each local_energy to all these ranks
  auto split_along_rank_zeros = world.split(qpu_comm.rank() == 0, world_rank);
  double global_energy = 0.0;
  if (qpu_comm.rank() == 0) {
    
    // This will put every computed local_energy into each
    // qpu sub-comm group, but on rank 0 only
    std::vector<double> all_local_energies;
    mpi::all_gather(split_along_rank_zeros, local_energy, all_local_energies);
    // Sum them all up on all rank 0s to get the global energy
    global_energy =
        std::accumulate(all_local_energies.begin(), all_local_energies.end(),
                        decltype(all_local_energies)::value_type(0));
  }

  qpu_comm.barrier();

  // Now broadcast that global_energy to all
  // other ranks in the sub groups
  mpi::broadcast(qpu_comm, global_energy, 0);

  // Setup a barrier
  qpu_comm.barrier();
  world.barrier();

  // every rank should have global_energy now
  buffer->addExtraInfo("__internal__decorator_aggregate_vqe__", global_energy);

  // Strategy:
  // Every process in a sub-communicator has the same
  // children buffer data. I want each sub-group to share
  // its results with the other subgroups. My thinking is
  // to do an all_gather on all world ranks, all ranks will then
  // have multiple copies of the data strings, so lets filter them
  // out by using a std::set. Then just load the buffers and add them
  // to the buffer.

  // Need to distribute resultant children buffers to all ranks
  //   std::vector<std::vector<std::string>> all_child_buffer_strings;
  //   mpi::all_gather(world, my_child_buffer_strings,
  //   all_child_buffer_strings);

  //   // Everyone has all copies of child buffer strings, filter them out to be
  //   // unique
  //   std::set<std::string> unique_buffer_strings;
  //   for (auto &child_buffer_strings : all_child_buffer_strings) {
  //     for (auto child_buffer_string : child_buffer_strings) {
  //       unique_buffer_strings.insert(child_buffer_string);
  //     }
  //   }

  //   // everyone add the children buffers to the incoming buffer
  //   std::map<std::string, std::shared_ptr<AcceleratorBuffer>> name_to_buffer;
  //   for (auto &buffer_string : unique_buffer_strings) {
  //     auto child_buffer = xacc::qalloc(buffer->size());
  //     std::istringstream s(buffer_string);
  //     child_buffer->load(s);
  //     name_to_buffer.insert({child_buffer->name(), child_buffer});
  //   }

  //   // buffers need to be in same order as functions coming in
  //   for (auto &f : functions) {
  //     buffer->appendChild(f->name(), name_to_buffer[f->name()]);
  //   }

  //   qpu_comm.barrier();
  //   world.barrier();

  buffer->addExtraInfo("rank", world_rank);
  return;
}

void HPCVirtTearDown::tearDown() {
  int finalized, initialized;
  MPI_Initialized(&initialized);
  if (initialized) {
    MPI_Finalized(&finalized);
    if (!finalized) {
      MPI_Finalize();
    }
  }
}

} // namespace quantum
} // namespace xacc

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

namespace {

class US_ABI_LOCAL HPCVirtActivator : public BundleActivator {

public:
  HPCVirtActivator() {}

  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::quantum::HPCVirtDecorator>();

    context.RegisterService<xacc::AcceleratorDecorator>(c);
    context.RegisterService<xacc::Accelerator>(c);

    context.RegisterService<xacc::TearDown>(
        std::make_shared<xacc::quantum::HPCVirtTearDown>());
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(HPCVirtActivator)
