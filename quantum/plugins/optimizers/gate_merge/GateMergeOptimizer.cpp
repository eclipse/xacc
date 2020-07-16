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
            assert(expandOk);
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

void MergeTwoQubitBlockOptimizer::apply(std::shared_ptr<CompositeInstruction> program, const std::shared_ptr<Accelerator> accelerator, const HeterogeneousMap& options)
{
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");

    // No need to optimize block with less than 6 gates
    // since the KAK decomposition will result in at least 5 gate.
    const size_t MIN_SIZE = 6;
    if (program->nInstructions() < MIN_SIZE)
    {
        return;
    }
    for (size_t instIdx = 0; instIdx < program->nInstructions(); ++instIdx)
    {
        std::pair<size_t, size_t> qubitPair = std::make_pair(0, 0);
        const auto sequence = findGateSequence(program, instIdx, MIN_SIZE, qubitPair);
        if (!sequence.empty()) 
        {
            auto tmpKernel = gateRegistry->createComposite("__TMP__");
            
            const auto mapBits = [&qubitPair](const std::vector<size_t>& in_bits){
                const auto translate = [&qubitPair](size_t bit) {
                    assert(bit == qubitPair.first || bit == qubitPair.second);
                    assert(qubitPair.first  != qubitPair.second);
                    if (qubitPair.first < qubitPair.second)
                    {
                        return (bit == qubitPair.first) ? 0 : 1;
                    }
                    else
                    {
                        return (bit == qubitPair.first) ? 1 : 0;
                    }
                }; 

                std::vector<size_t> newBits;
                for (const auto& bit: in_bits)         
                {
                    newBits.emplace_back(translate(bit));
                }   
                return newBits;
            };

            // Map { 0, 1 } bits back to original bits
            const auto remapBits = [&qubitPair](const std::vector<size_t>& in_bits){
                const auto translate = [&qubitPair](size_t bit) {
                    assert(bit == 0 || bit == 1);
                    assert(qubitPair.first  != qubitPair.second);
                    if (qubitPair.first < qubitPair.second)
                    {
                        return (bit == 0) ? qubitPair.first : qubitPair.second;
                    }
                    else
                    {
                        return (bit == 0) ? qubitPair.second : qubitPair.first;
                    }
                }; 

                std::vector<size_t> newBits;
                for (const auto& bit: in_bits)         
                {
                    newBits.emplace_back(translate(bit));
                }   
                return newBits;
            };

            for (const auto& instIdx: sequence)
            {
                auto instrPtr = program->getInstruction(instIdx)->clone();
                instrPtr->setBits(mapBits(instrPtr->bits()));
                tmpKernel->addInstruction(instrPtr);
            }
            auto fuser = xacc::getService<xacc::quantum::GateFuser>("default");
            fuser->initialize(tmpKernel);
            const Eigen::Matrix4cd uMat = fuser->calcFusedGate(2);
            auto kak = std::dynamic_pointer_cast<quantum::Circuit>(xacc::getService<Instruction>("kak"));
            const bool expandOk = kak->expand({ 
                std::make_pair("unitary", uMat)
            });
            assert(expandOk);

            // Optimized decomposed sequence:
            const auto nbInstructionsAfter = kak->nInstructions();
            // A simplified sequence was found.
            if (nbInstructionsAfter < sequence.size())
            {
                // Disable to remove:
                const auto programLengthBefore = program->nInstructions();
                for (const auto& instIdx: sequence)
                {
                    auto instrPtr = program->getInstruction(instIdx);
                    instrPtr->disable();
                }
                program->removeDisabled();

                const auto locationToInsert = sequence[0];
                for (auto& newInst: kak->getInstructions())
                {
                    newInst->setBits(remapBits(newInst->bits()));
                    program->insertInstruction(locationToInsert, newInst->clone());
                }
                const auto programLengthAfter = program->nInstructions();
                assert(programLengthAfter < programLengthBefore);
            }
        }
    }
}

std::vector<size_t> MergeTwoQubitBlockOptimizer::findGateSequence(const std::shared_ptr<CompositeInstruction> in_program, size_t in_startIdx, size_t in_lengthLimit, std::pair<size_t, size_t>& out_qubitPair) const
{
    const auto nbInstructions = in_program->nInstructions();
    assert(in_startIdx < nbInstructions);
    auto firstInst = in_program->getInstruction(in_startIdx);
    if (firstInst->name() == "Measure")
    {
        return {};
    }

    const auto qubitPairIfAny = [&]() -> std::optional<std::pair<size_t, size_t>> {
        if (firstInst->bits().size() == 2)
        {
            return std::make_pair(firstInst->bits()[0], firstInst->bits()[1]);
        }

        // Single qubit gate:
        // Scan forward to find the *first* two-qubit gate which involves this qubit.
        assert(firstInst->bits().size() == 1);
        const auto firstBitIdx =  firstInst->bits()[0];
        for (size_t instIdx = in_startIdx + 1; instIdx < nbInstructions; ++instIdx)
        {
            auto instPtr = in_program->getInstruction(instIdx);
            if (instPtr->bits().size() == 2 && xacc::container::contains(instPtr->bits(), firstBitIdx))
            {
                return std::make_pair(instPtr->bits()[0], instPtr->bits()[1]);
            }
        }
        // Cannot find the boundary: i.e. no two-qubit gates
        return std::optional<std::pair<size_t, size_t>>();
    }();

    if (!qubitPairIfAny.has_value())
    {
        return {};
    }
    const auto qubitPair = qubitPairIfAny.value();
    std::vector<size_t> gateSequence;
    gateSequence.emplace_back(in_startIdx);
    out_qubitPair = qubitPair;
    const auto returnSeq = [&](const std::vector<size_t>& in_seq) -> std::vector<size_t> {
        return (in_seq.size() >= in_lengthLimit) ? in_seq : std::vector<size_t>{};
    };

    for (size_t instIdx = in_startIdx + 1; instIdx < nbInstructions; ++instIdx)
    {
        auto instPtr = in_program->getInstruction(instIdx);
        if (instPtr->bits().size() == 1)
        {
            if (instPtr->bits()[0] == qubitPair.first || instPtr->bits()[0] == qubitPair.second)
            {
                if (instPtr->name() == "Measure")
                {
                    return returnSeq(gateSequence);
                }
                else
                {
                    gateSequence.emplace_back(instIdx);
                }
            }
        }
        else if (instPtr->bits().size() == 2)
        {
            const auto areQubitOperandsMatched = [&qubitPair](const std::vector<size_t>& in_bits) {
                const bool match1 =  (in_bits[0] == qubitPair.first) && (in_bits[1] == qubitPair.second); 
                const bool match2 =  (in_bits[1] == qubitPair.first) && (in_bits[0] == qubitPair.second); 
                return match1 || match2;
            };

            const auto hasOnlyOneBitMatched = [&qubitPair](const std::vector<size_t>& in_bits) {
                const bool oneMatched = xacc::container::contains(in_bits, qubitPair.first) || xacc::container::contains(in_bits, qubitPair.second);
                const bool oneNotMatched = !xacc::container::contains(in_bits, qubitPair.first) || !xacc::container::contains(in_bits, qubitPair.second);
                return oneMatched && oneNotMatched;
            };

            if (areQubitOperandsMatched(instPtr->bits()))
            {
                gateSequence.emplace_back(instIdx);
            }
            else if (hasOnlyOneBitMatched(instPtr->bits()))
            {
                return returnSeq(gateSequence);
            }
        }
    }
    // End of circuit:
    return returnSeq(gateSequence);
}
}
}