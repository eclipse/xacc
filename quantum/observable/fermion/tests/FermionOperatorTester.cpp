/***********************************************************************************
 * Copyright (c) 2019, UT-Battelle
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
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include <gtest/gtest.h>
#include "FermionOperator.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"

using namespace xacc::quantum;

TEST(FermionOperatorTester,checkSimple) {

  FermionOperator op(Operators{{3,1},{2,0}}, 2.2);

  EXPECT_TRUE(op == op);

  std::cout << "HI: " << (op*op).toString() << "\n";
  EXPECT_TRUE(op*op == FermionOperator());

  auto op2 = op + op;

  std::cout << op2.toString() << "\n";

  EXPECT_TRUE(-1.*op2 == -1.*(op+op));

}

TEST(FermionOperatorTester,checkFromStr) {

  std::string s("3.3 4^ 3^ 2^ 1^ 4 3 2 1");

  FermionOperator op(s);

  std::cout << op.toString() << "\n";

  std::string s2 = R"s2(0.7080240949826064
- 1.248846801817026 0^ 0
- 1.248846801817026 1^ 1
- 0.4796778151607899 2^ 2
- 0.4796778151607899 3^ 3 +
0.33667197218932576 0^ 1^ 1 0)s2";

FermionOperator op2(s2);

std::cout << op2.toString() << "\n";

}

TEST(FermionOperatorTester, checkFromStr2) {
    std::string src = ".12 0^ 1^ 0 1 - .12 0^ 1^ 1 0";
    FermionOperator op(src);
    std::cout << op.toString() << "\n";
}
int main(int argc, char** argv) {
    xacc::Initialize(argc,argv);
   ::testing::InitGoogleTest(&argc, argv);
   auto ret = RUN_ALL_TESTS();
   xacc::Finalize();
   return ret;
}
