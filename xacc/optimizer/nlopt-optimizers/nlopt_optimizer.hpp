#ifndef XACC_NLOPT_OPTIMIZER_HPP_
#define XACC_NLOPT_OPTIMIZER_HPP_

#include <type_traits>
#include <utility>

#include "Optimizer.hpp"

namespace xacc {

struct LambdaToVFunc {
    template<typename Tret, typename T>
    static Tret lambda_ptr_exec(const std::vector<double>& x, std::vector<double>& grad, void* fd) {
        return (Tret) (*(T*)fn<T>())(x,grad,fd);
    }

    template<typename Tret = double, typename Tfp = Tret(*)(const std::vector<double>&,std::vector<double>&,void*), typename T>
    static Tfp ptr(T& t) {
        fn<T>(&t);
        return (Tfp) lambda_ptr_exec<Tret, T>;
    }

    template<typename T>
    static void* fn(void* new_fn = nullptr) {
        static void* fn;
        if (new_fn != nullptr)
            fn = new_fn;
        return fn;
    }
};

class NLOptimizer : public Optimizer {
public:
  OptResult optimize(OptFunction &function) override;

  const std::string name() const override {
      return "nlopt";
  }

  const std::string description() const override {
      return "";
  }
};
} // namespace xacc
#endif
