#include "xacc.hpp"

template <typename T> std::vector<T> linspace(T a, T b, size_t N) {
  T h = (b - a) / static_cast<T>(N - 1);
  std::vector<T> xs(N);
  typename std::vector<T>::iterator x;
  T val;
  for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
    *x = val;
  return xs;
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto accelerator = xacc::getAccelerator("tnqvm");

  auto provider = xacc::getIRProvider("quantum");
  auto program = provider->createComposite("ansatz", {"t"});
  auto x = provider->createInstruction("X", std::vector<std::size_t>{0});
  auto ry = provider->createInstruction("Ry", std::vector<std::size_t>{1}, {"t"});
  auto cx = provider->createInstruction("CNOT", std::vector<std::size_t>{1,0});
  auto h0 = provider->createInstruction("H", std::vector<std::size_t>{0});
  auto h1 = provider->createInstruction("H", std::vector<std::size_t>{1});
  auto m0 = provider->createInstruction("Measure", std::vector<std::size_t>{0});
  auto m1 = provider->createInstruction("Measure", std::vector<std::size_t>{1});
  program->addInstructions({x,ry,cx,h0,h1,m0,m1});

  std::cout << "P:\n" << program->toString() << "\n";
  auto angles = linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (auto &a : angles) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({a});
    accelerator->execute(buffer, evaled);
    std::cout << "<X0X1>(theta) = " << buffer->getExpectationValueZ() << "\n";
  }

  xacc::Finalize();

  return 0;
}