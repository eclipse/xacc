#include "JW.hpp"
#include <memory>
#include "FermionOperator.hpp"
#include "PauliOperator.hpp"

namespace xacc {
namespace quantum {

std::shared_ptr<Observable>
JW::transform(std::shared_ptr<Observable> observable) {

  auto fermionObservable =
      std::dynamic_pointer_cast<FermionOperator>(observable);

  if (!fermionObservable) {
    XACCLogger::instance()->info(
        "Cannot execute Jordan Wigner on a non-fermion observable.");
    return observable;
  }

  auto result = std::make_shared<PauliOperator>();

  auto terms = fermionObservable->getTerms();

  // Loop over all Fermionic terms...
  for (auto &kv : terms) {

    auto coeff = kv.second.coeff();
    auto operators = kv.second.ops();

    PauliOperator current(coeff);
    for (auto &kv2 : operators) {
      std::map<int, std::string> zs;
      auto isCreation = kv2.second;

      int index = kv2.first;

      std::complex<double> ycoeff = isCreation ? std::complex<double>(0, -.5)
                                               : std::complex<double>(0, .5),
                           xcoeff(.5, 0);

      for (int j = 0; j < index; j++)
        zs.emplace(std::make_pair(j, "Z"));

      current *= PauliOperator(zs) * (PauliOperator({{index, "X"}}, xcoeff) +
                                      PauliOperator({{index, "Y"}}, ycoeff));
    }

    result->operator+=(current);
  }

  return result;
}

} // namespace quantum
} // namespace xacc