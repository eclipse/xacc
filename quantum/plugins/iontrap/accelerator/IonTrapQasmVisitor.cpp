/*******************************************************************************
 * Copyright (c) 2019-2020 UT-Battelle, LLC.
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

#include "IonTrapQasmVisitor.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {
    void IonTrapQasmVisitor::writeHeader() {
        for (int i = 0; i < nQubits; i++) {
            qasmFile << "\tqubit q" << i << "\r\n";
        }
        qasmFile << "\r\n";
    }

    void IonTrapQasmVisitor::visit(Hadamard &h) {
        qasmFile << "\tH q" << h.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(CNOT &cnot) {
        std::size_t leftIdx = std::min(cnot.bits()[0], cnot.bits()[1]);
        std::size_t rightIdx = std::max(cnot.bits()[0], cnot.bits()[1]);
        if (!couplingMap.count(std::make_pair(leftIdx, rightIdx))) {
            xacc::error("CNOT on non-adjacent qubits " + std::to_string(leftIdx)
                        + "," + std::to_string(rightIdx));
            return;
        }

        qasmFile << "\tCNOT q" << cnot.bits()[0] << ",q" << cnot.bits()[1] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(Rz &rz) {
        double theta = rz.getParameter(0).as<double>();
        qasmFile << "\tRZ(" << theta << ") q" << rz.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(Ry &ry) {
        double theta = ry.getParameter(0).as<double>();

        S s(ry.bits()[0]);
        Rx rx(ry.bits()[0], theta);
        Sdg sdg(ry.bits()[0]);

        // Ry(theta) = S.Rx(theta).Sdg
        visit(sdg);
        visit(rx);
        visit(s);
    }

    void IonTrapQasmVisitor::visit(Rx &rx) {
        double theta = rx.getParameter(0).as<double>();

        Hadamard h1(rx.bits()[0]);
        Rz rz(rx.bits()[0], theta);
        Hadamard h2(rx.bits()[0]);

        // RX(theta) = e^(-i*theta/2) H.RZ(theta).H
        visit(h1);
        visit(rz);
        visit(h2);
    }

    void IonTrapQasmVisitor::visit(X &x) {
        qasmFile << "\tX q" << x.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(Y &y) {
        qasmFile << "\tY q" << y.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(Z &z) {
        qasmFile << "\tZ q" << z.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(CRZ &crz) {
        xacc::warning("CRZ not implemented on IonTrap backend");
    }

    // From IonQ backend
    void IonTrapQasmVisitor::visit(CH &ch) {
        Hadamard h1(ch.bits()[1]);
        Sdg sdg(ch.bits()[1]);
        CNOT cn1(ch.bits());
        Hadamard h2(ch.bits()[1]);
        T t1(ch.bits()[1]);
        CNOT cn2(ch.bits());
        T t2(ch.bits()[1]);
        Hadamard h3(ch.bits()[1]);
        S s1(ch.bits()[1]);
        X x(ch.bits()[1]);
        S s2(ch.bits()[0]);
        visit(h1);
        visit(sdg);
        visit(cn1);
        visit(h2);
        visit(t1);
        visit(cn2);
        visit(t2);
        visit(h3);
        visit(s1);
        visit(x);
        visit(s2);
    }

    void IonTrapQasmVisitor::visit(S &s) {
        qasmFile << "\tS q" << s.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(Sdg &sdg) {
        qasmFile << "\tS! q" << sdg.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(T &t) {
        qasmFile << "\tT q" << t.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(Tdg &tdg) {
        qasmFile << "\tT! q" << tdg.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(CPhase &cphase) {
        // TODO: implement me
        xacc::warning("CPhase not implemented on IonTrap backend");
    }

    void IonTrapQasmVisitor::visit(Identity &i) {
        qasmFile << "\tI q" << i.bits()[0] << "\r\n";
    }

    void IonTrapQasmVisitor::visit(U &u) {
        double theta = u.getParameter(0).as<double>();
        double phi = u.getParameter(1).as<double>();
        double lambda = u.getParameter(2).as<double>();

        Rz rz1(u.bits()[0], phi);
        Rx rx1(u.bits()[0], -xacc::constants::pi/2);
        Rz rz2(u.bits()[0], theta);
        Rx rx2(u.bits()[0], xacc::constants::pi/2);
        Rz rz3(u.bits()[0], lambda);

        // Per https://qiskit.org/documentation/stubs/qiskit.circuit.library.U3Gate.html,
        // U(theta,pi,lambda) = Rz(phi).Rx(-pi/2).Rz(theta).Rx(pi/2).Rz(lambda)
        visit(rz3);
        visit(rx2);
        visit(rz2);
        visit(rx1);
        visit(rz1);
    }

    void IonTrapQasmVisitor::visit(iSwap &in_iSwapGate) {
        xacc::warning("iSWAP not implemented on IonTrap backend");
    }

    void IonTrapQasmVisitor::visit(fSim &in_fsimGate) {
        xacc::warning("fSim not implemented on IonTrap backend");
    }

    void IonTrapQasmVisitor::visit(IfStmt &ifStmt) {
        xacc::warning("if statement not implemented on IonTrap backend");
    }

    void IonTrapQasmVisitor::visit(Measure &meas) {
        // This should've been handled beforehand in IonTrapAccelerator
        xacc::error("Measure instruction reached IonTrapQasmVisitor!");
    }
}}
