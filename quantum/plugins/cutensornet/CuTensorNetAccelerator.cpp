/*******************************************************************************
 * Copyright (c) 2024 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#include "CuTensorNetVisitor.hpp"
#include "CuTensorNetAccelerator.hpp"
#include "xacc_plugin.hpp"
#include "IRUtils.hpp"

std::map<std::string, std::string> rotationGates{
    {"Measure", "Z"}, {"H", "X"}, {"Rx", "Y"}};

namespace xacc {
namespace quantum {

void CuTensorNetAccelerator::initialize(const HeterogeneousMap &params) {

  HANDLE_CUDA_ERROR(cudaSetDevice(0));
  if (!cutnHandle) {
    HANDLE_CUTN_ERROR(cutensornetCreate(&cutnHandle));
  }
  xacc::info("Initialized cuTensorNet library on GPU 0");

  if (params.keyExists<int>("bond-dimension")) {
    bondDimension = params.get<int>("bond-dimension");
  } else {
    xacc::warning("Using bond dimension default of 2.");
  }

  if (params.pointerLikeExists<PauliOperator>("observable")) {
    observable = params.getPointerLike<PauliOperator>("observable");
  }

  return;
}

void CuTensorNetAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> program) {

  auto numQubits = buffer->size();
  // Determine the MPS representation and allocate buffers for the MPS tensors
  const int64_t maxExtent = bondDimension;
  std::vector<std::vector<int64_t>> extents;
  std::vector<int64_t *> extentsPtr(numQubits);
  std::vector<void *> d_mpsTensors(numQubits, nullptr);
  for (int32_t i = 0; i < numQubits; i++) {
    if (i == 0) { // left boundary MPS tensor
      extents.push_back({2, maxExtent});
      HANDLE_CUDA_ERROR(
          cudaMalloc(&d_mpsTensors[i], 2 * maxExtent * 2 * fp64size));
    } else if (i == numQubits - 1) { // right boundary MPS tensor
      extents.push_back({maxExtent, 2});
      HANDLE_CUDA_ERROR(
          cudaMalloc(&d_mpsTensors[i], 2 * maxExtent * 2 * fp64size));
    } else { // middle MPS tensors
      extents.push_back({maxExtent, 2, maxExtent});
      HANDLE_CUDA_ERROR(cudaMalloc(&d_mpsTensors[i],
                                   2 * maxExtent * maxExtent * 2 * fp64size));
    }
    extentsPtr[i] = extents[i].data();
  }

  // Query the free memory on Device
  std::size_t freeSize{0}, totalSize{0};
  HANDLE_CUDA_ERROR(cudaMemGetInfo(&freeSize, &totalSize));
  const std::size_t scratchSize = (freeSize - (freeSize % 4096)) / 2;
  void *d_scratch{nullptr};
  HANDLE_CUDA_ERROR(cudaMalloc(&d_scratch, scratchSize));

  // Create the initial quantum state
  cutensornetState_t quantumState;
  const std::vector<int64_t> qubitDims(numQubits, 2);
  HANDLE_CUTN_ERROR(cutensornetCreateState(
      cutnHandle, CUTENSORNET_STATE_PURITY_PURE, numQubits, qubitDims.data(),
      CUDA_C_64F, &quantumState));

  CuTensorNetVisitor visitor(quantumState, cutnHandle);
  InstructionIterator it(program);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled() && nextInst->name() != "Measure") {
      nextInst->accept(&visitor);
    }
  }

  // Specify the final target MPS representation (use default fortran strides)
  HANDLE_CUTN_ERROR(cutensornetStateFinalizeMPS(
      cutnHandle, quantumState, CUTENSORNET_BOUNDARY_CONDITION_OPEN,
      extentsPtr.data(), /*strides=*/nullptr));

  // Optional, set up the SVD method for truncation.
  cutensornetTensorSVDAlgo_t algo = CUTENSORNET_TENSOR_SVD_ALGO_GESVDJ;
  HANDLE_CUTN_ERROR(cutensornetStateConfigure(
      cutnHandle, quantumState, CUTENSORNET_STATE_CONFIG_MPS_SVD_ALGO, &algo,
      sizeof(algo)));

  // Prepare the MPS computation and attach workspace
  cutensornetWorkspaceDescriptor_t workDesc;
  HANDLE_CUTN_ERROR(
      cutensornetCreateWorkspaceDescriptor(cutnHandle, &workDesc));

  HANDLE_CUTN_ERROR(cutensornetStatePrepare(cutnHandle, quantumState,
                                            scratchSize, workDesc, 0x0));

  double flops{0.0};
  HANDLE_CUTN_ERROR(cutensornetStateGetInfo(cutnHandle, quantumState,
                                            CUTENSORNET_STATE_INFO_FLOPS,
                                            &flops, sizeof(flops)));
  if (flops < 0.0) {
    xacc::error("Negative Flop count!");
  }

  int64_t worksize{0};
  HANDLE_CUTN_ERROR(cutensornetWorkspaceGetMemorySize(
      cutnHandle, workDesc, CUTENSORNET_WORKSIZE_PREF_RECOMMENDED,
      CUTENSORNET_MEMSPACE_DEVICE, CUTENSORNET_WORKSPACE_SCRATCH, &worksize));

  if (worksize <= scratchSize) {
    HANDLE_CUTN_ERROR(cutensornetWorkspaceSetMemory(
        cutnHandle, workDesc, CUTENSORNET_MEMSPACE_DEVICE,
        CUTENSORNET_WORKSPACE_SCRATCH, d_scratch, worksize));
  } else {
    xacc::error("Insufficient workspace size on Device!");
  }

  // Execute MPS computation
  HANDLE_CUTN_ERROR(cutensornetStateCompute(
      cutnHandle, quantumState, workDesc, extentsPtr.data(),
      /*strides=*/nullptr, d_mpsTensors.data(), 0));

  // Create an empty tensor network operator
  cutensornetNetworkOperator_t hamiltonian;
  HANDLE_CUTN_ERROR(cutensornetCreateNetworkOperator(
      cutnHandle, numQubits, qubitDims.data(), CUDA_C_64F, &hamiltonian));

  int64_t id = visitor.getTensorId();

  if (observable) {

    std::map<std::string, void *> devicePaulis;
    devicePaulis.insert({"X", visitor.getX()});
    devicePaulis.insert({"Y", visitor.getY()});
    devicePaulis.insert({"Z", visitor.getZ()});

    for (auto it = observable->begin(); it != observable->end(); ++it) {

      auto paulis = it->second.ops();

      std::vector<int32_t> numModes(paulis.size(), 1);
      std::vector<std::vector<int32_t>> stateModesVecs(paulis.size());
      std::vector<const int32_t *> stateModes(paulis.size());
      std::vector<const void *> gateData(paulis.size());

      int siteCounter = 0;
      for (auto &site : paulis) {
        stateModesVecs[siteCounter] = {static_cast<int32_t>(site.first)};
        stateModes[siteCounter] = stateModesVecs[siteCounter].data();
        gateData[siteCounter] = devicePaulis[site.second];
        siteCounter++;
      }

      HANDLE_CUTN_ERROR(cutensornetNetworkOperatorAppendProduct(
          cutnHandle, hamiltonian, cuDoubleComplex{1.0, 0.0}, paulis.size(),
          numModes.data(), stateModes.data(), NULL, gateData.data(), &id));
    }

  } else {
    xacc::error("Missing observable to calculate expectation value of.");
  }

  xacc::info("Constructed a tensor network operator corresponding to the "
             "provided operator");

  // Specify the quantum circuit expectation value
  cutensornetStateExpectation_t expectation;
  HANDLE_CUTN_ERROR(cutensornetCreateExpectation(cutnHandle, quantumState,
                                                 hamiltonian, &expectation));

  // Configure the computation of the specified quantum circuit expectation
  // value
  // desired number of hyper samples used in the tensor network
  // contraction path finder
  const int32_t numHyperSamples = 8;
  HANDLE_CUTN_ERROR(cutensornetExpectationConfigure(
      cutnHandle, expectation, CUTENSORNET_EXPECTATION_CONFIG_NUM_HYPER_SAMPLES,
      &numHyperSamples, sizeof(numHyperSamples)));

  // Prepare the specified quantum circuit expectation value for computation
  HANDLE_CUTN_ERROR(cutensornetExpectationPrepare(cutnHandle, expectation,
                                                  scratchSize, workDesc, 0x0));
  flops = 0.0;
  HANDLE_CUTN_ERROR(cutensornetExpectationGetInfo(
      cutnHandle, expectation, CUTENSORNET_EXPECTATION_INFO_FLOPS, &flops,
      sizeof(flops)));

  if (flops <= 0.0) {
    xacc::error("Invalid Flop count!");
  }

  // Attach the workspace buffer
  HANDLE_CUTN_ERROR(cutensornetWorkspaceGetMemorySize(
      cutnHandle, workDesc, CUTENSORNET_WORKSIZE_PREF_RECOMMENDED,
      CUTENSORNET_MEMSPACE_DEVICE, CUTENSORNET_WORKSPACE_SCRATCH, &worksize));
  xacc::info("Required scratch GPU workspace size (bytes) = " +
             std::to_string(worksize));

  if (worksize <= scratchSize) {
    HANDLE_CUTN_ERROR(cutensornetWorkspaceSetMemory(
        cutnHandle, workDesc, CUTENSORNET_MEMSPACE_DEVICE,
        CUTENSORNET_WORKSPACE_SCRATCH, d_scratch, worksize));
  } else {
    xacc::error("Insufficient workspace size on Device!");
  }

  // Compute the specified quantum circuit expectation value
  std::complex<double> expectVal{0.0, 0.0}, stateNorm2{0.0, 0.0};
  HANDLE_CUTN_ERROR(cutensornetExpectationCompute(
      cutnHandle, expectation, workDesc, static_cast<void *>(&expectVal),
      static_cast<void *>(&stateNorm2), 0x0));
  xacc::info("Computed the specified quantum circuit expectation value");
  expectVal /= stateNorm2;
  xacc::info("Expectation value = (" + std::to_string(expectVal.real()) + ", " +
             std::to_string(expectVal.imag()) + ")");
  xacc::info("Squared 2-norm of the state = (" +
             std::to_string(stateNorm2.real()) + ", " +
             std::to_string(stateNorm2.imag()) + ")");
  buffer->addExtraInfo("exp-val-z", expectVal.real());

  // Destroy the workspace descriptor
  HANDLE_CUTN_ERROR(cutensornetDestroyWorkspaceDescriptor(workDesc));

  // Destroy the quantum circuit expectation value
  HANDLE_CUTN_ERROR(cutensornetDestroyExpectation(expectation));

  // Destroy the tensor network operator
  HANDLE_CUTN_ERROR(cutensornetDestroyNetworkOperator(hamiltonian));

  // Destroy the quantum circuit state
  HANDLE_CUTN_ERROR(cutensornetDestroyState(quantumState));

  for (int32_t i = 0; i < numQubits; i++) {
    HANDLE_CUDA_ERROR(cudaFree(d_mpsTensors[i]));
  }

  HANDLE_CUDA_ERROR(cudaFree(d_scratch));
  xacc::info("Freed memory on GPU");

  return;
}

void CuTensorNetAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {

  auto kernelDecomposed =
      ObservedAnsatz::fromObservedComposites(compositeInstructions);

  auto baseCircuit = kernelDecomposed.getBase();
  auto obsCircuits = kernelDecomposed.getObservedSubCircuits();

  execute(buffer, baseCircuit, obsCircuits);
  return;
}

void CuTensorNetAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> baseCircuit,
    const std::vector<std::shared_ptr<CompositeInstruction>> basisRotations) {

  for (auto &c : basisRotations) {

    std::map<int, std::string> ops;
    InstructionIterator it(c);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (!dynamic_cast<xacc::quantum::Gate *>(nextInst.get())) {
        continue;
      }
      int bit = nextInst->bits()[0];
      auto keyPos = ops.find(bit);

      if (keyPos != ops.end()) {
        if (nextInst->name() != "Measure")
          ops.insert({bit, rotationGates[nextInst->name()]});
      } else {
        ops.insert({bit, rotationGates[nextInst->name()]});
      }
    }

    observable = new PauliOperator(ops);
    auto tmpBuffer = qalloc(buffer->size());
    execute(tmpBuffer, baseCircuit);
    tmpBuffer->setName(c->name());
    buffer->appendChild(c->name(), tmpBuffer);
  }

  return;
}

} // namespace quantum
} // namespace xacc
REGISTER_ACCELERATOR(xacc::quantum::CuTensorNetAccelerator)