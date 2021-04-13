/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "xacc.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "NoiseModel.hpp"
#include <memory>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

namespace py = pybind11;
using namespace xacc;

namespace pybind11 {
namespace detail {
template <typename... Ts>
struct type_caster<Variant<Ts...>> : variant_caster<Variant<Ts...>> {};

template <> struct visit_helper<Variant> {
  template <typename... Args>
  static auto call(Args &&... args) -> decltype(mpark::visit(args...)) {
    return mpark::visit(args...);
  }
};

template <typename... Ts>
struct type_caster<mpark::variant<Ts...>>
    : variant_caster<mpark::variant<Ts...>> {};

template <> struct visit_helper<mpark::variant> {
  template <typename... Args>
  static auto call(Args &&... args) -> decltype(mpark::visit(args...)) {
    return mpark::visit(args...);
  }
};
} // namespace detail
} // namespace pybind11

// We can't pass HeterogeneousMap back and forth effectively,
// so here we define a variant of common input types and an
// associated map to fake like it is a HeterogeneousMap
using PyHeterogeneousMapTypes =
    xacc::Variant<bool, int, double, std::string, std::vector<std::string>,
                  std::vector<std::complex<double>>, std::vector<double>, std::vector<int>, std::complex<double>, 
                  std::shared_ptr<CompositeInstruction>, std::vector<std::pair<int,int>>, std::shared_ptr<IRTransformation>,
                  std::shared_ptr<Instruction>, std::shared_ptr<Accelerator>, std::shared_ptr<AlgorithmGradientStrategy>,
                  std::shared_ptr<Observable>, std::shared_ptr<Optimizer>, Eigen::MatrixXcd, std::shared_ptr<Graph>, std::shared_ptr<NoiseModel>>;
using PyHeterogeneousMap = std::map<std::string, PyHeterogeneousMapTypes>;

// Visitor used to map PyHeterogeneousMap to HeterogeneousMap
class PyHeterogeneousMap2HeterogeneousMap {
protected:
  HeterogeneousMap &m;
  const std::string &key;

public:
  PyHeterogeneousMap2HeterogeneousMap(HeterogeneousMap &map,
                                      const std::string &k)
      : m(map), key(k) {}
  template <typename T> void operator()(const T &t) { m.insert(key, t); }
};

// Visitor used to map HeterogeneousMap to PyHeterogeneousMap
class HeterogeneousMap2PyHeterogeneousMap
    : public visitor_base<
          bool, int, double, std::string, std::vector<std::string>,
          std::vector<double>, std::vector<int>, std::complex<double>, std::vector<std::complex<double>>,
          std::shared_ptr<CompositeInstruction>, std::shared_ptr<Instruction>,
          std::shared_ptr<Accelerator>, std::shared_ptr<Observable>,
          std::shared_ptr<Optimizer>, std::shared_ptr<AlgorithmGradientStrategy>,
          Eigen::MatrixXcd, std::shared_ptr<Graph>, std::shared_ptr<NoiseModel>> {
private:
  PyHeterogeneousMap &pymap;

public:
  HeterogeneousMap2PyHeterogeneousMap(PyHeterogeneousMap &m) : pymap(m) {}

  template <typename T> void operator()(const std::string &key, const T &t) {
    pymap.insert({key, t});
  }
};

void bind_heterogeneous_map(py::module& m);
