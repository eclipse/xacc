#include "gsl_optimizer.hpp"
#include "xacc_plugin.hpp"
#include <gsl/gsl_multimin.h>

struct GSLFunctionWrapper {
  xacc::OptFunction &function;
  std::vector<double> grad;

  double operator()(const gsl_vector *v) {
    std::vector<double> x(v->size);
    for (size_t i = 0; i < v->size; ++i) {
      x[i] = gsl_vector_get(v, i);
    }
    if (grad.empty()) {
      return function(std::move(x));
    } else {
      return function(x, grad);
    }
  }

  void getGradient(gsl_vector *df) {
    for (size_t i = 0; i < grad.size(); ++i) {
      gsl_vector_set(df, i, grad[i]);
    }
  }
};

namespace xacc {

const std::string GSLOptimizer::get_algorithm() const {
  std::string optimizerAlgo = "COBYLA";
  if (options.stringExists("algorithm")) {
    optimizerAlgo = options.getString("algorithm");
  }
  if (options.stringExists("scipy-optimizer")) {
    optimizerAlgo = options.getString("scipy-optimizer");
  }
  return optimizerAlgo;
}

const bool GSLOptimizer::isGradientBased() const {

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

OptResult GSLOptimizer::optimize(OptFunction &function) {

  bool maximize = false;
  if (options.keyExists<bool>("maximize")) {
    xacc::info("Turning on maximize!");
    maximize = options.get<bool>("maximize");
  }

  std::string algo = "COBYLA";
  if (options.stringExists("algorithm")) {
    algo = options.getString("algorithm");
  }
  if (options.stringExists("gsl-optimizer")) {
    algo = options.getString("gsl-optimizer");
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
    xacc::info("[GSL] function tolerance set to " + std::to_string(tol));
  }
  if (options.keyExists<double>("gsl-ftol")) {
    tol = options.get<double>("ftol");
    xacc::info("[GSL] function tolerance set to " + std::to_string(tol));
  }

  int maxeval = 1000;
  if (options.keyExists<int>("maxeval")) {
    maxeval = options.get<int>("maxeval");
    xacc::info("[GSL] max function evaluations set to " +
               std::to_string(maxeval));
  }
  if (options.keyExists<int>("gsl-maxeval")) {
    maxeval = options.get<int>("maxeval");
    xacc::info("[GSL] max function evaluations set to " +
               std::to_string(maxeval));
  }

  double step = 1.0e-4;
  if (options.keyExists<double>("step-size")) {
    step = options.get<double>("step-size");
    xacc::info("[GSL] step size set to " + std::to_string(step));
  }
  if (options.keyExists<double>("gsl-step-size")) {
    step = options.get<double>("gsl-step-size");
    xacc::info("[GSL] step size set to " + std::to_string(step));
  }

  std::vector<double> x(function.dimensions());
  if (options.keyExists<std::vector<double>>("initial-parameters")) {
    x = options.get_with_throw<std::vector<double>>("initial-parameters");
  } else if (options.keyExists<std::vector<int>>("initial-parameters")) {
    auto tmpx = options.get<std::vector<int>>("initial-parameters");
    x = std::vector<double>(tmpx.begin(), tmpx.end());
  }

  auto dim = function.dimensions();
  gsl_vector *theta = gsl_vector_alloc(dim);
  for (size_t i = 0; i < dim; ++i) {
    gsl_vector_set(theta, i, x[i]);
  }

  GSLFunctionWrapper wrapper{function};
  double minVal;
  std::vector<double> result(dim);
  if (isGradientBased()) {

    wrapper.grad.resize(dim);
    gsl_multimin_function_fdf gslFunction;
    gslFunction.n = dim;

    gslFunction.f = [](const gsl_vector *v, void *params) -> double {
      GSLFunctionWrapper *w = static_cast<GSLFunctionWrapper *>(params);
      return (*w)(v);
    };

    gslFunction.df = [](const gsl_vector *v, void *params, gsl_vector *df) {
      GSLFunctionWrapper *w = static_cast<GSLFunctionWrapper *>(params);
      w->getGradient(df);
    };

    // I don't understand why, but this seems kinda redundant
    gslFunction.fdf = [](const gsl_vector *v, void *params, double *f,
                         gsl_vector *df) {
      GSLFunctionWrapper *w = static_cast<GSLFunctionWrapper *>(params);
      (*f) = (*w)(v);
      w->getGradient(df);
    };

    gslFunction.params = &wrapper;

    const gsl_multimin_fdfminimizer_type *T = gsl_multimin_fdfminimizer_conjugate_fr;
    gsl_multimin_fdfminimizer *s = gsl_multimin_fdfminimizer_alloc(T, dim);

    int set_status = gsl_multimin_fdfminimizer_set(s, &gslFunction, theta, 0.01, 1e-4);
    if (set_status) {
      gsl_multimin_fdfminimizer_free(s);
      gsl_vector_free(theta);
      throw std::runtime_error("Failed to set the minimizer.");
    }

    int status, iter = 0;
    do {
      iter++;
      status = gsl_multimin_fdfminimizer_iterate(s);

      if (status)
        break;

      // Check for convergence using gradient norm
      status = gsl_multimin_test_gradient(s->gradient, 1e-4);

    } while (status == GSL_CONTINUE && iter < 100);

    // std::vector<double> result(dim);
    for (size_t i = 0; i < dim; ++i) {
      result[i] = gsl_vector_get(s->x, i);
    }

    minVal = s->f;
    gsl_vector_free(theta);
    gsl_multimin_fdfminimizer_free(s);

  } else {

    gsl_multimin_function gslFunction;
    gslFunction.n = dim;
    gslFunction.f = [](const gsl_vector *v, void *params) -> double {
      GSLFunctionWrapper *w = static_cast<GSLFunctionWrapper *>(params);
      return (*w)(v);
    };

    gslFunction.params = &wrapper;

    gsl_vector *steps = gsl_vector_alloc(gslFunction.n);
    gsl_vector_set_all(steps, step);

    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2;
    gsl_multimin_fminimizer *s =
        gsl_multimin_fminimizer_alloc(T, gslFunction.n);
    gsl_multimin_fminimizer_set(s, &gslFunction, theta, steps);

    int status, iter;
    double size;
    do {
      iter++;
      status = gsl_multimin_fminimizer_iterate(s);

      if (status)
        break;

      size = gsl_multimin_fminimizer_size(s);
      status = gsl_multimin_test_size(size, 1e-4);
    } while (status == GSL_CONTINUE && iter < 100);

    std::vector<double> result(gslFunction.n);
    for (size_t i = 0; i < gslFunction.n; ++i) {
      result[i] = gsl_vector_get(s->x, i);
    }

    minVal = s->fval;

    gsl_multimin_fminimizer_free(s);
    gsl_vector_free(theta);
  }

  return {minVal, result};
}
} // namespace xacc
REGISTER_OPTIMIZER(xacc::GSLOptimizer)