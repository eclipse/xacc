#include <cassert>
#include "GateMergeOptimizer.hpp"
#include "GateFusion.hpp"
#include "xacc_service.hpp"

namespace {
bool compareMatIgnoreGlobalPhase(const Eigen::Matrix4cd& in_a, const Eigen::Matrix4cd& in_b)
{
    const auto flipKronOrder = [](const Eigen::Matrix4cd& in_mat){
        Eigen::Matrix4cd result = Eigen::Matrix4cd::Zero();
        const std::vector<size_t> order { 0, 2, 1, 3 };
        for (size_t i = 0; i < in_mat.rows(); ++i)
        {
            for (size_t j = 0; j < in_mat.cols(); ++j)
            {
                result(order[i], order[j]) = in_mat(i, j);
            }
        }
        return result;
    };
    
    const auto bFixed = flipKronOrder(in_b);

    // Find index of the largest element to normalize global phase.
    size_t colIdx = 0;
    size_t rowIdx = 0;
    double maxVal = std::abs(in_a(0,0));
    for (size_t i = 0; i < in_a.rows(); ++i)
    {
        for (size_t j = 0; j < in_a.cols(); ++j)
        {
            if (std::abs(in_a(i,j)) > maxVal)
            {
                maxVal = std::abs(in_a(i,j));
                colIdx = j;
                rowIdx = i;
            }
        }
    }

    const double TOL = 1e-6;
    if (std::abs(std::abs(in_a(rowIdx, colIdx)) - std::abs(bFixed(rowIdx, colIdx))) > TOL)
    {
        return false;
    }

    const std::complex<double> globalFactor = in_a(rowIdx, colIdx) / bFixed(rowIdx, colIdx);
    auto bFixedPhase = globalFactor * bFixed;
    const auto diff = (bFixedPhase - in_a).norm();
    return std::abs(diff) < TOL;
}
void flattenComposite(std::shared_ptr<CompositeInstruction> io_composite) 
{
  std::vector<xacc::InstPtr> flatten;
  InstructionIterator iter(io_composite);
  while (iter.hasNext()) 
  {
    auto inst = iter.next();
    if (!inst->isComposite()) 
    {
      flatten.emplace_back(inst->clone());
    }
  }
  io_composite->clear();
  io_composite->addInstructions(flatten);
}
// Check if the composite uses a single qubit register,
// the qubit line is determined by the qubit idx, 
// hence we don't want to accidentally merge wrong gates.
std::vector<std::string>
getBufferList(const std::shared_ptr<CompositeInstruction> &program) {
  std::set<std::string> allBuffers;
  InstructionIterator it(program);
  while (it.hasNext()) {
    auto nextInst = it.next();
    for (const auto &bufferName : nextInst->getBufferNames()) {
      allBuffers.emplace(bufferName);
    }
  }

  std::vector<std::string> result(allBuffers.begin(), allBuffers.end());
  return result;
}
} // namespace
namespace xacc {
namespace quantum {
void MergeSingleQubitGatesOptimizer::apply(std::shared_ptr<CompositeInstruction> program, const std::shared_ptr<Accelerator> accelerator, const HeterogeneousMap &options)
{
    const auto buffer_names = getBufferList(program);
    if (buffer_names.size() > 1) 
    {
        // If there are multiple buffers, we cannot apply gate merging
        // due to qubit Id ambiguity.
        return;
    }

    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    flattenComposite(program);
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
            Eigen::Matrix2cd uMat = fuser->calcFusedGate(1);
            uMat.transposeInPlace();
            Eigen::Vector4cd matAsVec(Eigen::Map<Eigen::Vector4cd>(uMat.data(), uMat.cols()*uMat.rows()));
            std::vector<std::complex<double>> flattenedUnitary;
            flattenedUnitary.resize(matAsVec.size());
            Eigen::Vector4cd::Map(&flattenedUnitary[0], flattenedUnitary.size()) = matAsVec;

            auto zyz = std::dynamic_pointer_cast<quantum::Circuit>(xacc::getService<Instruction>("z-y-z"));
            const bool expandOk = zyz->expand({ 
                std::make_pair("unitary", flattenedUnitary)
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
                if (program->nInstructions() == sequence[0])
                {
                    for (auto& newInst: zyz->getInstructions())
                    {
                        newInst->setBits({bitIdx});
                        newInst->setBufferNames({ buffer_names[0] });
                        program->addInstruction(newInst->clone());
                    }
                }
                else
                {
                    auto locationToInsert = sequence[0];
                    for (auto& newInst: zyz->getInstructions())
                    {
                        newInst->setBits({bitIdx});
                        newInst->setBufferNames({ buffer_names[0] });
                        program->insertInstruction(locationToInsert, newInst->clone());
                        locationToInsert++;
                    }
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
    const auto buffer_names = getBufferList(program);
    if (buffer_names.size() > 1) 
    {
        // If there are multiple buffers, we cannot apply gate merging
        // due to qubit Id ambiguity.
        return;
    }
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    flattenComposite(program);
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
                        return (bit == qubitPair.first) ? 1 : 0;
                    }
                    else
                    {
                        return (bit == qubitPair.first) ? 0 : 1;
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
            Eigen::Matrix4cd uMat = fuser->calcFusedGate(2);
            uMat.transposeInPlace();
            Eigen::VectorXcd matAsVec(Eigen::Map<Eigen::VectorXcd>(uMat.data(), uMat.cols()*uMat.rows()));
            std::vector<std::complex<double>> flattenedUnitary;
            flattenedUnitary.resize(matAsVec.size());
            Eigen::VectorXcd::Map(&flattenedUnitary[0], flattenedUnitary.size()) = matAsVec;
            auto kak = std::dynamic_pointer_cast<quantum::Circuit>(xacc::getService<Instruction>("kak"));
            const bool expandOk = kak->expand({ 
                std::make_pair("unitary", flattenedUnitary)
            });
            assert(expandOk);
            
            const auto calcUopt = [](const std::shared_ptr<CompositeInstruction> composite) {
                auto fuser = xacc::getService<xacc::quantum::GateFuser>("default");
                fuser->initialize(composite);
                return fuser->calcFusedGate(2);
            };
            
            // Optimized decomposed sequence:
            const auto nbInstructionsAfter = kak->nInstructions();
            uMat.transposeInPlace();
            // A simplified sequence was found.
            if (nbInstructionsAfter < sequence.size() && compareMatIgnoreGlobalPhase(uMat, calcUopt(kak)))
            {
                // Disable to remove:
                const auto programLengthBefore = program->nInstructions();
                for (const auto& instIdx: sequence)
                {
                    auto instrPtr = program->getInstruction(instIdx);
                    instrPtr->disable();
                }
                program->removeDisabled();
                if (program->nInstructions() == sequence[0])
                {
                    for (auto& newInst: kak->getInstructions())
                    {
                        newInst->setBits(remapBits(newInst->bits()));
                        newInst->setBufferNames(std::vector<std::string>(newInst->bits().size(), buffer_names[0]));
                        program->addInstruction(newInst->clone());
                    }
                }
                else
                {
                    auto locationToInsert = sequence[0];
                    for (auto& newInst: kak->getInstructions())
                    {
                        newInst->setBits(remapBits(newInst->bits()));
                        newInst->setBufferNames(std::vector<std::string>(newInst->bits().size(), buffer_names[0]));
                        program->insertInstruction(locationToInsert, newInst->clone());
                        locationToInsert++;
                    }
                }
                // Jump forward since we don't want to re-analyze this block.
                instIdx += nbInstructionsAfter;
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