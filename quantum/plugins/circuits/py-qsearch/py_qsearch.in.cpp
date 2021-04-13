#include "py_qsearch.hpp"
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

PyQsearch::~PyQsearch() {
  if (libpython_handle) {
    int i = dlclose(libpython_handle);
    if (i != 0) {
      std::cout << "error closing python lib in qsearch: " << i << "\n";
      std::cout << dlerror() << "\n";
    }
  }
}
void PyQsearch::initialize() {
  if (!guard && !Py_IsInitialized()) {
    guard = std::make_shared<py::scoped_interpreter>();
    libpython_handle = dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
    initialized = true;
  }
}

bool PyQsearch::expand(const xacc::HeterogeneousMap &parameters) {

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

  locals["verbosity"] = parameters.get_or_default<int>("verbosity", 0);
  locals["write_to_stdout"] = parameters.get_or_default<bool>("write_to_stdout", false);
  locals["reoptimize_size"] = parameters.get_or_default<int>("reoptimize_size", 7);
  locals["has_compiler"] = false;
  if (parameters.stringExists("compiler_class")) {
    locals["compiler_class"] = parameters.getString("compiler_class");
    locals["has_compiler"] = true;
  }
  if (parameters.keyExists<int>("min_depth")) {
    locals["min_depth"] = parameters.get<int>("min_depth");
  }
  // Lets get a unique hash for the unitary
  // This will create a unique instance in
  // the project compilation database
  std::stringstream ss;
  ss << unitary;
  std::hash<std::string> hasher;
  auto hash = hasher(ss.str());

  // Create a compile name unique to this matrix
  std::string compile_name = "__internal_umat";
  locals["compile-name"] = compile_name + "_" + std::to_string(hash);

  // we'll store this to the $HOME/.xacc_qsearch_db directory
  std::string internal_project_path =
      std::string(std::getenv("HOME")) + std::string("/.xacc_qsearch_db");
  locals["internal_project_path"] = internal_project_path;


// def qsearch_synthesize(utry):
// """
// Synthesis function with this tool.

// Args:
//     utry (np.ndarray): The unitary to synthesize.

// Returns
//     qasm (str): The synthesized QASM output.

// Raises:
//     TypeError: If utry is not a valid unitary.

//     ValueError: If the utry has invalid dimensions.
// """
// # Pass options into qsearch, being maximally quiet,
// # and set the target to utry
// opts = options.Options()
// opts.target = utry
// opts.verbosity = 0
// opts.write_to_stdout = False
// opts.reoptimize_size = 7
// #opts.solver = multistart_solvers.MultiStart_Solver(24)
// #opts.gateset = qsearch.gatesets.QubitCNOTAdjacencyList(topology)

// # use the LEAP compiler, which scales better than normal qsearch
// compiler = leap_compiler.LeapCompiler()
// output = compiler.compile(opts)

// # LEAP requires some post-processing
// pp = post_processing.LEAPReoptimizing_PostProcessor()
// output = pp.post_process_circuit(output, opts)
// output = assemblers.ASSEMBLER_IBMOPENQASM.assemble(output)
// return output

// # create the project
// with qsearch.Project("leapex") as project:
//     # Add 4 qubit qft
//     project.add_compilation("qft4", unitaries.qft(16))
//     # set a miniumum search depth (to reduce frequent chopping that gets nowhere)
//     project["min_depth"] = 4
//     # configure qsearch to use the leap compiler
    // project["compiler_class"] = leap_compiler.LeapCompiler
//     project["verbosity"] = 2
//     project.run()
//     project.post_process(post_processing.LEAPReoptimizing_PostProcessor(), solver=multistart_solvers.MultiStart_Solver(8), parallelizer=parallelizers.ProcessPoolParallelizer, reoptimize_size=7)

  // Setup the python src.
  auto py_src =
      R"#(import qsearch, numpy as np
from qsearch.assemblers import ASSEMBLER_IBMOPENQASM
from qsearch import leap_compiler, parallelizers, post_processing
umat = locals()['unitary']
oqasm_src = ''
compile_name = locals()['compile-name']
with qsearch.Project(locals()['internal_project_path']) as project:
    project.add_compilation(compile_name, umat)
    project["verbosity"] = locals()["verbosity"]
    project["write_to_stdout"] = locals()["write_to_stdout"]
    if locals()['has_compiler']:
        if locals()['compiler_class'] == 'LeapCompiler':
            project["compiler_class"] = leap_compiler.LeapCompiler
    project.run()
    if locals()['has_compiler']:
        if locals()['compiler_class'] == 'LeapCompiler':
            project.post_process(post_processing.LEAPReoptimizing_PostProcessor(), reoptimize_size=locals()['reoptimize_size'])
    oqasm_src = ASSEMBLER_IBMOPENQASM.assemble(project._compilations[compile_name])
  )#";

  try {
    py::exec(py_src, py::globals(), locals);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << "XACC Py-QSearch Exec Error:\n";
    ss << e.what();
    xacc::error(ss.str());
  }

  auto oqasm_src = locals["oqasm_src"].cast<std::string>();

  auto staq = xacc::getCompiler("staq");
  auto program = staq->compile(oqasm_src)->getComposites()[0];

  for (int i = 0; i < program->nInstructions(); i++) {
    addInstruction(program->getInstruction(i));
  }

  return true;
}
const std::vector<std::string> PyQsearch::requiredKeys() { return {"unitary"}; }
} // namespace circuits
} // namespace xacc
REGISTER_PLUGIN(xacc::circuits::PyQsearch, xacc::Instruction)