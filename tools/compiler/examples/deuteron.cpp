#include "xacc_runtime.hpp"

__qpu__ void ansatz(qbit &q, double t0) {
  X(0);
  Ry(t0, 1);
  CX(1, 0);
}
__qpu__ void Z0(qbit &q, double t0) {
  ansatz(q, t0);
  Measure(0);
}

__qpu__ void Z1(qbit &q, double t0) {
  ansatz(q, t0);
  Measure(1);
}

__qpu__ void X0X1(qbit &q, double t0) {
  ansatz(q, t0);
  H(0);
  H(1);
  Measure(0);
  Measure(1);
}

__qpu__ void Y0Y1(qbit &q, double t0) {
  ansatz(q, t0);
  Rx(1.57, 0);
  Rx(1.57, 1);
  Measure(0);
  Measure(1);
}

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

  std::vector<double> sweep = linspace(-3.14, 3.14, 40);
  for (auto &t : sweep) {

    // Allocate a register of 2 qubits
    qbit q = xacc::qalloc(2);

    Z0(q, t);
    auto z0_exp = q->getExpectationValueZ();

    Z1(q, t);
    auto z1_exp = q->getExpectationValueZ();

    X0X1(q, t);
    auto x0x1_exp = q->getExpectationValueZ();

    Y0Y1(q, t);
    auto y0y1_exp = q->getExpectationValueZ();

    auto energy = 5.907 - 2.1433 * (x0x1_exp + y0y1_exp) + .21829 * z0_exp -
                  6.125 * z1_exp;

    std::cout << t << ", Exp: " << z0_exp << ", " << z1_exp << ", " << x0x1_exp
              << ", " << y0y1_exp << ", " << energy << "\n";
  }

  xacc::Finalize();

  return 0;
}