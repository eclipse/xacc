#include "NearestNeighborTransform.hpp"
#include "InstructionIterator.hpp"

namespace xacc {
namespace quantum {

void NearestNeighborTransform::apply(
    std::shared_ptr<CompositeInstruction> in_program,
    const std::shared_ptr<Accelerator> in_accelerator,
    const HeterogeneousMap &in_options) {
  // The option is qubit-distance (default is 1, adjacent qubits)
  // any 2-qubit gates that are further than this distance are converted into
  // Swap + Original Gate (at qubit distance) + Swap
  // Strategy: meet in the middle:
  // e.g. CNOT q0, q9;  qubit-distance = 2 (max distance is next neighbor,
  // e.g. CNOT q0, q2; CNOT q7, q9 are okay) then we will Swap q0->q4; q9->q6;
  // then CNOT q4, q6; then Swap q4->q0 and Swap q6->q9

  int maxDistance = 1;
  if (in_options.keyExists<int>("max-distance")) {
    maxDistance = in_options.get<int>("max-distance");
  }

  auto provider = xacc::getIRProvider("quantum");
  auto flattenedProgram =
      provider->createComposite(in_program->name() + "_Flattened");
  InstructionIterator it(in_program);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled() && !nextInst->isComposite()) {
      flattenedProgram->addInstruction(nextInst->clone());
    }
  }

  auto transformedProgram =
      provider->createComposite(in_program->name() + "_Transformed");
  for (int i = 0; i < flattenedProgram->nInstructions(); ++i) {
    auto inst = flattenedProgram->getInstruction(i);

    const auto exceedMaxDistance = [&maxDistance](int q1, int q2) -> bool {
      return std::abs(q1 - q2) > maxDistance;
    };

    if (inst->bits().size() == 2 &&
        exceedMaxDistance(inst->bits()[0], inst->bits()[1])) {
      const int origLowerIdx = std::min({inst->bits()[0], inst->bits()[1]});
      const int origUpperIdx = std::max({inst->bits()[0], inst->bits()[1]});
      size_t lowerIdx = origLowerIdx;
      size_t upperIdx = origUpperIdx;
      // Insert swaps
      for (;; /*Break inside*/) {
        transformedProgram->addInstruction(
            provider->createInstruction("Swap", {lowerIdx, lowerIdx + 1}));
        lowerIdx++;

        if (!exceedMaxDistance(lowerIdx, upperIdx)) {
          break;
        }

        transformedProgram->addInstruction(
            provider->createInstruction("Swap", {upperIdx, upperIdx - 1}));
        upperIdx--;

        if (!exceedMaxDistance(lowerIdx, upperIdx)) {
          break;
        }
      }

      // Run new gate
      const bool bitCompare = inst->bits()[0] < inst->bits()[1];
      inst->setBits(bitCompare ? std::vector<size_t>{lowerIdx, upperIdx}
                               : std::vector<size_t>{upperIdx, lowerIdx});
      transformedProgram->addInstruction(inst);

      // Insert swaps
      for (size_t i = lowerIdx; i > origLowerIdx; --i) {
        transformedProgram->addInstruction(
            provider->createInstruction("Swap", {i, i - 1}));
      }

      for (size_t i = upperIdx; i < origUpperIdx; ++i) {
        transformedProgram->addInstruction(
            provider->createInstruction("Swap", {i, i + 1}));
      }
    } else {
      transformedProgram->addInstruction(inst);
    }
  }
  // DEBUG:
  // std::cout << "After transform: \n" <<  transformedProgram->toString() <<
  // "\n";
  in_program->clear();
  in_program->addInstructions(transformedProgram->getInstructions());

  return;
}
} // namespace quantum
} // namespace xacc
