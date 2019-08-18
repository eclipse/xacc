#include "RichExtrapDecorator.hpp"
#include "InstructionIterator.hpp"
#include "XACC.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {
void RichExtrapDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  if (!decoratedAccelerator) {
    xacc::error("Null Decorated Accelerator Error");
  }

  if (!xacc::optionExists("rich-extrap-r")) {
    xacc::error(
        "Cannot find rich-extrap-r. Skipping Richardson Extrapolation.");
    return;
  }

  // Get RO error probs
  auto r = std::stoi(xacc::getOption("rich-extrap-r"));
  buffer->addExtraInfo("rich-extrap-r", ExtraInfo(r));

  auto provider = xacc::getService<IRProvider>("quantum");

  auto f =
      provider->createComposite(function->name(), function->getVariables());

  InstructionIterator it(function);
  while (it.hasNext()) {
    auto nextInst = it.next();

    if (!nextInst->isComposite() && nextInst->isEnabled()) {

      if (nextInst->name() == "CNOT") {
        for (int i = 0; i < r; i++) {
          auto tmp = nextInst->clone();
          tmp->setBits(nextInst->bits());
          f->addInstruction(tmp);
        }
      } else {
        f->addInstruction(nextInst);
      }
    }
  }

  //   std::cout << "HELLO: \n" << f->toString("q");

  decoratedAccelerator->execute(buffer, f);

  return;
}

void RichExtrapDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;

  if (!decoratedAccelerator) {
    xacc::error("RichExtrap - Null Decorated Accelerator Error");
  }

  if (!xacc::optionExists("rich-extrap-r")) {
    xacc::error(
        "Cannot find rich-extrap-r. Skipping Richardson Extrapolation.");
  }

  // Get RO error probs
  auto r = std::stoi(xacc::getOption("rich-extrap-r"));
  buffer->addExtraInfo("rich-extrap-r", ExtraInfo(r));

  auto provider = xacc::getService<IRProvider>("quantum");

  std::vector<std::shared_ptr<CompositeInstruction>> newFuncs;

  for (auto &f : functions) {
    auto newF = provider->createComposite(f->name(), f->getVariables());

    InstructionIterator it(f);
    while (it.hasNext()) {
      auto nextInst = it.next();

      if (!nextInst->isComposite() && nextInst->isEnabled()) {

        if (nextInst->name() == "CNOT") {
          for (int i = 0; i < r; i++) {
            auto tmp = nextInst->clone();
            tmp->setBits(nextInst->bits());
            newF->addInstruction(tmp);
          }
        } else {
          newF->addInstruction(nextInst);
        }
      }
    }

    // xacc::info("HI: " + newF->toString("q"));

    newFuncs.push_back(newF);
  }

  decoratedAccelerator->execute(buffer, newFuncs);
}

} // namespace quantum
} // namespace xacc