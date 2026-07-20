#pragma once

#include "IRTransformation.hpp"
namespace xacc {
namespace quantum {
// Merge contiguous sequence of gates (1- or 2-qubit) 
// into a more efficient gate sequence.
class MergeSingleQubitGatesOptimizer : public IRTransformation
{
public:
    virtual void apply(std::shared_ptr<CompositeInstruction> program,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap &options = {}) override;
    virtual const IRTransformationType type() const override { return IRTransformationType::Optimization; }
    const std::string name() const override { return "single-qubit-gate-merging"; }
    const std::string description() const override { return ""; }
private: 
    std::vector<size_t> findSingleQubitGateSequence(const std::shared_ptr<CompositeInstruction> in_program, size_t in_startIdx, size_t in_lengthLimit) const;
};

// Try to merge a block containing single and two-qubit gates
// and decompose it into a more optimized representation only if *possible*.
// e.g. only if the original block has more gates than its re-synthesized version.  
// We could also define a custom target for re-write 
// such as reducing the number of two-qubit gates in the block
// rather than the overall gate count.
class MergeTwoQubitBlockOptimizer : public IRTransformation
{
public:
    virtual void apply(std::shared_ptr<CompositeInstruction> program,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap &options = {}) override;
    virtual const IRTransformationType type() const override { return IRTransformationType::Optimization; }
    const std::string name() const override { return "two-qubit-block-merging"; }
    const std::string description() const override { return ""; }

private:
    // Finds the sequence of gates (from the start index) which forms a two-qubit block (with no connections to outside the block)
    std::vector<size_t> findGateSequence(const std::shared_ptr<CompositeInstruction> in_program, size_t in_startIdx, size_t in_lengthLimit, std::pair<size_t, size_t>& out_qubitPair) const;    
};
}
}