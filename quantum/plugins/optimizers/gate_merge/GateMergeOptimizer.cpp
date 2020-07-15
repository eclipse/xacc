#include "GateMergeOptimizer.hpp"
#include <cassert>

namespace xacc {
namespace quantum {
void MergeSingleQubitGatesOptimizer::apply(std::shared_ptr<CompositeInstruction> program, const std::shared_ptr<Accelerator> accelerator, const HeterogeneousMap &options)
{
    std::set<size_t> processInstIdx;
    for (size_t instIdx = 0; instIdx < program->nInstructions(); ++instIdx)
    {
        const auto sequence = findSingleQubitGateSequence(program, instIdx, 2, processInstIdx);
        if (!sequence.empty()) 
        {
            for (const auto& instIdx: sequence)
            {
                assert(!xacc::container::contains(processInstIdx, instIdx));
                processInstIdx.emplace(instIdx);
            }
            
            // std::cout << "Found sequence:\n";
            // for (const auto& idx : sequence)
            // {
            //     std::cout << program->getInstruction(idx)->toString() << "\n";
            // }
        }
    }
}

std::vector<size_t> MergeSingleQubitGatesOptimizer::findSingleQubitGateSequence(const std::shared_ptr<CompositeInstruction> in_program, size_t in_startIdx, size_t in_lengthLimit, const std::set<size_t>& in_processedInstIdx) const
{
    const auto nbInstructions = in_program->nInstructions();
    std::unordered_map<size_t, std::vector<size_t>> qubitToSequence;
    
    for (size_t instIdx = in_startIdx; instIdx < nbInstructions; ++instIdx)
    {
        auto instPtr = in_program->getInstruction(instIdx);
        if (instPtr->bits().size() > 1)
        {
            // Two-qubit gate: clear the sequence of all qubit operands.
            // Returns if any sequence meets the length.
            auto& seq1 = qubitToSequence[instPtr->bits()[0]];
            auto& seq2 = qubitToSequence[instPtr->bits()[1]];
            // Returns if a sequence has been accumulated.
            if (seq1.size() >= in_lengthLimit || seq2.size() >= in_lengthLimit)
            {
                if (seq1.size() >= in_lengthLimit && seq2.size() >= in_lengthLimit)
                {
                    assert(seq1[0] != seq2[0]);
                    return (seq1[0] < seq2[0]) ?  seq1 : seq2;
                }
                
                return (seq1.size() >= in_lengthLimit) ? seq1 :  seq2;
            }

            seq1.clear();
            seq2.clear();
        }
        else
        {
            if (instPtr->name() == "Measure")
            {
                if (qubitToSequence[instPtr->bits()[0]].size() >= in_lengthLimit)
                {
                    return qubitToSequence[instPtr->bits()[0]];
                }
                qubitToSequence[instPtr->bits()[0]].clear();
            }
            else
            {
                // If this single-qubit gate has not been covered before.
                if (!xacc::container::contains(in_processedInstIdx, instIdx))
                {
                    // Single-qubit gate: accumulate the sequence.
                    auto& currentSeq = qubitToSequence[instPtr->bits()[0]];
                    currentSeq.emplace_back(instIdx);
                }
            }
        }
    }
    
    // Handle multiple un-terminated long sequence.
    size_t minInstIdx = in_program->nInstructions();
    for (const auto& [qIdx, seq]: qubitToSequence)
    {
        if (seq.size() >= in_lengthLimit)
        {
            minInstIdx = (seq[0] < minInstIdx) ? seq[0] : minInstIdx;
        }
    }

    for (const auto& [qIdx, seq]: qubitToSequence)
    {
        if (seq.size() >= in_lengthLimit && seq[0] == minInstIdx)
        {
            return seq;
        }
    }
    return {};
}
}
}