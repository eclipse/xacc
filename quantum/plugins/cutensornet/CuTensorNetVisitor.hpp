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
#pragma once
#include "AllGateVisitor.hpp"
#include <complex>
#include <vector>
#include <cmath>
#include "CuTensorNetHelper.hpp"

using namespace xacc;
using namespace xacc::quantum;
constexpr std::size_t fp64size = sizeof(double);

namespace xacc {
namespace quantum {

class CuTensorNetVisitor : public AllGateVisitor,
                           public InstructionVisitor<Circuit> {

private:
  int64_t id;
  const double invsq2 = 1.0 / std::sqrt(2.0);
  std::complex<double> i{0.0, 1.0};
  // gate definitions
  // identity
  void *d_gateI{nullptr};
  // Pauli X gate
  void *d_gateX{nullptr};
  // Pauli Y gate
  void *d_gateY{nullptr};
  // Pauli Z gate
  void *d_gateZ{nullptr};
  // Hadamard gate
  void *d_gateH{nullptr};
  // S gate
  void *d_gateS{nullptr};
  // Sdg
  void *d_gateSdg{nullptr};
  // T
  void *d_gateT{nullptr};
  // Tdg
  void *d_gateTdg{nullptr};
  // CNOT
  void *d_gateCX{nullptr};
  // CZ
  void *d_gateCZ{nullptr};
  // CY
  void *d_gateCY{nullptr};
  // Swap
  void *d_gateSwap{nullptr};
  // iSwap
  void *d_gateiSwap{nullptr};

  std::vector<void *> allocatedGates;
  cutensornetState_t quantumState_;
  cutensornetHandle_t cutnHandle_;

  std::vector<int32_t> getBits(const std::vector<std::size_t> &gateBits) {
    std::vector<int32_t> bits;
    bits.reserve(gateBits.size());
    for (std::size_t b : gateBits) {
      bits.push_back(static_cast<int32_t>(b));
    }
    return bits;
  }

  void allocateX() {
    const std::vector<std::complex<double>> h_gateX{0, 1, 1, 0};
    HANDLE_CUDA_ERROR(cudaMalloc(&d_gateX, 4 * (2 * fp64size)));
    HANDLE_CUDA_ERROR(cudaMemcpy(d_gateX, h_gateX.data(), 4 * (2 * fp64size),
                                 cudaMemcpyHostToDevice));
    allocatedGates.push_back(d_gateX);
  }

  void allocateY() {
    const std::vector<std::complex<double>> h_gateY{0, -i, i, 0};
    HANDLE_CUDA_ERROR(cudaMalloc(&d_gateY, 4 * (2 * fp64size)));
    HANDLE_CUDA_ERROR(cudaMemcpy(d_gateY, h_gateY.data(), 4 * (2 * fp64size),
                                 cudaMemcpyHostToDevice));
    allocatedGates.push_back(d_gateY);
  }

  void allocateZ() {
    const std::vector<std::complex<double>> h_gateZ{1, 0, 0, -1};
    HANDLE_CUDA_ERROR(cudaMalloc(&d_gateZ, 4 * (2 * fp64size)));
    HANDLE_CUDA_ERROR(cudaMemcpy(d_gateZ, h_gateZ.data(), 4 * (2 * fp64size),
                                 cudaMemcpyHostToDevice));
    allocatedGates.push_back(d_gateZ);
  }

public:

  CuTensorNetVisitor(cutensornetState_t quantumState,
                     cutensornetHandle_t cutnHandle)
      : quantumState_(quantumState), cutnHandle_(cutnHandle) {}

  void *getX() {
    if (!d_gateX) {
      allocateX();
    }
    return d_gateX;
  }

  void *getY() {
    if (!d_gateY) {
      allocateY();
    }
    return d_gateY;
  }

  void *getZ() {
    if (!d_gateZ) {
      allocateZ();
    }
    return d_gateZ;
  }

  int64_t getTensorId() {
    return id;
  }

  void visit(Identity &identity) override {
    if (!d_gateI) {
      const std::vector<std::complex<double>> h_gateI{1, 0, 0, 1};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateI, 4 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateI, h_gateI.data(), 4 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateI);
    }

    auto bits = getBits(identity.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateI, nullptr, 1, 0, 1,
        &id));
  }

  void visit(X &x) override {
    if (!d_gateX) {
      allocateX();
    }

    auto bits = getBits(x.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateX, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Y &y) override {
    if (!d_gateY) {
      allocateY();
    }

    auto bits = getBits(y.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateY, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Z &z) override {
    if (!d_gateZ) {
      allocateZ();
    }

    auto bits = getBits(z.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateZ, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Hadamard &h) override {
    if (!d_gateH) {
      const std::vector<std::complex<double>> h_gateH{invsq2, invsq2, invsq2,
                                                      -invsq2};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateH, 4 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateH, h_gateH.data(), 4 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateH);
    }

    auto bits = getBits(h.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateH, nullptr, 1, 0, 1,
        &id));
  }

  void visit(S &s) override {
    if (!d_gateS) {
      const std::vector<std::complex<double>> h_gateS{1, 0, 0, i};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateS, 4 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateS, h_gateS.data(), 4 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateS);
    }

    auto bits = getBits(s.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateS, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Sdg &sdg) override {
    if (!d_gateSdg) {
      const std::vector<std::complex<double>> h_gateSdg{1, 0, 0, -i};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateSdg, 4 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateSdg, h_gateSdg.data(),
                                   4 * (2 * fp64size), cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateSdg);
    }

    auto bits = getBits(sdg.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateSdg, nullptr, 1, 0, 1,
        &id));
  }

  void visit(T &t) override {
    if (!d_gateT) {
      const std::vector<std::complex<double>> h_gateT{1, 0, 0,
                                                      (1.0 + i) * invsq2};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateT, 4 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateT, h_gateT.data(), 4 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateT);
    }

    auto bits = getBits(t.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateT, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Tdg &tdg) override {
    if (!d_gateTdg) {
      const std::vector<std::complex<double>> h_gateTdg{1, 0, 0,
                                                        (1.0 - i) * invsq2};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateTdg, 4 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateTdg, h_gateTdg.data(),
                                   4 * (2 * fp64size), cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateTdg);
    }

    auto bits = getBits(tdg.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateTdg, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Rx &rx) override {
    auto theta = InstructionParameterToDouble(rx.getParameter(0)) / 2.0;
    const std::vector<std::complex<double>> h_gateRx{
        std::cos(theta), -i * std::sin(theta), -i * std::sin(theta),
        std::cos(theta)};
    void *d_gateRx{nullptr};
    HANDLE_CUDA_ERROR(cudaMalloc(&d_gateRx, 4 * (2 * fp64size)));
    HANDLE_CUDA_ERROR(cudaMemcpy(d_gateRx, h_gateRx.data(), 4 * (2 * fp64size),
                                 cudaMemcpyHostToDevice));
    allocatedGates.push_back(d_gateRx);

    auto bits = getBits(rx.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateRx, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Ry &ry) override {
    auto theta = InstructionParameterToDouble(ry.getParameter(0)) / 2.0;
    const std::vector<std::complex<double>> h_gateRy{
        std::cos(theta), -std::sin(theta), std::sin(theta), std::cos(theta)};
    void *d_gateRy{nullptr};
    HANDLE_CUDA_ERROR(cudaMalloc(&d_gateRy, 4 * (2 * fp64size)));
    HANDLE_CUDA_ERROR(cudaMemcpy(d_gateRy, h_gateRy.data(), 4 * (2 * fp64size),
                                 cudaMemcpyHostToDevice));
    allocatedGates.push_back(d_gateRy);

    auto bits = getBits(ry.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateRy, nullptr, 1, 0, 1,
        &id));

  }

  void visit(Rz &rz) override {
    auto theta = InstructionParameterToDouble(rz.getParameter(0)) / 2.0;
    const std::vector<std::complex<double>> h_gateRz{
        std::cos(theta) - i * std::sin(theta), 0.0, 0.0,
        std::cos(theta) + i * std::sin(theta)};
    void *d_gateRz{nullptr};
    HANDLE_CUDA_ERROR(cudaMalloc(&d_gateRz, 4 * (2 * fp64size)));
    HANDLE_CUDA_ERROR(cudaMemcpy(d_gateRz, h_gateRz.data(), 4 * (2 * fp64size),
                                 cudaMemcpyHostToDevice));
    allocatedGates.push_back(d_gateRz);

    auto bits = getBits(rz.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 1, bits.data(), d_gateRz, nullptr, 1, 0, 1,
        &id));
  }

  void visit(CNOT &cnot) override {
    if (!d_gateCX) {
      const std::vector<std::complex<double>> h_gateCX{1, 0, 0, 0, 0, 1, 0, 0,
                                                       0, 0, 0, 1, 0, 0, 1, 0};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateCX, 16 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateCX, h_gateCX.data(),
                                   16 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateCX);
    }

    auto bits = getBits(cnot.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 2, bits.data(), d_gateCX, nullptr, 1, 0, 1,
        &id));
  }

  void visit(CY &cy) override {
    if (!d_gateCY) {
      const std::vector<std::complex<double>> h_gateCY{1, 0, 0, 0,  0, 1, 0, 0,
                                                       0, 0, 0, -i, 0, 0, i, 0};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateCY, 16 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateCY, h_gateCY.data(),
                                   16 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateCY);
    }

    auto bits = getBits(cy.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 2, bits.data(), d_gateCY, nullptr, 1, 0, 1,
        &id));
  }

  void visit(CZ &cz) override {
    if (!d_gateCZ) {
      const std::vector<std::complex<double>> h_gateCZ{1, 0, 0, 0, 0, 1, 0, 0,
                                                       0, 0, 1, 0, 0, 0, 0, -1};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateCZ, 16 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateCZ, h_gateCZ.data(),
                                   16 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateCZ);
    }

    auto bits = getBits(cz.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 2, bits.data(), d_gateCZ, nullptr, 1, 0, 1,
        &id));
  }

  void visit(Swap &s) override {
    if (!d_gateSwap) {
      const std::vector<std::complex<double>> h_gateSwap{
          1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateSwap, 16 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateSwap, h_gateSwap.data(),
                                   16 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateSwap);
    }

    auto bits = getBits(s.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 2, bits.data(), d_gateSwap, nullptr, 1, 0,
        1, &id));
  }

  void visit(iSwap &s) override {
    if (!d_gateiSwap) {
      const std::vector<std::complex<double>> h_gateiSwap{
          1, 0, 0, 0, 0, 0, i, 0, 0, i, 0, 0, 0, 0, 0, 1};
      HANDLE_CUDA_ERROR(cudaMalloc(&d_gateiSwap, 16 * (2 * fp64size)));
      HANDLE_CUDA_ERROR(cudaMemcpy(d_gateiSwap, h_gateiSwap.data(),
                                   16 * (2 * fp64size),
                                   cudaMemcpyHostToDevice));
      allocatedGates.push_back(d_gateiSwap);
    }

    auto bits = getBits(s.bits());
    HANDLE_CUTN_ERROR(cutensornetStateApplyTensorOperator(
        cutnHandle_, quantumState_, 2, bits.data(), d_gateiSwap, nullptr, 1, 0,
        1, &id));
  }

  void visit(Circuit &circuit) override{};

  ~CuTensorNetVisitor() {
    for (auto g : allocatedGates) {
      HANDLE_CUDA_ERROR(cudaFree(g));
    }
  }
};

} // namespace quantum
} // namespace xacc