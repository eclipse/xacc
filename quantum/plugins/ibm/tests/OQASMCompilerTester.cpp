/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial implementation - H. Charles Zhao
 *
 **********************************************************************************/
#include <gtest/gtest.h>
#include "XACC.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "AllGateVisitor.hpp"
#include "xacc_service.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST(OQasmCompilerTester, checkCompileOQASM) {
    xacc::Initialize();

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

    EXPECT_TRUE(ir->getKernels().size() == 2);

    auto function = ir->getKernel("nonsense");
    std::cout << "HELLO\n" << function->toString("qregister") << "\n";
    std::cout << "N: " << function->nInstructions() << std::endl;

    EXPECT_TRUE(ir->getKernels().size() == 2);

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

    xacc::Finalize();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
