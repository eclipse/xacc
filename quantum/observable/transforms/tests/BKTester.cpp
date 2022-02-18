#include <bits/c++config.h>
#include <gtest/gtest.h>
#include "BK.hpp"
#include "xacc.hpp"
#include <memory>
#include <regex>
#include "FermionOperator.hpp"
#include "PauliOperator.hpp"
#include "ObservableTransform.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"

using namespace xacc::quantum;

TEST(BravyiKitaevTransformationTester, checkBKTransform) {

  FermionOperator op("(3.17, 0) 2^ 0 + (3.17, 0) 0^ 2");
  auto fermion = std::make_shared<FermionOperator>(op);

  auto bk = xacc::getService<xacc::ObservableTransform>("bk");
  auto result = bk->transform(fermion);
  auto resultsStr = result->toString();

  PauliOperator pauli("(-1.585,0) Y0 Y1 + (-1.585,0) X0 X1 Z2");
  EXPECT_TRUE(
      std::dynamic_pointer_cast<PauliOperator>(result)->operator==(pauli));
}

TEST(BravyiKitaevTransformationTester, checkH2) {

  auto str = std::string(
      "(-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + "
      "(-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + "
      "(0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + "
      "(0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + "
      "(-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + "
      "(0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + "
      "(-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + "
      "(-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + "
      "(0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + "
      "(0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + "
      "(-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + "
      "(0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + "
      "(0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + "
      "(0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + "
      "(0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + "
      "(0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + "
      "(0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + "
      "(0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + "
      "(-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + "
      "(0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + "
      "(0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + "
      "(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + "
      "(0.7080240981,0)");

  auto acc = xacc::getAccelerator("qpp");

  auto bk = xacc::getService<xacc::ObservableTransform>("bk");
  auto H = bk->transform(xacc::quantum::getObservable("fermion", str));

  auto ansatz =
      xacc::createComposite("hf", {{"ne", 2}, {"nq", 4}, {"transform", "bk"}});

  auto vqe = xacc::getAlgorithm(
      "vqe", {{"ansatz", ansatz}, {"observable", H}, {"accelerator", acc}});

  auto q = xacc::qalloc(H->nBits());
  EXPECT_NEAR(vqe->execute(q, {})[0], -1.11633, 1e-4);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}