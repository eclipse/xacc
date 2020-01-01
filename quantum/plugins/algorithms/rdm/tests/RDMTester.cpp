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
#include "Instruction.hpp"
#include "xacc.hpp"
#include <gtest/gtest.h>
#include "xacc_service.hpp"
#include "FermionOperator.hpp"
#include <unsupported/Eigen/CXX11/Tensor>

using namespace xacc;

const std::string src = R"src(0.7080240949826064
- 1.248846801817026 0^ 0
- 1.248846801817026 1^ 1
- 0.4796778151607899 2^ 2
- 0.4796778151607899 3^ 3
+ 0.33667197218932576 0^ 1^ 1 0
+ 0.0908126658307406 0^ 1^ 3 2
+ 0.09081266583074038 0^ 2^ 0 2
+ 0.331213646878486 0^ 2^ 2 0
+ 0.09081266583074038 0^ 3^ 1 2
+ 0.331213646878486 0^ 3^ 3 0
+ 0.33667197218932576 1^ 0^ 0 1
+ 0.0908126658307406 1^ 0^ 2 3
+ 0.09081266583074038 1^ 2^ 0 3
+ 0.331213646878486 1^ 2^ 2 1
+ 0.09081266583074038 1^ 3^ 1 3
+ 0.331213646878486 1^ 3^ 3 1
+ 0.331213646878486 2^ 0^ 0 2
+ 0.09081266583074052 2^ 0^ 2 0
+ 0.331213646878486 2^ 1^ 1 2
+ 0.09081266583074052 2^ 1^ 3 0
+ 0.09081266583074048 2^ 3^ 1 0
+ 0.34814578469185886 2^ 3^ 3 2
+ 0.331213646878486 3^ 0^ 0 3
+ 0.09081266583074052 3^ 0^ 2 1
+ 0.331213646878486 3^ 1^ 1 3
+ 0.09081266583074052 3^ 1^ 3 1
+ 0.09081266583074048 3^ 2^ 0 1
+ 0.34814578469185886 3^ 2^ 2 3)src";

const std::string rucc = R"rucc(__qpu__ void f(qbit q, double t0) {
    X(q[0]);
    X(q[1]);
    Rx(q[0],1.5707);
    H(q[1]);
    H(q[2]);
    H(q[3]);
    CNOT(q[0],q[1]);
    CNOT(q[1],q[2]);
    CNOT(q[2],q[3]);
    Rz(q[3], t0);
    CNOT(q[2],q[3]);
    CNOT(q[1],q[2]);
    CNOT(q[0],q[1]);
    Rx(q[0],-1.5707);
    H(q[1]);
    H(q[2]);
    H(q[3]);
})rucc";

TEST(RDMGeneratorTester, checkGround) {

//   if (xacc::hasAccelerator("tnqvm")) {
    // Get the user-specified Accelerator,
    // or TNQVM if none specified
    auto accelerator = xacc::getAccelerator("qpp");
    int nQubits = 4;

    xacc::quantum::FermionOperator op(src);
    Eigen::Tensor<double,2> hpq(4,4); hpq.setZero();
    Eigen::Tensor<double,4> hpqrs(4,4,4,4);hpqrs.setZero();
    double enuc = 0.0;

    auto terms = op.getTerms();
    for (auto& kv : terms) {
        auto ops = kv.second.ops();
        if (ops.size() == 4) {
            hpqrs(ops[0].first,ops[1].first,ops[2].first, ops[3].first) = std::real(kv.second.coeff());
        } else if (ops.size() == 2) {
            hpq(ops[0].first,ops[1].first) = std::real(kv.second.coeff());
        } else {
            enuc = std::real(kv.second.coeff());
        }
    }

    // Create the UCCSD ansatz and evaluate
    // at the known optimal angles
    auto compiler = xacc::getCompiler("xasm");

    auto ir2 = compiler->compile(rucc, accelerator);
    auto ruccsd = ir2->getComposite("f");

    std::vector<double> parameters{.22984};
    ruccsd = (*ruccsd.get())(parameters);

    // Create the 2-RDM
    // std::vector<int> qubitMap {1,3,5,7}; // map to physical qubits
    // ruccsd->mapBits(qubitMap);
     auto rdmGen = xacc::getAlgorithm("rdm");
     EXPECT_TRUE(rdmGen->initialize({std::make_pair("accelerator",accelerator ), std::make_pair("ansatz", ruccsd)}));
     auto buffer = xacc::qalloc(4);
     rdmGen->execute(buffer);
    // auto buffers = generator.generate(ruccsd, qubitMap);

    auto data = buffer->getInformation("2-rdm").as<std::vector<double>>();

    // EXPECT_EQ(buffers.size(), 54);
     Eigen::TensorMap<Eigen::Tensor<double,4>> rho_pqrs(data.data(), 4,4,4,4);

    // // Get the 1 rdm from the 2 rdm
    Eigen::array<int, 2> cc2({1, 3});
    Eigen::Tensor<double, 2> rho_pq = rho_pqrs.trace(cc2);

    double energy = enuc;
    // Compute the 2 rdm contribution to the energy
    for (int p = 0; p < nQubits; p++) {
      for (int q = 0; q < nQubits; q++) {
        for (int r = 0; r < nQubits; r++) {
          for (int s = 0; s < nQubits; s++) {
            energy +=
                0.5 * std::real(hpqrs(p, q, r, s) *
                                (rho_pqrs(p, q, s, r) + rho_pqrs(r, s, q, p)));
          }
        }
      }
    }

    // Compute the 1 rdm contribution to the energy
    for (int p = 0; p < nQubits; p++) {
      for (int q = 0; q < nQubits; q++) {
        energy += 0.5 * std::real(hpq(p, q) * (rho_pq(p, q) + rho_pq(q, p)));
      }
    }
    std::cout << "ENERGY: " << energy << "\n";

    // // auto energy = rhoGen.energy();
    EXPECT_NEAR(energy, -1.1371, 1e-4);
//   }
}

int main(int argc, char **argv) {
  xacc::Initialize();
  int ret = 0;
  ::testing::InitGoogleTest(&argc, argv);
  ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
