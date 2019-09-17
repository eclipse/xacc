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
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "AllGateVisitor.hpp"
#include "xacc_service.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST(OQasmCompilerTester, checkCompileOQASM) {

    const std::string src("__qpu__ nonsense(AcceleratorBuffer qregister, double n) {\n"
        "   // nonsense\n"
        "   OPENQASM 2.0;\n"
        "   qreg q[3];\n"
        "   creg c[3];\n"
        "   // optional post-rotation for state tomography\n"
        //"gate post q { }\n"
        "   U(0.3,0.2,0.1) q[0];\n"
        "   U(3.14,0.2,0) q[1];\n"
        "   U(pi/2,n,n/2.3) q[1];\n"
        "   U(pi/2,n,0.1/3) q[2];\n"
        "   U(2*pi/3,n,0/2) q[1];\n"
        "   h q[1];\n"
        "   x q[0];\n"
        "   y q[2];\n"
        "   rx(3.14) q[1];\n"
        "   CX q[1],q[2];\n"
        "   barrier q;\n"
        "   CX q[0],q[1];\n"
        "   h q[0];\n"
        "   measure q[0] -> c[0];\n"
        "   measure q[1] -> c[1];\n"
        //"post q[2];\n"
        "   measure q[2] -> c[2];\n"
        "}\n"
        "__qpu__ morenonsense(AcceleratorBuffer ab, double n) {\n"
        "   // nonsense 2\n"
        "   OPENQASM 2.0;\n"
        "   nonsense(ab, n);\n"
        "   }\n"
        );

    auto compiler = xacc::getService<Compiler>("openqasm");
    auto ir = compiler->compile(src);

    EXPECT_TRUE(ir->getComposites().size() == 2);

    auto function = ir->getComposite("nonsense");
    std::cout << "HELLO\n" << function->toString() << "\n";
    std::cout << "N: " << function->nInstructions() << std::endl;

    EXPECT_TRUE(ir->getComposites().size() == 2);

    auto hadamardVisitor = std::make_shared<CountGatesOfTypeVisitor<Hadamard>>(function);
    auto cnotVisitor = std::make_shared<CountGatesOfTypeVisitor<CNOT>>(function);
    auto xVisitor = std::make_shared<CountGatesOfTypeVisitor<X>>(function);
    auto yVisitor = std::make_shared<CountGatesOfTypeVisitor<Y>>(function);
    auto uVisitor = std::make_shared<CountGatesOfTypeVisitor<U>>(function);
    auto rxVisitor = std::make_shared<CountGatesOfTypeVisitor<Rx>>(function);
    auto measureVisitor = std::make_shared<CountGatesOfTypeVisitor<Measure>>(function);

    EXPECT_TRUE(hadamardVisitor->countGates() == 2);
    EXPECT_TRUE(cnotVisitor->countGates() == 2);
    EXPECT_TRUE(xVisitor->countGates() == 1);
    EXPECT_TRUE(yVisitor->countGates() == 1);
    EXPECT_TRUE(uVisitor->countGates() == 5);
    EXPECT_TRUE(rxVisitor->countGates() == 1);
    EXPECT_TRUE(measureVisitor->countGates() == 3);

    // xacc::Finalize();
}

int main(int argc, char** argv) {
        xacc::Initialize();

    ::testing::InitGoogleTest(&argc, argv);
    auto ret =  RUN_ALL_TESTS();
    return ret;
}
