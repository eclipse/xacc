#include "RichExtrapDecorator.hpp"
#include "InstructionIterator.hpp"
#include "XACC.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace quantum {
void RichExtrapDecorator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                                  const std::shared_ptr<Function> function) {

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

  auto provider = xacc::getService<IRProvider>("gate");

  auto f = provider->createFunction(function->name(), function->bits(),
                                    function->getParameters());

  InstructionIterator it(function);
  while (it.hasNext()) {
    auto nextInst = it.next();

    if (!nextInst->isComposite() && nextInst->isEnabled()) {

      if (nextInst->name() == "CNOT") {
        for (int i = 0; i < r; i++)
          f->addInstruction(nextInst);
      } else {
        f->addInstruction(nextInst);
      }
    }
  }

//   std::cout << "HELLO: \n" << f->toString("q");

  decoratedAccelerator->execute(buffer, f);

  return;
}

std::vector<std::shared_ptr<AcceleratorBuffer>> RichExtrapDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<Function>> functions) {

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

  auto provider = xacc::getService<IRProvider>("gate");

  std::vector<std::shared_ptr<Function>> newFuncs;

  for (auto &f : functions) {
    auto newF =
        provider->createFunction(f->name(), f->bits(), f->getParameters());

    InstructionIterator it(f);
    while (it.hasNext()) {
      auto nextInst = it.next();

      if (!nextInst->isComposite() && nextInst->isEnabled()) {

        if (nextInst->name() == "CNOT") {
          for (int i = 0; i < r; i++)
            newF->addInstruction(nextInst);
        } else {
          newF->addInstruction(nextInst);
        }
      }
    }

    // std::cout << "HI: " << newF->toString("q") << "\n";
    
    newFuncs.push_back(newF);
  }


  return decoratedAccelerator->execute(buffer, newFuncs);
}

} // namespace quantum
} // namespace xacc