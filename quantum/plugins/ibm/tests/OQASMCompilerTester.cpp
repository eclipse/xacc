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

    const std::string src("__qpu__ void nonsense(qbit qregister, double n) {\n"
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

        );

    auto compiler = xacc::getService<Compiler>("openqasm");
    auto ir = compiler->compile(src);

    EXPECT_TRUE(ir->getComposites().size() == 1);

    auto function = ir->getComposite("nonsense");
    std::cout << "HELLO\n" << function->toString() << "\n";
    std::cout << "N: " << function->nInstructions() << std::endl;

    EXPECT_TRUE(ir->getComposites().size() == 1);

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

TEST(OQasmCompilerTester, checkComplex) {

    const std::string src = R"src(include "qelib1.inc";
qreg q[4];
creg c[4];
u3(3.442869371886, -pi/2, pi/2) q[0];
u1(2.671120676205) q[0];
u3(1.540494492206, -pi/2, pi/2) q[1];
u1(3.290440112129) q[1];
u3(0.022294485029, -pi/2, pi/2) q[2];
u1(0.436279645426) q[2];
u3(3.572738737062, -pi/2, pi/2) q[3];
u1(4.867597859102) q[3];
cx q[0], q[1];
cx q[0], q[2];
cx q[0], q[3];
u1(2.58848772445) q[0];
u3(0.194413641661, -pi/2, pi/2) q[0];
u1(3.639810972471) q[0];
u1(4.912787893839) q[1];
u3(3.079155348405, -pi/2, pi/2) q[1];
u1(0.04721288765) q[1];
u1(0.515801553473) q[2];
u3(3.574647607134, -pi/2, pi/2) q[2];
u1(0.68888718156) q[2];
u1(3.832419100678) q[3];
u3(5.969537803768, -pi/2, pi/2) q[3];
u1(4.238742400331) q[3];
cx q[0], q[1];
cx q[0], q[2];
cx q[0], q[3];
u1(1.98824294549) q[0];
u3(0.507174280394, -pi/2, pi/2) q[0];
u1(4.537931482297) q[1];
u3(1.540871188987, -pi/2, pi/2) q[1];
u1(0.78718157196) q[2];
u3(5.472312765418, -pi/2, pi/2) q[2];
u1(2.028198126855) q[3];
u3(1.059840546465, -pi/2, pi/2) q[3];
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3];)src";
}
int main(int argc, char** argv) {
        xacc::Initialize();

    ::testing::InitGoogleTest(&argc, argv);
    auto ret =  RUN_ALL_TESTS();
    return ret;
}
