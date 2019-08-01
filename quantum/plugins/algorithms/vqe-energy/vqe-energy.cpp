#include "vqe-energy.hpp"

#include "Observable.hpp"
#include "XACC.hpp"

#include "InstructionParameter.hpp"
#include <memory>
using namespace xacc;

namespace xacc {
namespace algorithm {
bool VQEEnergy::initialize(const AlgorithmParameters &parameters) {
  if (!parameters.count("observable")) {
    return false;
  } else if (!parameters.count("ansatz")) {
    return false;
  } else if (!parameters.count("accelerator")) {
    return false;
  }
  try {
    observable =
        parameters.at("observable").as_no_error<std::shared_ptr<Observable>>();
  } catch (std::exception &e) {
    observable = std::shared_ptr<Observable>(
        parameters.at("observable").as<Observable *>(), [](Observable *) {});
  }
  kernel = parameters.at("ansatz").as<std::shared_ptr<Function>>();
  accelerator = parameters.at("accelerator").as<std::shared_ptr<Accelerator>>();

  if (parameters.count("initial-parameters")) {
    initial_params =
        parameters.at("initial-parameters").as<std::vector<double>>();
  }
  return true;
}

const std::vector<std::string> VQEEnergy::requiredParameters() const {
  return {"observable", "accelerator", "ansatz"};
}

void VQEEnergy::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // void VQE::execute(xacc::Observable &observable, Optimizer &optimizer) {
  auto kernels = observable->observe(kernel);
  std::vector<double> coefficients;
  std::vector<std::string> kernelNames;
  std::vector<std::shared_ptr<Function>> fsToExec;

  double identityCoeff = 0.0;
  for (auto &f : kernels) {
    kernelNames.push_back(f->name());
    InstructionParameter p = f->getOption("coefficient");
    std::complex<double> coeff = p.as<std::complex<double>>();

    if (f->nInstructions() > kernel->nInstructions()) {
      fsToExec.push_back(f->operator()(initial_params));
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
    buffers[i]->addExtraInfo("parameters", initial_params);
    buffer->appendChild(fsToExec[i]->name(), buffers[i]);
  }
  std::stringstream ss;
  ss << "E(" << initial_params[0];
  for (int i = 1; i < initial_params.size(); i++)
    ss << "," << initial_params
    [i];
  ss << ") = " << energy;
  xacc::info(ss.str());

  buffer->addExtraInfo("vqe-energy", ExtraInfo(energy));
  buffer->addExtraInfo("vqe-params", ExtraInfo(initial_params));
  return;
}
} // namespace algorithm
} // namespace xacc