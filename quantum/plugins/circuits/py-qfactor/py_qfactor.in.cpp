#include "py_qfactor.hpp"
#include "xacc.hpp"
#include <dlfcn.h>

#include "xacc_plugin.hpp"

#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>

namespace xacc {

namespace circuits {

// If the matrix is finite: no NaN elements
template <typename Derived>
inline bool isFinite(const Eigen::MatrixBase<Derived> &x) {
  return ((x - x).array() == (x - x).array()).all();
}

inline bool isSquare(const Eigen::MatrixXcd &in_mat) {
  return in_mat.rows() == in_mat.cols();
}

// Default tolerace for validation
constexpr double TOLERANCE = 1e-6;

bool allClose(const Eigen::MatrixXcd &in_mat1, const Eigen::MatrixXcd &in_mat2,
              double in_tol = TOLERANCE) {
  if (!isFinite(in_mat1) || !isFinite(in_mat2)) {
    return false;
  }

  if (in_mat1.rows() == in_mat2.rows() && in_mat1.cols() == in_mat2.cols()) {
    for (int i = 0; i < in_mat1.rows(); ++i) {
      for (int j = 0; j < in_mat1.cols(); ++j) {
        if (std::abs(in_mat1(i, j) - in_mat2(i, j)) > in_tol) {
          return false;
        }
      }
    }

    return true;
  }
  return false;
}

bool isUnitary(const Eigen::MatrixXcd &in_mat) {
  if (!isSquare(in_mat) || !isFinite(in_mat)) {
    return false;
  }

  Eigen::MatrixXcd Id =
      Eigen::MatrixXcd::Identity(in_mat.rows(), in_mat.cols());

  return allClose(in_mat * in_mat.adjoint(), Id);
}

PyQfactor::~PyQfactor() {
  if (libpython_handle) {
    int i = dlclose(libpython_handle);
    if (i != 0) {
      std::cout << "error closing python lib in qfactor: " << i << "\n";
      std::cout << dlerror() << "\n";
    }
  }
}
void PyQfactor::initialize() {
  if (!guard && !Py_IsInitialized()) {
    guard = std::make_shared<py::scoped_interpreter>();
    libpython_handle = dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
    initialized = true;
  }
}

bool PyQfactor::expand(const xacc::HeterogeneousMap &parameters) {

  Eigen::MatrixXcd unitary;
  if (parameters.keyExists<Eigen::MatrixXcd>("unitary")) {
    unitary = parameters.get<Eigen::MatrixXcd>("unitary");
  }
  const auto nbQubits = [](size_t unitaryDim) {
    unsigned int ret = 0;
    while (unitaryDim > 1) {
      unitaryDim >>= 1;
      ret++;
    }

    return ret;
  };
  auto n_qubits = nbQubits(unitary.rows());
  if (!isUnitary(unitary)) {
    xacc::error("Input matrix is not a unitary matrix");
    return false;
  }
  if (!initialized) {
    initialize();
  }

  auto locals = py::dict();
  locals["unitary"] = unitary;
  
  // Connectivity information
  std::vector<std::pair<int, int>> connectivity;
  if (parameters.keyExists<std::vector<std::pair<int, int>>>("connectivity")) {
    connectivity = parameters.get<std::vector<std::pair<int, int>>>("connectivity");
  }
  // If no connectivity constraints, assume all to all:
  if (connectivity.empty()) {
    for (int i = 0; i < n_qubits; ++i) {
      for (int j = i + 1; j < n_qubits; ++j) {
        connectivity.emplace_back(std::make_pair(i, j));
      }
    }
  }

  // Unitary gates on two qubits to be optimized.
  // We only expose decomposition into two-qubit unitaries,
  // so that these matrix can be decomposed into gates using XACC KAK.
  std::vector<std::pair<int, int>> layers;
  if (parameters.keyExists<std::vector<std::pair<int, int>>>("circuit-structure")) {
    layers = parameters.get<std::vector<std::pair<int, int>>>("circuit-structure");
  }

  // If the user has fixed the structure.
  const bool isFixedStructure = !layers.empty();
  
  // If not fixed, try an iterative approach:
  // starting with two layers.
  // Note: one layer is usually not enough.
  int n_layers = 2;
  if (!isFixedStructure) {
    for (int i = 0; i < n_layers; ++i) {
      for (const auto &connectedPair : connectivity) {
        layers.emplace_back(connectedPair);
      }
    }
  }
  assert(!layers.empty());
  locals["layers"] = layers;
  // Setup the python src.
  auto py_src =
      R"#(import numpy as np
from scipy.stats import unitary_group
from qfactor import Gate, optimize, get_distance
umat = locals()['unitary']
circuit = []
circuit_layers = locals()['layers']
for layer in circuit_layers:
  circuit.append(Gate(unitary_group.rvs(4), layer))
# Call the optimize function
ans = optimize(circuit, umat)
uMats = []
for uGate in ans:
  uMats.append(uGate.utry)
final_distance = get_distance(ans, umat)
  )#";

  try {
    py::exec(py_src, py::globals(), locals);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << "XACC Py-QFactor Exec Error:\n";
    ss << e.what();
    xacc::error(ss.str());
  }
  
  const auto resultMats = locals["uMats"];
  const auto finalTraceDistance = locals["final_distance"];
  py::print(resultMats);
  py::print(finalTraceDistance);
  // Next: construct the circuit from unitary gate matrices...
  return true;
}
const std::vector<std::string> PyQfactor::requiredKeys() { return {"unitary"}; }
} // namespace circuits
} // namespace xacc
REGISTER_PLUGIN(xacc::circuits::PyQfactor, xacc::Instruction)