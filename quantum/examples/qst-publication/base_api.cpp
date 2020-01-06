#include "xacc.hpp"
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  // Get reference to the Accelerator
  auto accelerator = xacc::getAccelerator("tnqvm");

  // Get the IRProvider and create an
  // empty CompositeInstruction
  auto provider = xacc::getIRProvider("quantum");
  auto program = provider->createComposite("foo", {"t"});

  // Create X, Ry, CX, and Measure gates
  auto x = provider->createInstruction("X", {0});
  auto ry = provider->createInstruction("Ry", {1}, {"t"});
  auto cx = provider->createInstruction("CNOT", {1, 0});
  auto m0 = provider->createInstruction("Measure", {0});

  // Add them to the CompositeInstruction
  program->addInstructions({x, ry, cx, m0});

  // Loop over [-pi, pi] and compute <Z0>
  auto angles = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (auto &a : angles) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({a});
    accelerator->execute(buffer, evaled);
    std::cout << "<Z0>(" << a << ") = " << buffer->getExpectationValueZ()
              << "\n";
  }

  xacc::Finalize();
  return 0;
}
