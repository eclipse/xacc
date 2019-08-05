#include "vqe.hpp"

#include "Observable.hpp"
#include "XACC.hpp"

#include "InstructionParameter.hpp"
#include <memory>
using namespace xacc;

namespace xacc {
namespace algorithm {
bool VQE::initialize(const AlgorithmParameters &parameters) {
  if (!parameters.count("observable")) {
    return false;
  } else if (!parameters.count("ansatz")) {
    return false;
  } else if (!parameters.count("optimizer")) {
    return false;
  } else if (!parameters.count("accelerator")) {
    return false;
  }

  try {
    observable = parameters.at("observable").as_no_error<std::shared_ptr<Observable>>();
  } catch (std::exception &e) {
    observable = std::shared_ptr<Observable>(parameters.at("observable").as<Observable*>(), [](Observable*){});
  }
  optimizer = parameters.at("optimizer").as<std::shared_ptr<Optimizer>>();
  kernel = parameters.at("ansatz").as<std::shared_ptr<Function>>();
  accelerator = parameters.at("accelerator").as<std::shared_ptr<Accelerator>>();

  return true;
}

const std::vector<std::string> VQE::requiredParameters() const {
  return {"observable", "optimizer", "accelerator", "ansatz"};
}

void VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // void VQE::execute(xacc::Observable &observable, Optimizer &optimizer) {
  auto kernels = observable->observe(kernel);

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.
  OptFunction f(
      [&, this](const std::vector<double> &x) {
        std::vector<double> coefficients;
        std::vector<std::string> kernelNames;
        std::vector<std::shared_ptr<Function>> fsToExec;

        double identityCoeff = 0.0;
        for (auto &f : kernels) {
          kernelNames.push_back(f->name());
          InstructionParameter p = f->getOption("coefficient");
          std::complex<double> coeff = p.as<std::complex<double>>();

          int nFunctionInstructions = 0;
          if (f->getInstruction(0)->isComposite()) {
              nFunctionInstructions = kernel->nInstructions() + f->nInstructions() - 1;
          } else {
              nFunctionInstructions = f->nInstructions();
          }

          if (nFunctionInstructions > kernel->nInstructions()) {
            fsToExec.push_back(f->operator()(x));
            coefficients.push_back(std::real(coeff));
          } else {
            identityCoeff += std::real(coeff);
          }
        }

        auto buffers = accelerator->execute(buffer, fsToExec);

        double energy = identityCoeff;
        for (int i = 0; i < buffers.size(); i++) {
          auto expval = buffers[i]->getExpectationValueZ();
          energy += expval * coefficients[i];
          buffers[i]->addExtraInfo("coefficient", coefficients[i]);
          buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
          buffers[i]->addExtraInfo("exp-val-z", expval);
          buffers[i]->addExtraInfo("parameters", x);
          buffer->appendChild(fsToExec[i]->name(), buffers[i]);
        }

        std::stringstream ss;
        ss << "E(" << x[0];
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << energy;
        xacc::info(ss.str());
        return energy;
      },
      kernel->nParameters());

  // std::cout << f({.55}) << "\n";
  auto result = optimizer->optimize(f);

  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
  return;
}
} // namespace algorithm
} // namespace xacc