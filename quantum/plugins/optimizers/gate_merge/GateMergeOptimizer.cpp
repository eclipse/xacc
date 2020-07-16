#include <cassert>
#include "GateMergeOptimizer.hpp"
#include "GateFusion.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {
void MergeSingleQubitGatesOptimizer::apply(std::shared_ptr<CompositeInstruction> program, const std::shared_ptr<Accelerator> accelerator, const HeterogeneousMap &options)
{
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    for (size_t instIdx = 0; instIdx < program->nInstructions(); ++instIdx)
    {
        const auto sequence = findSingleQubitGateSequence(program, instIdx, 2);
        if (!sequence.empty()) 
        {
            auto tmpKernel = gateRegistry->createComposite("__TMP__");
            for (const auto& instIdx: sequence)
            {
                auto instrPtr = program->getInstruction(instIdx)->clone();
                assert(instrPtr->bits().size() == 1);
                // Remap to bit 0 for fusing
                instrPtr->setBits({ 0 });
                tmpKernel->addInstruction(instrPtr);
            }

            auto fuser = xacc::getService<xacc::quantum::GateFuser>("default");
            fuser->initialize(tmpKernel);
            const Eigen::Matrix2cd uMat = fuser->calcFusedGate(1);
            auto zyz = std::dynamic_pointer_cast<quantum::Circuit>(xacc::getService<Instruction>("z-y-z"));
            const bool expandOk = zyz->expand({ 
                std::make_pair("unitary", uMat)
            });

            // Optimized decomposed sequence:
            const auto nbInstructionsAfter = zyz->nInstructions();
            // A simplified sequence was found.
            if (nbInstructionsAfter < sequence.size())
            {
                // Rewrite:
                const size_t bitIdx = program->getInstruction(sequence[0])->bits()[0];
                // Disable to remove:
                const auto programLengthBefore = program->nInstructions();
                for (const auto& instIdx: sequence)
                {
                    auto instrPtr = program->getInstruction(instIdx);
                    instrPtr->disable();
                }
                program->removeDisabled();
                const auto locationToInsert = sequence[0];
                for (auto& newInst: zyz->getInstructions())
                {
                    newInst->setBits({bitIdx});
                    program->insertInstruction(locationToInsert, newInst->clone());
                }
                const auto programLengthAfter = program->nInstructions();
                assert(programLengthAfter < programLengthBefore);
            }
        }
    }
}

std::vector<size_t> MergeSingleQubitGatesOptimizer::findSingleQubitGateSequence(const std::shared_ptr<CompositeInstruction> in_program, size_t in_startIdx, size_t in_lengthLimit) const
{
    const auto nbInstructions = in_program->nInstructions();
    std::unordered_map<size_t, std::vector<size_t>> qubitToSequence;
    assert(in_startIdx < nbInstructions);
    auto firstInst = in_program->getInstruction(in_startIdx);
    // Not a single-qubit gate.
    if (firstInst->bits().size() > 1 || firstInst->name() == "Measure")
    {
        return {};
    }

    const size_t bitIdx = firstInst->bits()[0];
    std::vector<size_t> gateSequence;
    gateSequence.emplace_back(in_startIdx);

    const auto returnSeq = [&](const std::vector<size_t>& in_seq) -> std::vector<size_t> {
        return (in_seq.size() >= in_lengthLimit) ? in_seq : std::vector<size_t>{};
    };

    for (size_t instIdx = in_startIdx + 1; instIdx < nbInstructions; ++instIdx)
    {
        auto instPtr = in_program->getInstruction(instIdx);
        // Matching bitIdx
        if (instPtr->bits().size() == 1 && instPtr->bits()[0] == bitIdx)
        {
            if (instPtr->name() != "Measure")
            {
                gateSequence.emplace_back(instIdx);
            }
            else 
            {
                return returnSeq(gateSequence);
            }
        }
        // If this is a two-qubit gate that involves this qubit wire,
        // terminate the scan and return.
        else if (instPtr->bits().size() == 2 && xacc::container::contains(instPtr->bits(), bitIdx))
        {
            return returnSeq(gateSequence);
        }
    }  
    // Reach the end of the circuit:  
    return returnSeq(gateSequence);    
}
}
}