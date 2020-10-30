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
#include "py_optimizer.hpp"

void bind_optimizer(py::module &m) {
  // Expose Optimizer
  py::class_<xacc::Optimizer, std::shared_ptr<xacc::Optimizer>, PyOptimizer>(
      m, "Optimizer",
      "The Optimizer interface provides optimization routine implementations "
      "for use in algorithms.")
      .def(py::init<>(), "")
      .def("name", &xacc::Optimizer::name, "")
      .def(
          "optimize",
          [&](xacc::Optimizer &o, py::function &f,
              const int ndim) -> OptResult {
            OptFunction opt(
                [&](const std::vector<double> &x, std::vector<double> &grad) {
                  auto ret = f(x);
                  if (py::isinstance<py::tuple>(ret)) {
                    auto result =
                        ret.cast<std::pair<double, std::vector<double>>>();
                    for (int i = 0; i < grad.size(); i++) {
                      grad[i] = result.second[i];
                    }
                    return result.first;
                  } else {
                    return ret.cast<double>();
                  }
                },
                ndim);
            return o.optimize(opt);
          },
          "")
      .def(
          "optimize",
          [&](xacc::Optimizer &o, py::object &f) -> OptResult {
            if (!py::hasattr(f, "__call__")) {
              xacc::error(
                  "Invalid object passed to optimizer optimize, must have "
                  "__call__ implemented.");
            }
            if (!py::hasattr(f, "dimensions")) {
              xacc::error(
                  "Invalid object passed to optimizer optimize, must have "
                  "dimensions() implemented.");
            }
            OptFunction opt(
                [&](const std::vector<double> &x, std::vector<double> &grad) {
                  if (grad.empty()) {
                    return f.attr("__call__")(x).cast<double>();
                  } else {
                  auto result = f.attr("__call__")(x, grad).cast<std::pair<double, std::vector<double>>>();
                  grad = result.second;
                  return result.first;
                  }
                },
                f.attr("dimensions")().cast<int>());
            return o.optimize(opt);
          },
          "")
      .def("optimize", [](xacc::Optimizer &o) { return o.optimize(); })
      .def("setOptions",
           (void (xacc::Optimizer::*)(const HeterogeneousMap &)) &
               xacc::Optimizer::setOptions,
           "");

  py::class_<xacc::OptFunction>(m, "OptFunction", "")
      .def(py::init<std::function<double(const std::vector<double> &,
                                         std::vector<double> &)>,
                    const int>())
      .def("dimensions", &xacc::OptFunction::dimensions, "")
      .def(
          "__call__",
          [](OptFunction &o, const std::vector<double> &x) {
            std::vector<double> tmpgrad(o.dimensions());
            return o(x, tmpgrad);
          },
          "");
}