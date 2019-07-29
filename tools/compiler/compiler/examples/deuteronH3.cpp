#include "xacc_runtime.hpp"
#include <iostream>
#include "nlopt.hpp"

__qpu__ void ansatz(qbit &q, double t0, double t1) {
  X(0);
  Ry(t0, 1);
  Ry(t1, 2);
  CX(2, 0);
  CX(0, 1);
  Ry(-t0, 1);
  CX(0, 1);
  CX(1, 0);
}

double objectiveFunction(const std::vector<double> &x,
                         std::vector<double> &grad, void *f_data) {
  auto hamiltonian = xacc::getObservable(
      "pauli", "5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
               "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2");
  qbit q = xacc::qalloc(3);
#pragma xacc observe hamiltonian
  ansatz(q, x[0], x[1]);
  return mpark::get<double>(q->getInformation("observation"));
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  auto dim = 2;
  nlopt::algorithm algo = nlopt::algorithm::LN_COBYLA;
  double tol = 1e-8;
  int maxeval = 30;

  nlopt::opt _opt(algo, dim);
  _opt.set_min_objective(objectiveFunction, NULL);
  _opt.set_lower_bounds(std::vector<double>(dim, -3.1415926));
  _opt.set_upper_bounds(std::vector<double>(dim, 3.1415926));
  _opt.set_maxeval(maxeval);
  _opt.set_ftol_rel(tol);

  double optF;
  std::vector<double> x(dim);
  try {
    auto result = _opt.optimize(x, optF);
  } catch (std::exception &e) {
    xacc::XACCLogger::instance()->error("NLOpt failed: " +
                                        std::string(e.what()));
  }

  std::cout << "Energy " << optF << "\n";

  xacc::Finalize();

  return 0;
}