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
#include "AcceleratorDecorator.hpp"
#include "Instruction.hpp"
#include "xacc.hpp"
#include <gtest/gtest.h>
#include <memory>
#include "xacc_service.hpp"
#include "FermionOperator.hpp"

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

TEST(RDMPurificationDecoratorTester, checkGround) {

//   if (xacc::hasAccelerator("tnqvm")) {
    xacc::setOption("ibm-shots", "1024");
    xacc::setOption("u-p-depol", ".025");
    xacc::setOption("cx-p-depol", ".03");

    std::shared_ptr<Observable> op = std::make_shared<xacc::quantum::FermionOperator>(src);

    // Get the user-specified Accelerator,
    // or TNQVM if none specified
    auto accelerator = xacc::getAccelerator("qpp");
    int nQubits = 4;

    auto accd = xacc::getService<AcceleratorDecorator>("rdm-purification");
    accd->initialize({std::make_pair("fermion-observable", op)});
    accd->setDecorated(accelerator);

    auto buffer = xacc::qalloc(4);

    // xacc::setOption("rdm-source", src);
    // xacc::setOption("rdm-qubit-map", "0,1,6,5");

    // Create the UCCSD ansatz and evaluate
    // at the known optimal angles

    auto compiler = xacc::getService<xacc::Compiler>("xasm");

    auto ir = compiler->compile(rucc, accelerator);
    auto ruccsd = ir->getComposite("f");

    std::vector<double> parameters{.22984};

    ruccsd = (*ruccsd.get())(parameters);

    // auto function = xacc::getIRProvider("gate")->createFunction("f", {});
    // function->addInstruction(ruccsd);

    auto functions = op->observe(ruccsd);

    std::cout << "executing\n";
    accd->execute(buffer, functions);
    auto buffers = buffer->getChildren();
    std::cout << " Made it here\n";
    std::cout << "Purification Energy: "
              << mpark::get<double>(
                     buffers[0]->getInformation("purified-energy"))
              << "\n";

    // buffers[0]->print(std::cout);
    // std::cout << "EXPVAL: " << buffers[0]->getExpectationValueZ() << "\n";
    // EXPECT_NEAR(energy, -1.1371, 1e-4);
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
