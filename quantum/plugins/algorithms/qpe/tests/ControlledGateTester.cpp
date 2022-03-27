#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "CommonGates.hpp"
#include <Eigen/Dense>
#include "CountGatesOfTypeVisitor.hpp"
#include "GateFusion.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(ControlledGateTester, checkSimple) {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto t = std::make_shared<T>(0);
  auto rz = std::make_shared<Rz>(0, 3.1415);
  auto u1 = std::make_shared<U1>(0, 3.1415);
  auto x = std::make_shared<X>(0);
  const std::vector<size_t> expectedBits{1, 0};
  {
    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__X");
    comp->addInstruction(x->clone());
    auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    ctrlKernel->expand(
        {std::make_pair("U", comp), std::make_pair("control-idx", 1)});
    EXPECT_EQ(ctrlKernel->nInstructions(), 1);
    EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CNOT");
    EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CNOT");
    EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
  }
  {
    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__T");
    comp->addInstruction(t->clone());
    auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    ctrlKernel->expand(
        {std::make_pair("U", comp), std::make_pair("control-idx", 1)});
    // T -> CPhase(pi/4)
    EXPECT_EQ(ctrlKernel->nInstructions(), 1);
    EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CPhase");
    EXPECT_NEAR(ctrlKernel->getInstruction(0)->getParameter(0).as<double>(),
                M_PI_4, 1e-4);
    EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
  }
  {
    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__Rz");
    comp->addInstruction(rz->clone());
    auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    ctrlKernel->expand(
        {std::make_pair("U", comp), std::make_pair("control-idx", 1)});
    // Rz -> CRz
    EXPECT_EQ(ctrlKernel->nInstructions(), 1);
    EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CRZ");
    EXPECT_EQ(ctrlKernel->getInstruction(0)->getParameter(0),
              rz->getParameter(0));
    EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
  }
  {
    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__U1");
    comp->addInstruction(u1->clone());
    auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    ctrlKernel->expand(
        {std::make_pair("U", comp), std::make_pair("control-idx", 1)});
    // U1 -> CPhase (cU1)
    EXPECT_EQ(ctrlKernel->nInstructions(), 1);
    EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CPhase");
    EXPECT_EQ(ctrlKernel->getInstruction(0)->getParameter(0),
              u1->getParameter(0));
    EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
  }
}

TEST(ControlledGateTester, checkMultipleControl) {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto x = std::make_shared<X>(0);
  std::shared_ptr<xacc::CompositeInstruction> comp =
      gateRegistry->createComposite("__COMPOSITE__X");
  comp->addInstruction(x);
  auto ccx = std::dynamic_pointer_cast<CompositeInstruction>(
      xacc::getService<Instruction>("C-U"));
  const std::vector<int> ctrl_idxs{1, 2};
  ccx->expand({{"U", comp}, {"control-idx", ctrl_idxs}});
  std::cout << "HOWDY: \n" << ccx->toString() << "\n";

  // Test truth table
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 8192)});
  auto xGate0 = gateRegistry->createInstruction("X", {0});
  auto xGate1 = gateRegistry->createInstruction("X", {1});
  auto xGate2 = gateRegistry->createInstruction("X", {2});
  auto measureGate0 = gateRegistry->createInstruction("Measure", {0});
  auto measureGate1 = gateRegistry->createInstruction("Measure", {1});
  auto measureGate2 = gateRegistry->createInstruction("Measure", {2});

  const auto runTestCase = [&](bool in_bit0, bool in_bit1, bool in_bit2) {
    static int counter = 0;
    auto composite = gateRegistry->createComposite("__TEMP_COMPOSITE__" +
                                                   std::to_string(counter));
    counter++;
    // State prep:
    if (in_bit0) {
      composite->addInstruction(xGate0);
    }
    if (in_bit1) {
      composite->addInstruction(xGate1);
    }
    if (in_bit2) {
      composite->addInstruction(xGate2);
    }

    std::string inputBitString;
    inputBitString.append(in_bit0 ? "1" : "0");
    inputBitString.append(in_bit1 ? "1" : "0");
    inputBitString.append(in_bit2 ? "1" : "0");

    // Add ccx
    composite->addInstructions(ccx->getInstructions());
    // Mesurement:
    composite->addInstructions({measureGate0, measureGate1, measureGate2});

    auto buffer = xacc::qalloc(3);
    acc->execute(buffer, composite);
    std::cout << "Input bitstring: " << inputBitString << "\n";
    buffer->print();
    // CCNOT gate:
    const auto expectedBitString = [&inputBitString]() -> std::string {
      // If both control bits are 1's
      // q0q1q2
      if (inputBitString == "011") {
        return "111";
      }
      if (inputBitString == "111") {
        return "011";
      }
      // Otherwise, no changes
      return inputBitString;
    }();
    // Check bit string
    EXPECT_NEAR(buffer->computeMeasurementProbability(expectedBitString), 1.0,
                0.1);
  };

  // 3 bits: run all test cases (8)
  for (int i = 0; i < (1 << 3); ++i) {
    runTestCase(i & 0x001, i & 0x002, i & 0x004);
  }
}

TEST(ControlledGateTester, checkMultipleControlQregs) {
  // Reference circuit (index-based)
  const std::string ref_circuit_str = []() {
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto x = std::make_shared<X>(0);
    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__X");
    comp->addInstruction(x);
    auto ccx = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    const std::vector<int> ctrl_idxs{1, 2};
    ccx->expand({{"U", comp}, {"control-idx", ctrl_idxs}});
    return ccx->toString();
  }();

  // Circuit with different buffer names.
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto x = std::make_shared<X>(0);
  x->setBufferNames({"target"});
  std::shared_ptr<xacc::CompositeInstruction> comp =
      gateRegistry->createComposite("__COMPOSITE__X");
  comp->addInstruction(x);
  auto ccx = std::dynamic_pointer_cast<CompositeInstruction>(
      xacc::getService<Instruction>("C-U"));

  const std::vector<std::pair<std::string, size_t>> ctrl_qubits{
      {"control1", 0}, {"control2", 0}};

  ccx->expand({{"U", comp}, {"control-idx", ctrl_qubits}});
  auto new_circ_str = ccx->toString();
  const auto replaceAll = [](const std::string &t, const std::string &s,
                             std::string &str) {
    std::string::size_type n = 0;
    while ((n = str.find(s, n)) != std::string::npos) {
      str.replace(n, s.size(), t);
      n += t.size();
    }
  };

  replaceAll("q0", "target0", new_circ_str);
  replaceAll("q1", "control10", new_circ_str);
  replaceAll("q2", "control20", new_circ_str);
  std::cout << "HOWDY:\n" << new_circ_str << "\n";
  EXPECT_EQ(ref_circuit_str, new_circ_str);
}

TEST(ControlledGateTester, checkControlSwap) {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto swap = std::make_shared<Swap>(0, 1);
  std::shared_ptr<xacc::CompositeInstruction> comp =
      gateRegistry->createComposite("__COMPOSITE__Swap");
  comp->addInstruction(swap);
  auto cswap = std::dynamic_pointer_cast<CompositeInstruction>(
      xacc::getService<Instruction>("C-U"));
  const std::vector<int> ctrl_idxs{2};
  cswap->expand({{"U", comp}, {"control-idx", ctrl_idxs}});
  std::cout << "HOWDY: \n" << cswap->toString() << "\n";
  // 2 CNOT + CCNOT (15 gates)
  EXPECT_EQ(cswap->nInstructions(), 15 + 2);
  // Test truth table
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 1024)});
  auto xGate0 = gateRegistry->createInstruction("X", {0});
  auto xGate1 = gateRegistry->createInstruction("X", {1});
  auto xGate2 = gateRegistry->createInstruction("X", {2});
  auto measureGate0 = gateRegistry->createInstruction("Measure", {0});
  auto measureGate1 = gateRegistry->createInstruction("Measure", {1});
  auto measureGate2 = gateRegistry->createInstruction("Measure", {2});

  const auto runTestCase = [&](bool in_bit0, bool in_bit1, bool in_bit2) {
    static int counter = 0;
    auto composite = gateRegistry->createComposite("__TEMP_COMPOSITE__" +
                                                   std::to_string(counter));
    counter++;
    // State prep:
    if (in_bit0) {
      composite->addInstruction(xGate0);
    }
    if (in_bit1) {
      composite->addInstruction(xGate1);
    }
    if (in_bit2) {
      composite->addInstruction(xGate2);
    }

    std::string inputBitString;
    inputBitString.append(in_bit0 ? "1" : "0");
    inputBitString.append(in_bit1 ? "1" : "0");
    inputBitString.append(in_bit2 ? "1" : "0");

    // Add cswap
    composite->addInstructions(cswap->getInstructions());
    // Mesurement:
    composite->addInstructions({measureGate0, measureGate1, measureGate2});

    auto buffer = xacc::qalloc(3);
    acc->execute(buffer, composite);
    std::cout << "Input bitstring: " << inputBitString << "\n";
    buffer->print();
    // CSWAP gate:
    const auto expectedBitString = [&inputBitString]() -> std::string {
      // If q2 is 1 and q0 and q1 are different ==> swap
      // q0q1q2
      if (inputBitString == "011") {
        return "101";
      }
      if (inputBitString == "101") {
        return "011";
      }
      // Otherwise, no changes
      return inputBitString;
    }();
    // Check bit string
    EXPECT_NEAR(buffer->computeMeasurementProbability(expectedBitString), 1.0,
                0.1);
  };

  // 3 bits: run all test cases (8)
  for (int i = 0; i < (1 << 3); ++i) {
    runTestCase(i & 0x001, i & 0x002, i & 0x004);
  }
}

TEST(ControlledGateTester, checkEltonBug) {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  {
    auto z = std::make_shared<Z>(4);
    z->setBufferNames(std::vector<std::string>{"q"});

    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__Z");
    comp->addInstruction(z);
    auto multi_ctrl_z = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    const std::vector<int> ctrl_idxs{0, 1, 2, 3};
    multi_ctrl_z->expand({{"U", comp}, {"control-idx", ctrl_idxs}});

    // std::cout << "HOWDY: \n" << multi_ctrl_z->toString() << "\n";
    std::cout << multi_ctrl_z->nInstructions() << "\n";

    xacc::quantum::CountGatesOfTypeVisitor<CNOT> vis(multi_ctrl_z);
    xacc::quantum::CountGatesOfTypeVisitor<U> visu(multi_ctrl_z);

    // QiskitImpl Reports: OrderedDict([('cx', 44), ('p', 30), ('u', 30)])
    // all ours are U+CX, and we remove U(0,0,0).
    // There are 8 U(0,0,0). So should have 44 CX, 60-8=52 Us
    EXPECT_EQ(vis.countGates(), 44);
    EXPECT_EQ(visu.countGates(), 52);
  }

  {
    auto z = std::make_shared<Z>(6);
    z->setBufferNames(std::vector<std::string>{"q"});

    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__Z");
    comp->addInstruction(z);
    auto multi_ctrl_z = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    const std::vector<int> ctrl_idxs{0, 1, 2, 3, 4, 5};
    multi_ctrl_z->expand({{"U", comp}, {"control-idx", ctrl_idxs}});

    // std::cout << "HOWDY: \n" << multi_ctrl_z->toString() << "\n";
    std::cout << multi_ctrl_z->nInstructions() << "\n";

    xacc::quantum::CountGatesOfTypeVisitor<CNOT> vis(multi_ctrl_z);
    xacc::quantum::CountGatesOfTypeVisitor<U> visu(multi_ctrl_z);
    // std::cout << vis.countGates() << "\n";
    // OrderedDict([('cx', 188), ('p', 126), ('u', 126)])
    // all ours are U+CX, and we remove U(0,0,0).
    // There are 32 U(0,0,0). So should have ?? CX, 252-32=220 Us
    EXPECT_EQ(vis.countGates(), 188);
    EXPECT_EQ(visu.countGates(), 220);
  }

    {
    auto x = std::make_shared<X>(6);
    x->setBufferNames(std::vector<std::string>{"q"});

    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__X");
    comp->addInstruction(x);
    auto multi_ctrl_z = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    const std::vector<int> ctrl_idxs{0, 1, 2, 3, 4, 5};
    multi_ctrl_z->expand({{"U", comp}, {"control-idx", ctrl_idxs}});

    // std::cout << "HOWDY: \n" << multi_ctrl_z->toString() << "\n";
    std::cout << multi_ctrl_z->nInstructions() << "\n";

    xacc::quantum::CountGatesOfTypeVisitor<CNOT> vis(multi_ctrl_z);
    xacc::quantum::CountGatesOfTypeVisitor<U> visu(multi_ctrl_z);
    // std::cout << vis.countGates() << "\n";
    // OrderedDict([('cx', 188), ('p', 126), ('u', 126)])
    // all ours are U+CX, and we remove U(0,0,0).
    // There are 32 U(0,0,0). So should have ?? CX, 252-63=220 Us
    EXPECT_EQ(vis.countGates(), 188);
    EXPECT_EQ(visu.countGates(), 220);
  }
}

TEST(ControlledGateTester, checkMultipleControlGrayCodeGen) {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto x = std::make_shared<X>(0);
  std::shared_ptr<xacc::CompositeInstruction> comp =
      gateRegistry->createComposite("__COMPOSITE__X");
  comp->addInstruction(x);
  auto mcx = std::dynamic_pointer_cast<CompositeInstruction>(
      xacc::getService<Instruction>("C-U"));
  const std::vector<int> ctrl_idxs{1, 2, 3, 4};
  mcx->expand({{"U", comp}, {"control-idx", ctrl_idxs}});
  std::cout << "HOWDY: \n" << mcx->toString() << "\n";
  Eigen::MatrixXcd uMat = GateFuser::fuseGates(mcx, 5);
  for (int i = 0; i < 1 << 5; ++i) {
    for (int j = 0; j < 1 << 5; ++j) {
      if (std::abs(uMat(i, j)) > 1e-12) {
        // std::cout << i << " " << j << ": " << uMat(i, j) << "\n";
        // Validate no phase value mismatches.
        EXPECT_NEAR(std::abs(uMat(i, j) - 1.0), 0.0, 1e-9);
      }
    }
  }
  // Test truth table
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 8192)});
  auto xGate0 = gateRegistry->createInstruction("X", {0});
  auto xGate1 = gateRegistry->createInstruction("X", {1});
  auto xGate2 = gateRegistry->createInstruction("X", {2});
  auto xGate3 = gateRegistry->createInstruction("X", {3});
  auto xGate4 = gateRegistry->createInstruction("X", {4});
  auto measureGate0 = gateRegistry->createInstruction("Measure", {0});
  auto measureGate1 = gateRegistry->createInstruction("Measure", {1});
  auto measureGate2 = gateRegistry->createInstruction("Measure", {2});
  auto measureGate3 = gateRegistry->createInstruction("Measure", {3});
  auto measureGate4 = gateRegistry->createInstruction("Measure", {4});

  const auto runTestCase = [&](bool in_bit0, bool in_bit1, bool in_bit2,
                               bool in_bit3, bool in_bit4) {
    static int counter = 0;
    auto composite = gateRegistry->createComposite("__TEMP_COMPOSITE__" +
                                                   std::to_string(counter));
    counter++;
    // State prep:
    if (in_bit0) {
      composite->addInstruction(xGate0);
    }
    if (in_bit1) {
      composite->addInstruction(xGate1);
    }
    if (in_bit2) {
      composite->addInstruction(xGate2);
    }
    if (in_bit3) {
      composite->addInstruction(xGate3);
    }
    if (in_bit4) {
      composite->addInstruction(xGate4);
    }

    std::string inputBitString;
    inputBitString.append(in_bit0 ? "1" : "0");
    inputBitString.append(in_bit1 ? "1" : "0");
    inputBitString.append(in_bit2 ? "1" : "0");
    inputBitString.append(in_bit3 ? "1" : "0");
    inputBitString.append(in_bit4 ? "1" : "0");

    // Add mcx
    composite->addInstructions(mcx->getInstructions());
    // Mesurement:
    composite->addInstructions(
        {measureGate0, measureGate1, measureGate2, measureGate3, measureGate4});

    auto buffer = xacc::qalloc(5);
    acc->execute(buffer, composite);
    // std::cout << "Input bitstring: " << inputBitString << "\n";
    // buffer->print();
    // MCX gate:
    const auto expectedBitString = [&inputBitString]() -> std::string {
      // If all control bits are 1's
      // q0q1q2q3q4
      if (inputBitString == "01111") {
        return "11111";
      }
      if (inputBitString == "11111") {
        return "01111";
      }
      // Otherwise, no changes
      return inputBitString;
    }();
    // Check bit string
    EXPECT_NEAR(buffer->computeMeasurementProbability(expectedBitString), 1.0,
                0.1);
  };

  // 5 bits: run all test cases (8)
  for (int i = 0; i < (1 << 5); ++i) {
    runTestCase(i & 0x001, i & 0x002, i & 0x004, i & 0x008, i & 0x010);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
