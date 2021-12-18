/*******************************************************************************
 * Copyright (c) 2019-2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *   Daniel Strano - adaption from Quantum++ to Qrack
 *   Austin Adams - adaption for GTRI testbed
 *******************************************************************************/
#ifndef QUANTUM_ACCELERATORS_IONTRAPACCELERATOR_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPACCELERATOR_HPP_

#include "xacc.hpp"
#include "IonTrapOneQubitPass.hpp"
#include "IonTrapTwoQubitPass.hpp"
#include "IonTrapQasmVisitor.hpp"
#include "IonTrapPassesCommon.hpp"

// On Linux, use inotify to check for result files. On other operating
// systems, use polling instead
#ifdef __linux__
#define IONTRAP_USE_INOTIFY
#endif

namespace xacc {
namespace quantum {

class IonTrapAccelerator : public Accelerator {
public:
    // Identifiable interface impls
    virtual const std::string name() const override { return "iontrap"; }
    virtual const std::string description() const override { return "XACC Accelerator targetting the IonTrap general-purpose quantum testbed"; }

    // Accelerator interface impls
    virtual void initialize(const HeterogeneousMap& params = {}) override;
    virtual void updateConfiguration(const HeterogeneousMap& config) override;
    virtual const std::vector<std::string> configurationKeys() override;
    virtual BitOrder getBitOrder() override { return BitOrder::LSB; }
    virtual std::vector<std::pair<int, int>> getConnectivity() override;
    virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> compositeInstruction) override;
    virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions) override;

private:
    static constexpr double RESULT_CHECK_INTERVAL_SEC = 1;

    std::string inputExt;
    bool adjacentIonCouplingsOnly;
    bool serializedSeq;
    bool keepTempFiles;
    bool emitQasm;
    bool keepTrailingGates;
    bool keepRzBeforeMeas;
    bool keepRxBeforeXX;
    bool keepLeadingRz;
    double threshold;
    std::size_t numQubits;
    IonTrapCouplingMap couplingMap;
    IonTrapMSPhaseMap msPhases;
    std::string watchdir;
    std::string logTransformsPath;

    void readConfig();
    std::map<std::string, std::string> readConfigKeys();
    void compileToNativeGatesAndLogTransforms(std::shared_ptr<CompositeInstruction>);
    void compileToNativeGates(std::shared_ptr<CompositeInstruction>, IonTrapLogTransformCallback);
#ifdef IONTRAP_USE_INOTIFY
    int getInotifyFd(std::string);
    void waitForResultFileInotify(int, std::string &);
#else
    void waitForResultFile(std::string &);
#endif
    std::vector<InstPtr> findOpTableRow(const std::shared_ptr<CompositeInstruction>,
                                        std::size_t &);
    void writeOpTable(const std::shared_ptr<CompositeInstruction>, std::ostream &);
    void writeInput(std::shared_ptr<AcceleratorBuffer>,
                    const std::shared_ptr<CompositeInstruction>,
                    const std::string &,
                    std::multimap<std::size_t, std::size_t> &);
    double calcExpZ(std::vector<std::complex<double>> &,
                    std::multimap<std::size_t, std::size_t> &);
    void readResult(std::shared_ptr<AcceleratorBuffer>, const std::string &,
                    std::multimap<std::size_t, std::size_t> &);
    std::string randomChars();
};
}}

#endif
