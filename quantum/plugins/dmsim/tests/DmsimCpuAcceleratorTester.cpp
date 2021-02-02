#include <gtest/gtest.h>
#include <string>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_observable.hpp"
#include "xacc_service.hpp"
//#include "Algorithm.hpp"

namespace {
    template <typename T>
    std::vector<T> linspace(T a, T b, size_t N)
    {
        T h = (b - a) / static_cast<T>(N - 1);
        std::vector<T> xs(N);
        typename std::vector<T>::iterator x;
        T val;
        for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
        {
            *x = val;
        }
        return xs;
    }
}

TEST(DmsimcpuAcceleratorTester, testBasic)
{
    auto accelerator = xacc::getAccelerator("dmsimcpu");
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz(qbit q, double t) {
      H(q[0]);
      CX(q[0], q[1]);
      CY(q[0], q[1]);
      CZ(q[1], q[0]);
      X(q[0]);
      Y(q[0]);
      Z(q[1]);
      Rz(q[0], 0.25);
      Rx(q[1], 0.25);
      Ry(q[0], 0.25);
      Swap(q[1], q[0]);
      CRZ(q[0], q[1], 0.25);
      CH(q[0],q[1]);
      S(q[0]);
      T(q[1]);
      Tdg(q[0]);
      U(q[1], 0.05, 0.15, 0.25);
    })", accelerator)->getComposites()[0];
    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, ir);
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
