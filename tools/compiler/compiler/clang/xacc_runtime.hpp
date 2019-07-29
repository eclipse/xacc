#include "XACC.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"
#include "Observable.hpp"

#define __qpu__ __attribute__((annotate("__qpu__")))
#define __observe__(OBS) __attribute__((annotate("observe_" #OBS)))

std::shared_ptr<Function> loadFromIR(const std::string &ir) {
  auto function =
      xacc::getService<xacc::IRProvider>("gate")->createFunction("f", {}, {});
  std::istringstream iss(ir);
  function->load(iss);
  return function;
}

// std::shared_ptr<Observable> getObservable(const std::string type,
//                                           const std::string fromStr) {
//   auto obs = xacc::getService<Observable>(type);
//   obs->fromString(fromStr);
//   return obs;
// }

template <typename T> std::vector<T> linspace(T a, T b, size_t N) {
  T h = (b - a) / static_cast<T>(N - 1);
  std::vector<T> xs(N);
  typename std::vector<T>::iterator x;
  T val;
  for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
    *x = val;
  return xs;
}

void observe(qbit qbits, std::vector<double> x, std::shared_ptr<Accelerator> qpu,
             std::shared_ptr<Function> function,
             std::shared_ptr<Observable> obs) {

  auto functions = obs->observe(function);

  std::vector<double> coefficients;
  std::vector<std::string> kernelNames;
  std::vector<std::shared_ptr<Function>> fsToExec;

  double identityCoeff = 0.0;
  for (auto &f : functions) {
    kernelNames.push_back(f->name());
    InstructionParameter p = f->getOption("coefficient");
    std::complex<double> coeff = p.as<std::complex<double>>();

    if (f->nInstructions() > function->nInstructions()) {
      fsToExec.push_back(f->operator()(x));
      coefficients.push_back(std::real(coeff));
    } else {
      identityCoeff += std::real(coeff);
    }
  }

  auto buffers = qpu->execute(qbits, fsToExec);

  double observation = identityCoeff;
  for (int i = 0; i < buffers.size(); i++) {
    auto expval = buffers[i]->getExpectationValueZ();
    observation += expval * coefficients[i];
    buffers[i]->addExtraInfo("coefficient", coefficients[i]);
    buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
    buffers[i]->addExtraInfo("exp-val-z", expval);
    buffers[i]->addExtraInfo("parameters", x);
    qbits->appendChild(fsToExec[i]->name(), buffers[i]);
  }

  qbits->addExtraInfo("observation", observation);
}
void observe(qbit qbits, std::vector<double> x, std::shared_ptr<Accelerator> qpu,
             std::shared_ptr<Function> function,
             Observable& obs) {

  auto functions = obs.observe(function);

  std::vector<double> coefficients;
  std::vector<std::string> kernelNames;
  std::vector<std::shared_ptr<Function>> fsToExec;

  double identityCoeff = 0.0;
  for (auto &f : functions) {
    kernelNames.push_back(f->name());
    InstructionParameter p = f->getOption("coefficient");
    std::complex<double> coeff = p.as<std::complex<double>>();

    if (f->nInstructions() > function->nInstructions()) {
      fsToExec.push_back(f->operator()(x));
      coefficients.push_back(std::real(coeff));
    } else {
      identityCoeff += std::real(coeff);
    }
  }

  auto buffers = qpu->execute(qbits, fsToExec);

  double observation = identityCoeff;
  for (int i = 0; i < buffers.size(); i++) {
    auto expval = buffers[i]->getExpectationValueZ();
    observation += expval * coefficients[i];
    buffers[i]->addExtraInfo("coefficient", coefficients[i]);
    buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
    buffers[i]->addExtraInfo("exp-val-z", expval);
    buffers[i]->addExtraInfo("parameters", x);
    qbits->appendChild(fsToExec[i]->name(), buffers[i]);
  }

  qbits->addExtraInfo("observation", observation);
}
} // namespace xacc