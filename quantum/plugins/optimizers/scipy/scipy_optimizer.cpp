#include "scipy_optimizer.hpp"
#include "Optimizer.hpp"
#include "pybind11/stl.h"
#include "pybind11/numpy.h"
#include "xacc.hpp"
#include "xacc_plugin.hpp"

namespace py = pybind11;

namespace xacc {

const std::string ScipyOptimizer::get_algorithm() const {
  std::string optimizerAlgo = "COBYLA";
  if (options.stringExists("algorithm")) {
    optimizerAlgo = options.getString("algorithm");
  }
  if (options.stringExists("scipy-optimizer")) {
    optimizerAlgo = options.getString("scipy-optimizer");
  }
  return optimizerAlgo;
}

const bool ScipyOptimizer::isGradientBased() const {

  std::string optimizerAlgo = "cobyla";
  if (options.stringExists("algorithm")) {
    optimizerAlgo = options.getString("algorithm");
  }
  if (options.stringExists("scipy-optimizer")) {
    optimizerAlgo = options.getString("scipy-optimizer");
  }

  if (options.stringExists("optimizer")) {
    optimizerAlgo = options.getString("optimizer");
  }

  if (optimizerAlgo == "bfgs") {
    return true;
  } else {
    return false;
  }
}

OptResult ScipyOptimizer::optimize(OptFunction &function) {

  bool maximize = false;
  if (options.keyExists<bool>("maximize")) {
    xacc::info("Turning on maximize!");
    maximize = options.get<bool>("maximize");
  }

  std::string algo = "COBYLA";
  if (options.stringExists("algorithm")) {
    algo = options.getString("algorithm");
  }
  if (options.stringExists("scipy-optimizer")) {
    algo = options.getString("scipy-optimizer");
  }
  if (options.stringExists("optimizer")) {
    algo = options.getString("optimizer");
  }

  if (algo == "cobyla" || algo == "COBYLA") {
    algo = "COBYLA";
  } else if (algo == "nelder-mead" || algo == "Nelder-Mead") {
    algo = "Nelder-Mead";
  } else if (algo == "bfgs" || algo == "BFGS" || algo == "l-bfgs") {
    algo = "BFGS";
  } else {
    xacc::XACCLogger::instance()->error("Invalid optimizer at this time: " +
                                        algo);
  }

  double tol = 1e-6;
  if (options.keyExists<double>("ftol")) {
    tol = options.get<double>("ftol");
    xacc::info("[Scipy] function tolerance set to " + std::to_string(tol));
  }
  if (options.keyExists<double>("scipy-ftol")) {
    tol = options.get<double>("ftol");
    xacc::info("[Scipy] function tolerance set to " + std::to_string(tol));
  }

  int maxeval = 1000;
  if (options.keyExists<int>("maxeval")) {
    maxeval = options.get<int>("maxeval");
    xacc::info("[Scipy] max function evaluations set to " +
               std::to_string(maxeval));
  }
  if (options.keyExists<int>("scipy-maxeval")) {
    maxeval = options.get<int>("maxeval");
    xacc::info("[Scipy] max function evaluations set to " +
               std::to_string(maxeval));
  }

  std::vector<double> x(function.dimensions());
  if (options.keyExists<std::vector<double>>("initial-parameters")) {
    x = options.get_with_throw<std::vector<double>>("initial-parameters");
  } else if (options.keyExists<std::vector<int>>("initial-parameters")) {
    auto tmpx = options.get<std::vector<int>>("initial-parameters");
    x = std::vector<double>(tmpx.begin(), tmpx.end());
  }

  // if fails to find mininum should not throw error
  bool throwError = true;
  if (options.keyExists<bool>("throw-error")) {
    throwError = options.get<bool>("throw-error");
  }

  // here the python stuff starts
  py::list pyInitialParams;
  for (const auto &param : x) {
    pyInitialParams.append(param);
  }

  // wrap the objective function in this lambda
  // scipy passes a numpy array to this function, hence the py::array_t type
  py::object pyObjFunction =
      py::cpp_function([&function](const py::array_t<double> &pyParams) {
        std::vector<double> params(pyParams.size());
        std::memcpy(params.data(), pyParams.data(),
                    pyParams.size() * sizeof(double));
        return function(std::move(params));
      });

  // call this for gradient-based optimization
  py::object pyObjFunctionWithGrad =
      py::cpp_function([&function](const py::array_t<double> &pyParams) {
        std::vector<double> params(pyParams.size());
        std::memcpy(params.data(), pyParams.data(),
                    pyParams.size() * sizeof(double));

        std::vector<double> grad(params.size());
        double result = function(params, grad);
        py::array_t<double> pyGrad(grad.size());
        std::memcpy(pyGrad.mutable_data(), grad.data(),
                    grad.size() * sizeof(double));

        return py::make_tuple(result, pyGrad);
      });

  py::module scipy_optimize = py::module::import("scipy.optimize");

  // error handling helps here to see if it's coming from C++ or python
  try {

    py::object result = scipy_optimize.attr("minimize")(
        isGradientBased() ? pyObjFunctionWithGrad : pyObjFunction,
        pyInitialParams,
        py::arg("args") = py::tuple(),
        py::arg("method") = algo,
        py::arg("tol") = tol,
        py::arg("jac") = (isGradientBased() ? true : false));

    std::vector<double> optimizedParams =
        result.attr("x").cast<std::vector<double>>();
    double optimalValue = result.attr("fun").cast<double>();

    return {optimalValue, optimizedParams};
  } catch (const py::error_already_set &e) {

    if (throwError) {
      xacc::error("Python error: " + std::string(e.what()));
      throw;
    }
    return {};

  } catch (const std::exception &e) {
  
    if (throwError) {
      xacc::error("Error: " + std::string(e.what()));
      throw;
    }
    return {};
  }
}
} // namespace xacc

// Register the plugin with XACC
REGISTER_OPTIMIZER(xacc::ScipyOptimizer)