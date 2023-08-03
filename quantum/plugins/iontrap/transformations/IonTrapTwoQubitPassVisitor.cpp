#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Accelerator.hpp"
#include "IonTrapTwoQubitPassVisitor.hpp"

namespace xacc {
namespace quantum {
    IonTrapTwoQubitPassVisitor::IonTrapTwoQubitPassVisitor()
    {
        _instIdx = -1;                 // -1 means uninitialized - do we need typedef, enum for this?
        _bInstructionVisited = false;
    }

    bool IonTrapTwoQubitPassVisitor::instructionVisited()
    {
        return _bInstructionVisited;
    }

    void IonTrapTwoQubitPassVisitor::initializeInstructionVisitor(std::size_t instructionIndex)
    {
        // initialize _instIdx to the index of the first instruction handled by this visitor
        _instIdx = instructionIndex;
        // set visited bool to false
        _bInstructionVisited = false;
    }

    std::pair<double, double> IonTrapTwoQubitPassVisitor::findMSPhases(IonTrapMSPhaseMap *msPhases, Instruction *cnot) {
        std::size_t leftIdx = std::min(cnot->bits()[0], cnot->bits()[1]);
        std::size_t rightIdx = std::max(cnot->bits()[0], cnot->bits()[1]);
        auto idxPair = std::make_pair(leftIdx, rightIdx);

        if (!msPhases->count(idxPair)) {
            xacc::error("pair " + std::to_string(leftIdx) + "," + std::to_string(rightIdx)
                        + " missing from set of MS phases");
        }

        auto phasePair = (*msPhases)[idxPair];
        double controlMSPhase = (leftIdx == cnot->bits()[0])? phasePair.first : phasePair.second;
        double targetMSPhase = (rightIdx == cnot->bits()[1])? phasePair.second : phasePair.first;    return std::make_pair(controlMSPhase, targetMSPhase);

        return std::make_pair(controlMSPhase, targetMSPhase);
    }

    void IonTrapTwoQubitPassVisitor::visit(CNOT& cnot) {   
        std::shared_ptr<CompositeInstruction> program = _paramsMap.get<std::shared_ptr<xacc::CompositeInstruction>>("composite");
        std::size_t instIdx = _instIdx;

        // TODO!!! check with others if this name check is acceptaable
        // this CNOT visitor is beingtriggerred by an XX instruction, so weed out for now
        if (program->getInstruction(instIdx)->name() != std::string("CNOT"))
        {
            return;
        }

        _bInstructionVisited = true;
        
        xacc::HeterogeneousMap options = _paramsMap.get<xacc::HeterogeneousMap>("options");

        IonTrapMSPhaseMap *msPhases = options.get<IonTrapMSPhaseMap*>("ms-phases");
        IonTrapLogTransformCallback logTransCallback = nullptr;

        auto [controlMSPhase, targetMSPhase] = findMSPhases(msPhases, &cnot); 

        if (options.keyExists<IonTrapLogTransformCallback>("log-trans-cb")) {
            logTransCallback = options.get<IonTrapLogTransformCallback>("log-trans-cb");
        }

        auto _gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
        InstPtr ry1 = _gateRegistry->createInstruction("Ry", {cnot.bits()[0]}, {-M_PI/2.0});
        InstPtr xx = _gateRegistry->createInstruction("XX", cnot.bits(), {M_PI/4.0});

        InstPtr ry2 = _gateRegistry->createInstruction("Ry", {cnot.bits()[0]}, {M_PI/2.0});
        InstPtr rz = _gateRegistry->createInstruction("Rz", {cnot.bits()[0]}, {M_PI/2.0});
        InstPtr rx = _gateRegistry->createInstruction("Rx", {cnot.bits()[1]}, {M_PI/2.0});

        program->insertInstruction(instIdx++, ry1);
        // TODO: Note that this is kind of incorrect: really, the combination of these Rz gates
        //       and an MS gate is actually an XX gate (see https://doi.org/10.1088/1367-2630/18/2/023048)
        //       but we are surrounding an XX instruction with Rz instructions. But this will
        //       work for now
        if (controlMSPhase) {
            InstPtr msRz1 = _gateRegistry->createInstruction("Rz", {cnot.bits()[0]}, {controlMSPhase});
            program->insertInstruction(instIdx++, msRz1);
        }
        if (targetMSPhase) {
            InstPtr msRz2 = _gateRegistry->createInstruction("Rz", {cnot.bits()[1]}, {targetMSPhase});
            program->insertInstruction(instIdx++, msRz2);
        }
        program->insertInstruction(instIdx++, xx);
        if (controlMSPhase) {
            InstPtr msRz3 = _gateRegistry->createInstruction("Rz", {cnot.bits()[0]}, {-controlMSPhase});
            program->insertInstruction(instIdx++, msRz3);
        }
        if (targetMSPhase) {
            InstPtr msRz4 = _gateRegistry->createInstruction("Rz", {cnot.bits()[1]}, {-targetMSPhase});
            program->insertInstruction(instIdx++, msRz4);
        }
        program->insertInstruction(instIdx++, ry2);
        program->insertInstruction(instIdx++, rz);
        program->insertInstruction(instIdx++, rx);

        // update the logTransCallback  if necessary
        if (logTransCallback) {
            std::vector<InstPtr> newInsts;
           for (std::size_t j = _instIdx; j < instIdx; j++) {
                newInsts.push_back(program->getInstruction(j));
            }
 
            // log the current instruction (_instIdx-1), along with newInst tansforms {newInsts}
            logTransCallback({program->getInstruction(_instIdx-1)}, {newInsts});
        }
    }

    void IonTrapTwoQubitPassVisitor::visit(CH& ch) { 
        _bInstructionVisited = true;

        auto _gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
        
        std::shared_ptr<CompositeInstruction> program = _paramsMap.get<std::shared_ptr<xacc::CompositeInstruction>>("composite");
        std::size_t instIdx = _instIdx;
        xacc::HeterogeneousMap options = _paramsMap.get<xacc::HeterogeneousMap>("options");

        IonTrapMSPhaseMap *msPhases = options.get<IonTrapMSPhaseMap*>("ms-phases");
        IonTrapLogTransformCallback logTransCallback = nullptr;
        
        if (options.keyExists<IonTrapLogTransformCallback>("log-trans-cb")) {
            logTransCallback = options.get<IonTrapLogTransformCallback>("log-trans-cb");
        }

        InstPtr s   = _gateRegistry->createInstruction("S", {ch.bits()[1]});
        InstPtr h   = _gateRegistry->createInstruction("H", {ch.bits()[1]});
        InstPtr t   = _gateRegistry->createInstruction("T", {ch.bits()[1]});
        InstPtr cx  = _gateRegistry->createInstruction("CNOT", ch.bits());
        InstPtr tdg = _gateRegistry->createInstruction("Tdg", {ch.bits()[1]});
        InstPtr h2  = _gateRegistry->createInstruction("H", {ch.bits()[1]});
        InstPtr sdg = _gateRegistry->createInstruction("Sdg", {ch.bits()[1]});

        program->insertInstruction(instIdx, s);
        program->insertInstruction(instIdx+1, h);
        program->insertInstruction(instIdx+2, t);
        program->insertInstruction(instIdx+3, cx);
        program->insertInstruction(instIdx+4, tdg);
        program->insertInstruction(instIdx+5, h2);
        program->insertInstruction(instIdx+6, sdg); 

        if (logTransCallback) {
            logTransCallback({program->getInstruction(instIdx+7)},
                                {program->getInstruction(instIdx),
                                program->getInstruction(instIdx+1),
                                program->getInstruction(instIdx+2),
                                program->getInstruction(instIdx+3),
                                program->getInstruction(instIdx+4),
                                program->getInstruction(instIdx+5),
                                program->getInstruction(instIdx+6)});
        }   
    }

    void IonTrapTwoQubitPassVisitor::visit(CY& cy) { 
        _bInstructionVisited = true;
        auto _gateRegistry = xacc::getService<xacc::IRProvider>("quantum");    
        std::shared_ptr<CompositeInstruction> program = _paramsMap.get<std::shared_ptr<xacc::CompositeInstruction>>("composite");
        std::size_t instIdx = _instIdx;
        xacc::HeterogeneousMap options = _paramsMap.get<xacc::HeterogeneousMap>("options");

        IonTrapMSPhaseMap *msPhases = options.get<IonTrapMSPhaseMap*>("ms-phases");
        IonTrapLogTransformCallback logTransCallback = nullptr;
        
        if (options.keyExists<IonTrapLogTransformCallback>("log-trans-cb")) {
            logTransCallback = options.get<IonTrapLogTransformCallback>("log-trans-cb");
        }

        InstPtr sdg = _gateRegistry->createInstruction("Sdg", {cy.bits()[1]});
        InstPtr cx = _gateRegistry->createInstruction("CNOT", cy.bits());
        InstPtr s = _gateRegistry->createInstruction("S", {cy.bits()[1]});

        program->insertInstruction(instIdx, sdg);
        program->insertInstruction(instIdx+1, cx);
        program->insertInstruction(instIdx+2, s);

        if (logTransCallback) {
            logTransCallback({program->getInstruction(instIdx+3)},
                                {program->getInstruction(instIdx),
                                program->getInstruction(instIdx+1),
                                program->getInstruction(instIdx+2)});
        }
    }

    void IonTrapTwoQubitPassVisitor::visit(CZ& cz) { 
        _bInstructionVisited = true;
        // get params from hmap
        auto _gateRegistry = xacc::getService<xacc::IRProvider>("quantum");       
        std::shared_ptr<CompositeInstruction> program = _paramsMap.get<std::shared_ptr<xacc::CompositeInstruction>>("composite");
        std::size_t instIdx = _instIdx;
        xacc::HeterogeneousMap options = _paramsMap.get<xacc::HeterogeneousMap>("options");

        IonTrapMSPhaseMap *msPhases = options.get<IonTrapMSPhaseMap*>("ms-phases");
        IonTrapLogTransformCallback logTransCallback = nullptr;
        
        if (options.keyExists<IonTrapLogTransformCallback>("log-trans-cb")) {
            logTransCallback = options.get<IonTrapLogTransformCallback>("log-trans-cb");
        }

        InstPtr h = _gateRegistry->createInstruction("H", {cz.bits()[1]});
        InstPtr cx = _gateRegistry->createInstruction("CNOT", cz.bits());
        InstPtr h2 = _gateRegistry->createInstruction("H", {cz.bits()[1]});

        program->insertInstruction(instIdx, h);
        program->insertInstruction(instIdx+1, cx);
        program->insertInstruction(instIdx+2, h2);

        if (logTransCallback) {
            logTransCallback({program->getInstruction(instIdx+3)},
                                {program->getInstruction(instIdx),
                                program->getInstruction(instIdx+1),
                                program->getInstruction(instIdx+2)});
        }
    }
    void IonTrapTwoQubitPassVisitor::visit(Swap& swap) { 
        _bInstructionVisited = true;
        auto _gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
        std::shared_ptr<CompositeInstruction> program = _paramsMap.get<std::shared_ptr<xacc::CompositeInstruction>>("composite");
        std::size_t instIdx = _instIdx;
        xacc::HeterogeneousMap options = _paramsMap.get<xacc::HeterogeneousMap>("options");

        IonTrapMSPhaseMap *msPhases = options.get<IonTrapMSPhaseMap*>("ms-phases");
        IonTrapLogTransformCallback logTransCallback = nullptr;
        
        if (options.keyExists<IonTrapLogTransformCallback>("log-trans-cb")) {
            logTransCallback = options.get<IonTrapLogTransformCallback>("log-trans-cb");
        }

        InstPtr cx1 = _gateRegistry->createInstruction("CNOT", swap.bits());
        InstPtr cx2 = _gateRegistry->createInstruction("CNOT", {swap.bits()[1], swap.bits()[0]});
        InstPtr cx3 = _gateRegistry->createInstruction("CNOT", swap.bits());

        program->insertInstruction(instIdx, cx1);
        program->insertInstruction(instIdx+1, cx2);
        program->insertInstruction(instIdx+2, cx3);

        if (logTransCallback) {
            logTransCallback({program->getInstruction(instIdx+3)},
                                {program->getInstruction(instIdx),
                                program->getInstruction(instIdx+1),
                                program->getInstruction(instIdx+2)});
        }
    }
} // quantum
} // xacc 
