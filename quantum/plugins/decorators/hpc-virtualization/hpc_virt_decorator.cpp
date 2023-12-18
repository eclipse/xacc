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
 *   Daniel Claudino - MPI native implementation
 *******************************************************************************/
#include "hpc_virt_decorator.hpp"
#include "InstructionIterator.hpp"
#include "Utils.hpp"
#include "xacc_service.hpp"
#include "TearDown.hpp"
#include <numeric>

namespace {
  static bool hpcVirtDecoratorInitializedMpi = false;
}

namespace xacc {
namespace quantum {

void HPCVirtDecorator::initialize(const HeterogeneousMap &params) {

  if (!qpuComm) {
    // Initializing MPI here
    int provided, isMPIInitialized;
    MPI_Initialized(&isMPIInitialized);
    if (!isMPIInitialized) {
      MPI_Init_thread(0, NULL, MPI_THREAD_MULTIPLE, &provided);
      hpcVirtDecoratorInitializedMpi = true;
      if (provided != MPI_THREAD_MULTIPLE) {
        xacc::warning("MPI_THREAD_MULTIPLE not provided.");
      }
    }
  }

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

  if (params.keyExists<int>("shots")) {
    shots = params.get<int>("shots");
    if (shots < 1) {
      xacc::error("Invalid 'shots' parameter.");
    }
  }

  isVqeMode = (shots < 1);
  if (params.keyExists<bool>("vqe-mode")) {
    isVqeMode = params.get<bool>("vqe-mode");
    if (isVqeMode) {
      xacc::info("Enable VQE Mode.");
    }
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

  // Get the rank and size in the original communicator
  auto start = std::chrono::high_resolution_clock::now();
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  auto world = std::make_shared<ProcessGroup>(MPI_COMM_WORLD, world_size);

  if (world_size < n_virtual_qpus) {
    // The number of MPI processes is less than the number of requested virtual
    // QPUs, just execute as if there is only one virtual QPU and give the QPU
    // the whole MPI_COMM_WORLD.
    void *qpu_comm_ptr = reinterpret_cast<void *>(world.get());
    if (!qpuComm) {
      qpuComm = world;
    }
    decoratedAccelerator->updateConfiguration(
        {{"mpi-communicator", qpu_comm_ptr}});
    // just execute
    decoratedAccelerator->execute(buffer, functions);
    return;
  }

  // get ranks in each comm
  std::vector<int> ranks(world_size);
  std::iota(ranks.begin(), ranks.end(), 0);
  auto ranksPerComm = split_vector(ranks, n_virtual_qpus);

  // get color
  int color;
  for (int i = 0; i < n_virtual_qpus; i++) {
    auto ranks = ranksPerComm[i];
    if (std::find(ranks.begin(), ranks.end(), world_rank) != ranks.end()) {
      color = i;
    }
  }

  // Split the communicator based on the color and use the
  // original rank for ordering
  if (!qpuComm) {
    // Splits MPI_COMM_WORLD into sub-communicators if not already.
    qpuComm = world->split(color);
  }

  // current rank now has a color to indicate which sub-comm it belongs to
  // Give that sub communicator to the accelerator
  void *qpu_comm_ptr =
      reinterpret_cast<void *>(qpuComm->getMPICommProxy().getRef<MPI_Comm>());

  // this enables shot-based simulation
  HeterogeneousMap properties;
  properties.insert("mpi-communicator", qpu_comm_ptr);
  if (!isVqeMode) {
    properties.insert("shots", shots);
  }
  decoratedAccelerator->updateConfiguration(properties);

  // get the number of sub-communicators
  // Everybody split the CompositeInstructions vector into n_virtual_qpu
  // segments and get the color for the given process
  auto split_vecs = split_vector(functions, n_virtual_qpus);

  // Get the segment corresponding to this color
  auto my_circuits = split_vecs[color];

  // Create a local buffer and execute
  auto my_buffer = xacc::qalloc(buffer->size());
  decoratedAccelerator->execute(my_buffer, my_circuits);

  // Split world along rank-0 in each sub-communicator and reduce the local
  // energies
  auto zeroRanksComm =
      world->split(world_rank == qpuComm->getProcessRanks()[0]);

  // Here we get the number of children buffers
  // and the number of children per comm
  int nGlobalChildren = 0;
  std::vector<int> nLocalChildren(n_virtual_qpus);
  if (world_rank == qpuComm->getProcessRanks()[0]) {

    // reduce the number of children
    auto nChildren = my_buffer->nChildren();
    MPI_Allreduce(&nChildren, &nGlobalChildren, 1, MPI_INT, MPI_SUM,
                  zeroRanksComm->getMPICommProxy().getRef<MPI_Comm>());

    // get number of children in each subcommunicator
    MPI_Allgather(&nChildren, 1, MPI_INT, nLocalChildren.data(), 1, MPI_INT,
                  zeroRanksComm->getMPICommProxy().getRef<MPI_Comm>());
  }

  // broadcast the total number of children
  qpuComm->broadcast(nGlobalChildren);

  // broadcast the number of children in each communicator
  qpuComm->broadcast(nLocalChildren);

  // get expectation values/bitstrings and the size of the key of each child buffer
  std::vector<int> globalKeySizes(nGlobalChildren);
  std::vector<int> globalNumberBitStrings;
  std::vector<double> globalExpVals;
  if (isVqeMode) {
    globalExpVals.resize(nGlobalChildren);
  } else {
    globalNumberBitStrings.resize(nGlobalChildren);
  }

  if (world_rank == qpuComm->getProcessRanks()[0]) {

    // get displacements for the keys in each comm
    std::vector<int> nKeyShift(n_virtual_qpus);
    for (int i = 0; i < n_virtual_qpus; i++) {
      nKeyShift[i] = std::accumulate(nLocalChildren.begin(),
                                     nLocalChildren.begin() + i, 0);
    }

    // get size of each key in the communicator
    std::vector<double> localExpVals;
    std::vector<int> localKeySizes, localNumberBitStrings;
    for (auto child : my_buffer->getChildren()) {
      localKeySizes.push_back(child->name().size());

      if (isVqeMode) {
        localExpVals.push_back(child->getExpectationValueZ());
      } else {
        localNumberBitStrings.push_back(child->getMeasurementCounts().size());
      }
    }

    // gather the size of each child key
    zeroRanksComm->allGatherv(localKeySizes, globalKeySizes, nLocalChildren, nKeyShift);

    if (isVqeMode) {
      // gather all expectation values
      zeroRanksComm->allGatherv(localExpVals, globalExpVals, nLocalChildren, nKeyShift);
    } else {
      // gather all bitstrings
      zeroRanksComm->allGatherv(localNumberBitStrings, globalNumberBitStrings, nLocalChildren, nKeyShift);
    }

  }

  // broadcast size of each key
  qpuComm->broadcast(globalKeySizes);

  // broadcast results
  if (isVqeMode) {
    // broadcast expectation values
    qpuComm->broadcast(globalExpVals);
  } else {
    // broadcast number of bit strings
    qpuComm->broadcast(globalNumberBitStrings);
  }

  // get the size of all keys
  auto nGlobalKeyChars =
      std::accumulate(globalKeySizes.begin(), globalKeySizes.end(), 0);

  // get total number of measured bitstrings
  auto nGlobalBitStrings =
      std::accumulate(globalNumberBitStrings.begin(), globalNumberBitStrings.end(), 0);

  // gather all keys chars
  std::vector<char> globalKeyChars(nGlobalKeyChars);
  std::vector<int> globalBitStrings, globalCounts;
  if (!isVqeMode) {
    globalBitStrings.resize(nGlobalBitStrings);
    globalCounts.resize(nGlobalBitStrings);
  }
  if (world_rank == qpuComm->getProcessRanks()[0]) {

    // get local key char arrays
    // and local bitstrings and counts
    std::vector<char> localKeys;
    std::vector<int> localBitStringIndices, localCounts;
    for (auto child : my_buffer->getChildren()) {

      for (auto c : child->name()) {
        localKeys.push_back(c);
      }

      // get bitstring decimals and counts
      if (!isVqeMode) {
        for (auto & count : child->getMeasurementCounts()) {
          auto bitString = count.first;
          // stoi is MSB
          if (decoratedAccelerator->getBitOrder() == Accelerator::BitOrder::LSB) {
		        std::reverse(bitString.begin(), bitString.end());
          }
          auto index = std::stoi(count.first, nullptr, 2);
          localBitStringIndices.push_back(index);
          localCounts.push_back(count.second);
        }
      }
    }

    // get the size of keys in the communicator
    std::vector<int> commKeySize(n_virtual_qpus);
    int shift = 0;
    for (int i = 0; i < n_virtual_qpus; i++) {
      auto it = globalKeySizes.begin() + shift;
      commKeySize[i] = std::accumulate(it, it + nLocalChildren[i], 0);
      shift += nLocalChildren[i];
    }

    // shifts for key sizes
    std::vector<int> keySizeShift(n_virtual_qpus);
    for (int i = 1; i < n_virtual_qpus; i++) {
      keySizeShift[i] =
          std::accumulate(commKeySize.begin(), commKeySize.begin() + i, 0);
    }

    // gather all key chars
    zeroRanksComm->allGatherv(localKeys, globalKeyChars, commKeySize, keySizeShift);

    if (!isVqeMode) {

      // get number of bit strings in the communicator
      std::vector<int> commNumberBitStrings(n_virtual_qpus);
      shift = 0;
      for (int i = 0; i < n_virtual_qpus; i++) {
        auto it = globalNumberBitStrings.begin() + shift;
        commNumberBitStrings[i] = std::accumulate(it, it + nLocalChildren[i], 0);
        shift += nLocalChildren[i];
      }

      // shifts for bit strings
      std::vector<int> bitStringShift(n_virtual_qpus);
      for (int i = 1; i < n_virtual_qpus; i++) {
        bitStringShift[i] =
            std::accumulate(commNumberBitStrings.begin(), commNumberBitStrings.begin() + i, 0);
      }

      // gather all bit strings
      zeroRanksComm->allGatherv(localBitStringIndices, globalBitStrings, commNumberBitStrings, bitStringShift);
      // gather all counts
      zeroRanksComm->allGatherv(localCounts, globalCounts, commNumberBitStrings, bitStringShift);
    }

  }

  // broadcast all keys
  qpuComm->broadcast(globalKeyChars);

  if (!isVqeMode) {
   // broadcast indices
    qpuComm->broadcast(globalBitStrings);
    qpuComm->broadcast(globalCounts);
  }

  // get binary from decimal
  const auto getBinary = [=](int n){
    std::string s;
    while (n != 0) {
      s += (n % 2 == 0 ? "0" : "1" );
      n /= 2;
    }
    //s += std::string(buffer->size() - s.size(), '0');
    std::reverse(s.begin(), s.end());
    return s;
  };

  // now every process has everything to rebuild the buffer
  int shift = 0, countShift = 0;
  for (int i = 0; i < nGlobalChildren; i++) {

    // get child name
    auto it = globalKeyChars.begin() + shift;
    std::string name(it, it + globalKeySizes[i]);

    // create child buffer and append it to buffer
    auto child = xacc::qalloc(buffer->size());
    child->setName(name);

    if (isVqeMode) {
      child->addExtraInfo("exp-val-z", globalExpVals[i]);
    } else {

      auto nChildBitStrings = globalNumberBitStrings[i];
      for (int b = 0; b < nChildBitStrings; b++) {

        auto counts = globalCounts[b + countShift];
        if (counts == 0) std::cout << "GOTCHA\n";
        auto bitStringDecimal = globalBitStrings[b + countShift];
        auto nBits = name.length() / 2;
        auto bitString = getBinary(bitStringDecimal);
        //auto bitString = getBinary(bitStringDecimal, nBits);
        child->appendMeasurement(bitString, counts);

      }
      countShift += nChildBitStrings;
    }

    buffer->appendChild(name, child);
    shift += globalKeySizes[i];
  }

  // Setup a barrier
  MPI_Barrier(qpuComm->getMPICommProxy().getRef<MPI_Comm>());
  MPI_Barrier(world->getMPICommProxy().getRef<MPI_Comm>());
  buffer->addExtraInfo("rank", world_rank);

  return;
}

void HPCVirtDecorator::finalize() {
  if (qpuComm) {
    // Make sure we explicitly release this so that MPICommProxy is destroyed
    // before framework shutdown (MPI_Finalize if needed)
    qpuComm.reset();
  }
}

class HPCVirtTearDown : public xacc::TearDown {
public:
  virtual void tearDown() override {
    auto c = xacc::getService<xacc::AcceleratorDecorator>("hpc-virtualization", false);
    if (c) {
      auto casted = std::dynamic_pointer_cast<xacc::quantum::HPCVirtDecorator>(c);
      assert(casted);
      casted->finalize();
    }

    int finalized, initialized;
    MPI_Initialized(&initialized);
    if (initialized) {
      MPI_Finalized(&finalized);
      if (!finalized && hpcVirtDecoratorInitializedMpi) {
        MPI_Finalize();
      }
    }
  }
  virtual std::string name() const override { return "xacc-hpc-virt"; }
};


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
    context.RegisterService<xacc::TearDown>(std::make_shared<xacc::quantum::HPCVirtTearDown>());
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(HPCVirtActivator)
