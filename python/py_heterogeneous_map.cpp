#include "py_heterogeneous_map.hpp"

void bind_heterogeneous_map(py::module &m) {
  py::class_<xacc::HeterogeneousMap>(m, "HeterogeneousMap", "")
      .def(py::init<>(), "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string, const int &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const double &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const std::string &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const std::vector<int> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const std::vector<double> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const std::vector<std::complex<double>> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(
               const std::string, const std::vector<std::pair<int, int>> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(
               const std::string, const std::shared_ptr<Observable> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(
               const std::string, const std::shared_ptr<Optimizer> &)) &
               xacc::HeterogeneousMap::insert,
           "")
     .def("insert",
           (void (xacc::HeterogeneousMap::*)(
               const std::string, const Eigen::MatrixXcd&)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def(
          "__getitem__",
          [](HeterogeneousMap &m,
             const std::string key) -> PyHeterogeneousMapTypes {
            if (m.keyExists<int>(key)) {
              return m.get<int>(key);
            } else if (m.keyExists<bool>(key)) {
              return m.get<bool>(key);
            } else if (m.keyExists<double>(key)) {
              return m.get<double>(key);
            } else if (m.stringExists(key)) {
              return m.getString(key);
            } else if (m.keyExists<std::vector<double>>(key)) {
              return m.get<std::vector<double>>(key);
            } else if (m.keyExists<std::vector<std::complex<double>>>(key)) {
              return m.get<std::vector<std::complex<double>>>(key);
            } else if (m.keyExists<std::vector<int>>(key)) {
              return m.get<std::vector<int>>(key);
            } else if (m.keyExists<std::shared_ptr<Observable>>(key)) {
              return m.get<std::shared_ptr<Observable>>(key);
            } else if (m.keyExists<std::shared_ptr<Optimizer>>(key)) {
              return m.get<std::shared_ptr<Optimizer>>(key);
            } else if (m.keyExists<std::vector<std::string>>(key)) {
              return m.get<std::vector<std::string>>(key);
            } else if (m.keyExists<Eigen::MatrixXcd>(key)) {
              return m.get<Eigen::MatrixXcd>(key);
            } else if (m.pointerLikeExists<CompositeInstruction>(key)) {
              return xacc::as_shared_ptr(
                  m.getPointerLike<CompositeInstruction>(key));
            } else {
              xacc::error("Invalid key for heterogeneous map");
              return 0;
            }
          },
          "")
      .def(
          "__contains__",
          [](HeterogeneousMap &m, const std::string key) {
            return m.keyExists<int>(key) || m.keyExists<double>(key) ||
                   m.keyExists<bool>(key) || m.stringExists(key) ||
                   m.keyExists<std::vector<double>>(key) ||
                   m.keyExists<std::vector<std::complex<double>>>(key) ||
                   m.keyExists<std::vector<int>>(key) ||
                   m.keyExists<std::vector<std::string>>(key) ||
                   m.keyExists<std::shared_ptr<Observable>>(key) ||
                   m.keyExists<std::shared_ptr<Optimizer>>(key) ||
                   m.keyExists<std::shared_ptr<Eigen::MatrixXcd>>(key) ||
                   m.pointerLikeExists<CompositeInstruction>(key);
          },
          "");

  py::class_<PyHeterogeneousMapTypes>(
      m, "PyHeterogeneousMap",
      "The PyHeterogeneousMap provides a variant structure "
      "to provide parameters to XACC HeterogeneousMaps."
      "This type can be an int, double, string, List[int], List[double], List[complex]"
      "List[string], Observable, Accelerator, Function, or Optimizer.")
      .def(py::init<int>(), "Construct as an int.")
      .def(py::init<bool>(), "Construct as a bool")
      .def(py::init<double>(), "Construct as a double.")
      .def(py::init<std::string>(), "Construct as a string.")
      .def(py::init<std::vector<std::string>>(), "Construct as a List[string].")
      .def(py::init<std::vector<int>>(), "Construct as a List[int].")
      .def(py::init<std::vector<double>>(), "Construct as a List[double].")
      .def(py::init<std::vector<std::complex<double>>>(), "Construct as a List[complex].")
      .def(py::init<std::shared_ptr<xacc::Accelerator>>(),
           "Construct as an Accelerator.")
      .def(py::init<std::shared_ptr<xacc::CompositeInstruction>>(),
           "Construct as a CompositeInstruction.")
      .def(py::init<std::shared_ptr<xacc::Instruction>>(),
           "Construct as a CompositeInstruction.")
      .def(py::init<std::shared_ptr<xacc::Observable>>(),
           "Construct as an Observable.")
      .def(py::init<std::shared_ptr<xacc::Optimizer>>(),
           "Construct as an Optimizer.")
      .def(py::init<Eigen::MatrixXcd>(),
           "Construct as an Eigen matrix.");
}