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
#include <typeinfo>
#include <fstream>
#include <cerrno>
#include <random>
#include <thread>
#include "InstructionIterator.hpp"
#include "xacc_service.hpp"
#include "IonTrapAccelerator.hpp"

extern "C" {
#include <unistd.h>
#ifdef IONTRAP_USE_INOTIFY
#include <sys/inotify.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif
}

namespace xacc {
namespace quantum {

    const std::vector<std::string> IonTrapAccelerator::configurationKeys() {
        return {"watchdir", "adjacent-ion-couplings-only", "serialized-seq",
                "save-temps", "emit-qasm", "keep-leading-rz", "keep-rx-before-xx",
                "keep-rz-before-meas", "keep-trailing-gates", "input-ext",
                "log-trans-path", "threshold"};
    }

    void IonTrapAccelerator::initialize(const HeterogeneousMap &params) {
        adjacentIonCouplingsOnly = params.keyExists<bool>("adjacent-ion-couplings-only")
                                   && params.get<bool>("adjacent-ion-couplings-only");
        serializedSeq = params.keyExists<bool>("serialized-seq")
                        && params.get<bool>("serialized-seq");
        keepTempFiles = params.keyExists<bool>("save-temps")
                        && params.get<bool>("save-temps");
        emitQasm = params.keyExists<bool>("emit-qasm")
                   && params.get<bool>("emit-qasm");
        keepTrailingGates = params.keyExists<bool>("keep-trailing-gates")
                            && params.get<bool>("keep-trailing-gates");
        keepRzBeforeMeas = params.keyExists<bool>("keep-rz-before-meas")
                           && params.get<bool>("keep-rz-before-meas");
        keepRxBeforeXX = params.keyExists<bool>("keep-rx-before-xx")
                         && params.get<bool>("keep-rx-before-xx");
        keepLeadingRz = params.keyExists<bool>("keep-leading-rz")
                        && params.get<bool>("keep-leading-rz");

        if (params.keyExists<std::string>("input-ext")) {
            inputExt = params.get<std::string>("input-ext");
        }

        if (params.keyExists<std::string>("log-trans-path")) {
            logTransformsPath = params.get<std::string>("log-trans-path");
        }

        if (params.keyExists<double>("threshold")) {
            threshold = params.get<double>("threshold");
        } else {
            threshold = IonTrapOneQubitPass::DEFAULT_THRESHOLD;
        }

        if (params.keyExists<std::string>("watchdir")) {
            watchdir = params.get<std::string>("watchdir");
        } else {
            xacc::error("watchdir parameter is required");
            return;
        }

        readConfig();
    }

    void IonTrapAccelerator::updateConfiguration(const HeterogeneousMap &params) {
        if (params.keyExists<bool>("adjacent-ion-couplings-only")) {
            adjacentIonCouplingsOnly = params.get<bool>("adjacent-ion-couplings-only");
        }
        if (params.keyExists<bool>("serialized-seq")) {
            serializedSeq = params.get<bool>("serialized-seq");
        }
        if (params.keyExists<bool>("save-temps")) {
            keepTempFiles = params.get<bool>("save-temps");
        }
        if (params.keyExists<bool>("emit-qasm")) {
            emitQasm = params.get<bool>("emit-qasm");
        }
        if (params.keyExists<bool>("keep-trailing-gates")) {
            keepTrailingGates = params.get<bool>("keep-trailing-gates");
        }
        if (params.keyExists<bool>("keep-rz-before-meas")) {
            keepRzBeforeMeas = params.get<bool>("keep-rz-before-meas");
        }
        if (params.keyExists<bool>("keep-rx-before-xx")) {
            keepRxBeforeXX = params.get<bool>("keep-rx-before-xx");
        }
        if (params.keyExists<bool>("keep-leading-rz")) {
            keepLeadingRz = params.get<bool>("keep-leading-rz");
        }
        if (params.keyExists<std::string>("input-ext")) {
            inputExt = params.get<std::string>("input-ext");
        }
        if (params.keyExists<std::string>("log-trans-path")) {
            logTransformsPath = params.get<std::string>("log-trans-path");
        }
        if (params.keyExists<double>("threshold")) {
            threshold = params.get<double>("threshold");
        }
        if (params.keyExists<std::string>("watchdir")) {
            watchdir = params.get<std::string>("watchdir");
            readConfig();
        }
    }

    void IonTrapAccelerator::readConfig() {
        auto config = readConfigKeys();

        if (!config.count("NumberOfIons")) {
            xacc::error("config file missing NumberOfIons");
        } else {
            numQubits = (std::size_t) std::stoul(config["NumberOfIons"]);
        }

        couplingMap.clear();
        msPhases.clear();

        static const std::string msPhasePrefix("MS_Phi");

        for (auto &kv : config) {
            auto &key = kv.first;
            auto &val = kv.second;
            if (key.find(msPhasePrefix) == 0) {
                std::size_t left = key[key.size() - 2] - '0';
                std::size_t right = key[key.size() - 1] - '0';

                if (adjacentIonCouplingsOnly && left + 1 != right) {
                    continue;
                }

                std::pair<std::size_t, std::size_t> ionPair(left, right);
                couplingMap.insert(ionPair);

                std::size_t idx = key[msPhasePrefix.size()] - '0';
                double phase = std::stod(val);
                if (msPhases.count(ionPair)) {
                    msPhases[ionPair] = std::make_pair((idx == left)? phase : msPhases[ionPair].first,
                                                       (idx == right)? phase : msPhases[ionPair].second);
                } else {
                    msPhases[ionPair] = std::make_pair(phase, phase);
                }
            }
        }

        return;
    }

    std::map<std::string, std::string> IonTrapAccelerator::readConfigKeys() {
        std::string configFilename = watchdir + "/config.txt";
        std::ifstream configFile(configFilename, std::ios::binary);
        std::map<std::string, std::string> config;

        if (configFile.is_open()) {
            std::string line;
            while (std::getline(configFile, line, '\n')) {
                xacc::trim(line);

                auto idx = line.find('=');
                if (idx == std::string::npos) {
                    xacc::warning("invalid line in config file: " + line);
                    continue;
                }

                std::string key = line.substr(0, idx);
                std::string value = line.substr(idx + 1, std::string::npos);
                config[key] = value;
            }
        } else {
            xacc::error("failed to open file " + configFilename);
            return config;
        }

        configFile.close();
        return config;
    }

    // Useful debugging/stats feature: log all IR transformations
    // performed by the backend. This way, we can see which CNOTs became
    // which MS gates, and which single-qubit gates become which pi/2
    // rotations. I have a local python script to convert this log file
    // to graphviz, but I am trying to find a better visualization
    void IonTrapAccelerator::compileToNativeGatesAndLogTransforms(std::shared_ptr<CompositeInstruction> compositeInstruction) {
        std::size_t nextId = 0;
        // Hack to allow us to attach additional data to
        // instructions
        std::map<void *, std::size_t> instIds;
        std::ofstream logTransformsFile(logTransformsPath, std::ios::binary);

        auto logTransformsCallback = [&](std::vector<InstPtr> oldInsts, std::vector<InstPtr> newInsts) {
            // Skip lines with only measurements on RHS
            bool nonMeasurement = false;
            for (auto inst : newInsts) {
                nonMeasurement = nonMeasurement || inst->name() != "Measure";
            }
            if (!nonMeasurement) {
                return;
            }

            std::string sep = "";
            for (auto inst : oldInsts) {
                auto id = instIds[(void *)inst.get()];
                logTransformsFile << sep << id;
                sep = ";";
            }

            logTransformsFile << " -> ";

            sep = "";
            for (auto inst : newInsts) {
                // Ignore measurements
                if (inst->name() == "Measure") {
                    continue;
                }

                std::size_t id = nextId++;
                instIds[(void *)inst.get()] = id;
                logTransformsFile << sep << id << "[" << inst->name();

                if (inst->nParameters()) {
                    logTransformsFile << "(";
                    std::string sep2 = "";
                    for (auto param : inst->getParameters()) {
                        logTransformsFile << sep2 << InstructionParameterToDouble(param);
                        sep2 = ",";
                    }
                    logTransformsFile << ")";
                }

                logTransformsFile << " ";
                std::string sep2 = "";
                for (auto bit : inst->bits()) {
                    logTransformsFile << sep2 << bit;
                    sep2 = ",";
                }

                logTransformsFile << "]";
                sep = ";";
            }

            logTransformsFile << "\n";
        };

        InstructionIterator it(compositeInstruction);
        while (it.hasNext()) {
            auto nextInst = it.next();
            if (!nextInst->isEnabled() || nextInst->isComposite()) {
                continue;
            }
            logTransformsCallback({}, {nextInst});
        }

        compileToNativeGates(compositeInstruction, logTransformsCallback);
        logTransformsFile.close();
    }

    void IonTrapAccelerator::compileToNativeGates(std::shared_ptr<CompositeInstruction> compositeInstruction,
                                          IonTrapLogTransformCallback logTransformsCallback) {
        auto twoQubitPass = xacc::getService<IRTransformation>("iontrap-2q-pass");
        auto oneQubitPass = xacc::getService<IRTransformation>("iontrap-1q-pass");
        twoQubitPass->apply(compositeInstruction, nullptr, {{"ms-phases", &msPhases},
                                                            {"log-trans-cb", logTransformsCallback}});
        oneQubitPass->apply(compositeInstruction, nullptr, {{"log-trans-cb", logTransformsCallback},
                                                            {"keep-trailing-gates", keepTrailingGates},
                                                            {"keep-rz-before-meas", keepRzBeforeMeas},
                                                            {"keep-rx-before-xx", keepRxBeforeXX},
                                                            {"keep-leading-rz", keepLeadingRz},
                                                            {"threshold", threshold}});
    }

#ifdef IONTRAP_USE_INOTIFY
    int IonTrapAccelerator::getInotifyFd(std::string watchdir) {
        int inotify_fd;
        if ((inotify_fd = inotify_init()) < 0) {
            xacc::error(std::string("inotify_init: ") + std::strerror(errno));
            return -1;
        }

        // IN_MOVED_TO is the relevant event here, since the Igor code
        // will rename() a temp file to the final output file once
        // synced fully to disk. But use IN_CREATE in case this changes
        // in the future
        if (inotify_add_watch(inotify_fd, watchdir.c_str(), IN_CREATE | IN_MOVED_TO) < 0) {
            int err = errno;
            close(inotify_fd);
            xacc::error(std::string("inotify_add_watch: ") + std::strerror(err));
            return -1;
        }

        return inotify_fd;
    }

    void IonTrapAccelerator::waitForResultFileInotify(int inotify_fd, std::string &resultFilename) {
        char buf[4096];
        int found = 0;

        while (!found) {
            ssize_t n;
            if ((n = read(inotify_fd, buf, sizeof buf)) < 0) {
                int err = errno;
                close(inotify_fd);
                xacc::error(std::string("read inotify fd ") + std::strerror(err));
                return;
            } else if (!n) {
                close(inotify_fd);
                xacc::error("inotify fd unexpectedly sent eof");
                return;
            }

            const struct inotify_event *event;
            size_t i;
            for (i = 0; !found && i < n; i += sizeof *event + event->len) {
                event = (const struct inotify_event *)(buf + i);
                if (event->len) {
                    std::string eventFilename(event->name);
                    if (eventFilename == resultFilename) {
                        xacc::debug("got inotify event for result file `" + resultFilename + "', parsing");
                        found = 1;
                    } else {
                        xacc::debug("got inotify event for `" + eventFilename + "', not result file `" + resultFilename + "'. ignoring");
                    }
                }
            }
        }

        close(inotify_fd);
        return;
    }
#else
	void IonTrapAccelerator::waitForResultFile(std::string &resultPath) {
        for (;;) {
            struct stat buf;
            if (!stat(resultPath.c_str(), &buf)) {
                xacc::debug("found result file `" + resultPath + "', parsing");
                return;
            } else {
                std::this_thread::sleep_for(std::chrono::duration<double>(RESULT_CHECK_INTERVAL_SEC));
            }
        }
    }
#endif

    // TODO: add example of revised seq file
    std::vector<InstPtr> IonTrapAccelerator::findOpTableRow(const std::shared_ptr<CompositeInstruction> program,
                                                         std::size_t &startIdx) {
        std::size_t i;
        std::vector<InstPtr> ret;
        std::set<std::size_t> blocked;
        bool advanceStartIdx = true;
        bool done = false;

        for (i = startIdx; !done && i < program->nInstructions(); i++) {
            InstPtr inst = program->getInstruction(i);

            // Measure instructions handled before this method
            if (!inst->isEnabled() || inst->name() == "Measure") {
                // ignore
            } else if (inst->bits().size() > 1) {
                if (!ret.size()) {
                    ret.push_back(inst);
                    inst->disable();
                    done = true;
                } else {
                    advanceStartIdx = false;
                    for (std::size_t ionIdx : inst->bits()) {
                        blocked.insert(ionIdx);
                    }
                }
            } else { // single-qubit
                std::size_t ionIdx = inst->bits()[0];
                if (blocked.count(ionIdx)) {
                    advanceStartIdx = false;
                } else {
                    ret.push_back(inst);
                    inst->disable();
                    blocked.insert(ionIdx);
                    done = serializedSeq || blocked.size() == numQubits;
                }
            }

            if (advanceStartIdx) {
                startIdx++;
            }
        }

        return ret;
    }

    void IonTrapAccelerator::writeOpTable(const std::shared_ptr<CompositeInstruction> program,
                                       std::ostream &seqFile) {
        for (std::size_t i = 0; i < program->nInstructions(); i++) {
            InstPtr inst = program->getInstruction(i);
            if (inst->isComposite()) {
                xacc::error("compositeInstruction is not flattened, even after "
                            "compilation IRTransformations somehow");
                return;
            }
        }

        std::size_t instIdx = 0;
        std::vector<InstPtr> row;

        while (!(row = findOpTableRow(program, instIdx)).empty()) {
            if (row[0]->name() == "XX") {
                if (row.size() != 1) {
                    xacc::error("parallel row has multiple XX gates");
                    return;
                }

                // TODO: hold on! is alpha == pi/4?

                InstPtr inst = row[0];
                std::size_t leftIon, rightIon;
                leftIon = std::min<std::size_t>(inst->bits()[0], inst->bits()[1]);
                rightIon = std::max<std::size_t>(inst->bits()[0], inst->bits()[1]);
                seqFile << "2," << leftIon << "," << rightIon << "," << "0";
                for (std::size_t i = 1; i < numQubits; i++) {
                    seqFile << ",,";
                }
                seqFile << "\r\n";
                continue;
            }

            // These are single-qubit rotations
            seqFile << "1,0";

            std::set<std::size_t> bitsPresent;

            for (InstPtr inst : row) {
                if (inst->name() == "XX") {
                    xacc::error("parallel row has multiple XX gates");
                    return;
                } else if (inst->name() == "Rphi") {
                    std::size_t ion = inst->bits()[0];

                    if (!bitsPresent.insert(ion).second) {
                        // IGOR actually supports this in its current
                        // state, but it is not physically possible
                        xacc::error("parallel row has duplicated ions!");
                        return;
                    }

                    // TODO: hold on! is theta = pi/2?
                    double phi = inst->getParameter(0).as<double>();
                    seqFile << "," << ion << "," << phi;
                } else {
                    xacc::error("unknown instruction name " + inst->name());
                    return;
                }
            }

            // NaN for unused qubits
            for (std::size_t i = row.size(); i < numQubits; i++) {
                seqFile << ",,";
            }
            seqFile << "\r\n";
        }
    }

    void IonTrapAccelerator::writeInput(std::shared_ptr<AcceleratorBuffer> buffer,
                                     const std::shared_ptr<CompositeInstruction> compositeInstruction,
                                     const std::string &inputFilename,
                                     std::multimap<std::size_t, std::size_t> &measuredBits) {
        std::shared_ptr<IonTrapQasmVisitor> visitor;
        std::ofstream inputFile(inputFilename, std::ios::binary);
        inputFile.precision(std::numeric_limits<double>::max_digits10);

        if (emitQasm) {
            visitor = std::make_shared<IonTrapQasmVisitor>(buffer->size(), couplingMap, inputFile);
            visitor->writeHeader();
        }

        InstructionIterator it(compositeInstruction);
        while (it.hasNext()) {
            auto nextInst = it.next();
            if (!nextInst->isEnabled() || nextInst->isComposite()) {
                continue;
            }

            auto bits = nextInst->bits();

            if (nextInst->name() == "Measure") {
                if (nextInst->bits().size() < 1) {
                    xacc::warning("Measurement with no qubits!");
                }

                for (std::size_t bit : nextInst->bits()) {
                    if (bit >= numQubits) {
                        xacc::error("Attempt to measure qubit larger than number of physical qubits");
                    }
                    measuredBits.insert({bit, measuredBits.size()});
                }
            } else if (std::any_of(bits.cbegin(), bits.cend(),
                                  [&](std::size_t bit) { return measuredBits.count(bit); })) {
                xacc::error("Cannot perform intra-circuit measurements");
            } else if (emitQasm) {
                nextInst->accept(visitor);
            }
        }

        if (!emitQasm) {
            writeOpTable(compositeInstruction, inputFile);
        }

        inputFile.close();

        if (measuredBits.empty()) {
            xacc::warning("Circuit with no measurements!");
        }
    }

    double IonTrapAccelerator::calcExpZ(std::vector<std::complex<double>> &amplitudes,
                                     std::multimap<std::size_t, std::size_t> &measuredBits) {
        // Algorithm yoinked from QppVisitor::calcExpectationValueZ()
        std::size_t expectedSize = 1 << numQubits;
        if (amplitudes.size() != expectedSize) {
            xacc::error("wrong number of amplitudes: " + std::to_string(amplitudes.size())
                        + " instead of " + std::to_string(expectedSize));
            return 0.0;
        }

        double result = 0.0;
        for (std::size_t i = 0; i < amplitudes.size(); i++) {
            std::size_t count = 0;
            for (auto pair : measuredBits) {
                std::size_t physBitIdx, logicalBitIdx;
                std::tie(physBitIdx, logicalBitIdx) = pair;

                if ((i >> physBitIdx) & 0x1) {
                    count++;
                }
            }
            bool odd = count & 1;
            result += (odd ? -1.0 : 1.0) * std::norm(amplitudes[i]);
        }

        return result;
    }

    // Example results file:
    // NumRuns:8192
    //
    // State:
    // -1 + I*0
    // 0 + I*0
    // 0 + I*0
    // -1 + I*0
    // 0 + I*0
    // 0 + I*0
    // 0 + I*0
    // 0 + I*0
    //
    // Simulation:
    // 000:4095
    // 011:4097
    void IonTrapAccelerator::readResult(std::shared_ptr<AcceleratorBuffer> buffer,
                                     const std::string &resultFilename,
                                     std::multimap<std::size_t, std::size_t> &measuredBits) {
        std::ifstream resultFile(resultFilename, std::ios::binary);
        bool readingState = false;
        bool readingSim = false;
        std::vector<std::complex<double>> amplitudes;
        std::map<std::string, int> counts;

        while (!resultFile.eof()) {
            std::string line;
            std::getline(resultFile, line, '\n');
            xacc::trim(line);

            if (line.empty()) {
                // Ignore empty lines
                readingSim = readingState = false;
            } else if (readingState) {
                std::size_t plusIdx = line.find('+');
                std::string realStr = line.substr(0, plusIdx);
                xacc::trim(realStr);

                std::string imagStr = line.substr(plusIdx+1);
                std::size_t starIdx = imagStr.find('*');
                imagStr = imagStr.substr(starIdx+1);
                xacc::trim(imagStr);

                std::complex<double> amp(std::stod(realStr), std::stod(imagStr));
                amplitudes.push_back(amp);
            } else if (readingSim) {
                // If nothing was measured, ignore simulation results
                if (measuredBits.empty()) {
                    xacc::warning("Ignoring simulation result as there are no measurements in the circuit");
                } else {
                    std::size_t colonIdx = line.find(':');

                    if (colonIdx == std::string::npos || colonIdx == 0 || colonIdx == line.size()) {
                        xacc::error("corrupted results file: missing colon for sim results");
                    }

                    std::string physBits = line.substr(0, colonIdx);
                    unsigned long count = std::stoul(line.substr(colonIdx + 1));

                    std::string logicalBits(measuredBits.size(), '0');
                    for (auto pair : measuredBits) {
                        std::size_t physBitIdx, logicalBitIdx;
                        std::tie(physBitIdx, logicalBitIdx) = pair;

                        if (physBitIdx >= physBits.size()) {
                            xacc::error("Attempt to measure qubit larger than number of physical qubits");
                        }
                        if (logicalBitIdx >= logicalBits.size()) {
                            xacc::error("Attempt to measure logical qubit larger than number of logical qubits");
                        }

                        logicalBits[logicalBits.size() - 1 - logicalBitIdx] = physBits[physBits.size() - 1 - physBitIdx];
                    }

                    counts[logicalBits] += (int)count;
                }
            } else {
                static const std::string simLine("Simulation:");
                readingSim = line.substr(0, simLine.size()) == simLine;
                static const std::string stateLine("State:");
                readingState = line.substr(0, stateLine.size()) == stateLine;
            }
        }

        resultFile.close();

        buffer->setMeasurements(counts);

        // It's okay if the state is missing, as there's no way the
        // actual system could return that
        if (!amplitudes.empty()) {
            buffer->addExtraInfo("exp-val-z", calcExpZ(amplitudes, measuredBits));
        }
    }

    std::string IonTrapAccelerator::randomChars() {
        char chars[16];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib('a', 'z');

        for (unsigned int i = 0; i < sizeof chars; i++) {
            chars[i] = distrib(gen);
        }

        return std::string(chars, sizeof chars);
    }

    std::vector<std::pair<int, int>> IonTrapAccelerator::getConnectivity() {
        return std::vector<std::pair<int, int>>(couplingMap.begin(), couplingMap.end());
    }

    void IonTrapAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                                  const std::shared_ptr<CompositeInstruction> compositeInstruction) {
        xacc::debug("compile IR to native gates...");

        if (!emitQasm) {
            if (logTransformsPath.empty()) {
                compileToNativeGates(compositeInstruction, nullptr);
            } else {
                compileToNativeGatesAndLogTransforms(compositeInstruction);
            }
        }

        // TODO: what if already exists? would be nice to have O_EXCL
        std::string ext = inputExt.empty()? (emitQasm? ".qasm" : ".seq") : inputExt;
        std::string inputBasename = "xacc-" + randomChars() + ext;
        std::string inputFilename = watchdir + "/" + inputBasename;
        std::string tmpInputFilename = inputFilename + ".tmp";
        std::string resultBasename = inputBasename + ".result";
        std::string resultFilename = watchdir + "/"  + resultBasename;

#ifdef IONTRAP_USE_INOTIFY
        xacc::debug("setting up inotify on directory `" + watchdir + "'");
        int inotifyFd = getInotifyFd(watchdir);
#endif

        xacc::debug("writing temp input file `" + tmpInputFilename + "'");

        std::multimap<std::size_t, std::size_t> measuredBits;
        writeInput(buffer, compositeInstruction, tmpInputFilename, measuredBits);

        if (xacc::verbose) {
            // Needed for benchmarking, since we compare measurement
            // outcomes and need to ignore bits that aren't measured
            std::stringstream ss;
            std::string sep = "";
            for (const auto &kv : measuredBits) {
                ss << sep << kv.second << ":" << kv.first;
                sep = ",";
            }
            xacc::debug("Measurement bits (logical to physical) ->" + ss.str());
        }

        xacc::debug("renaming temp input file `" + tmpInputFilename + "' to `" + inputFilename + "'");

        // Atomically move input file to end in .seq/.qasm to signal our Igor
        // code to pick it up and run it
        if (rename(tmpInputFilename.c_str(), inputFilename.c_str()) < 0) {
            int err = errno;
#ifdef IONTRAP_USE_INOTIFY
            close(inotifyFd);
#endif
            xacc::error(std::string("rename: ") + std::strerror(err));
            return;
        }

        xacc::debug("waiting on result file `" + resultFilename + "'");

#ifdef IONTRAP_USE_INOTIFY
        waitForResultFileInotify(inotifyFd, resultBasename);
#else
		waitForResultFile(resultFilename);
#endif
        readResult(std::move(buffer), resultFilename, measuredBits);

        if (keepTempFiles) {
            xacc::debug("preserving temp files `" + resultFilename + "' and `" + inputFilename + "' to obey save-temps flag");
        } else {
            xacc::debug("deleting temp files `" + resultFilename + "' and `" + inputFilename + "'");

            if (unlink(inputFilename.c_str()) < 0 || unlink(resultFilename.c_str()) < 0) {
                int err = errno;
                xacc::warning(std::string("unlink() temp file: ") + std::strerror(err));
            }
        }
    }

    void IonTrapAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions) {
        for (auto& f : compositeInstructions) {
            auto tmpBuffer = std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
            execute(tmpBuffer, f);
            buffer->appendChild(f->name(), tmpBuffer);
        }
    }
}}
