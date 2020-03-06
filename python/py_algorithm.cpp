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
#include "py_algorithm.hpp"
#include "Algorithm.hpp"
#include "py_heterogeneous_map.hpp"

void bind_algorithm(py::module &m) {

  py::class_<xacc::Algorithm, std::shared_ptr<xacc::Algorithm>, PyAlgorithm>
      alg(m, "Algorithm",
          "The XACC Algorithm interface takes as input a dictionary of "
          "AlgorithmParameters "
          "and executes the desired Algorithm.");

  alg.def(py::init<>())
      .def("name", &xacc::Algorithm::name, "Return the name of this Algorithm.")
      .def("execute",
           (void (xacc::Algorithm::*)(
               const std::shared_ptr<xacc::AcceleratorBuffer>) const) &
               xacc::Algorithm::execute,
           "Execute the Algorithm, storing the results in provided "
           "AcceleratorBuffer.")
      .def("execute",
           (std::vector<double>(xacc::Algorithm::*)(
               const std::shared_ptr<xacc::AcceleratorBuffer>,
               const std::vector<double> &)) &
               xacc::Algorithm::execute,
           "Execute the Algorithm, storing the results in provided "
           "AcceleratorBuffer.")
      .def(
          "calculate",
          [](Algorithm &a, const std::string task,
             const std::shared_ptr<AcceleratorBuffer> b,
             PyHeterogeneousMap &params) {
            HeterogeneousMap m;
            for (auto &item : params) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            return a.calculate(task, b, m);
          },
          "")
      .def(
          "calculate",
          [](Algorithm &a, const std::string task,
             const std::shared_ptr<AcceleratorBuffer> b) {
            return a.calculate(task, b);
          },
          "")
      .def("initialize", [](Algorithm& a, PyHeterogeneousMap& options) {
            HeterogeneousMap m;
            for (auto &item : options) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            return a.initialize(m);
      },
           "Initialize the algorithm with given AlgorithmParameters.")
      .def("clone", &xacc::Algorithm::clone, "");
}