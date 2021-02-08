/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "JW.hpp"
#include <memory>
#include "FermionOperator.hpp"
#include "PauliOperator.hpp"
#include "xacc_observable.hpp"

namespace xacc {
namespace quantum {
template <typename T>
bool ptr_is_a(std::shared_ptr<Observable> ptr) {
  return std::dynamic_pointer_cast<T>(ptr) != nullptr;
}
std::shared_ptr<Observable>
JW::transform(std::shared_ptr<Observable> Hptr_input) {

  // First we pre-process the observable to a FermionOperator
  std::shared_ptr<Observable> observable;
  auto obs_str = Hptr_input->toString();
  if (ptr_is_a<FermionOperator>(Hptr_input)) {  
    observable = Hptr_input;
  } else if (obs_str.find("^") != std::string::npos) {
    observable = xacc::quantum::getObservable("fermion", obs_str);
  } else {
    XACCLogger::instance()->error("[Jordan Wigner] Error, cannot cast incoming Observable ptr to something we can process.");
  }

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

    auto var = kv.second.var();
    auto coeff = kv.second.coeff();
    auto operators = kv.second.ops();

    PauliOperator current(coeff, var);
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