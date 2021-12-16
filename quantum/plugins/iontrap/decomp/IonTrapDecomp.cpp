/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Austin Adams - initial implementation
 *******************************************************************************/
#include <cassert>
#include "IonTrapDecomp.hpp"

namespace xacc {
namespace quantum {

std::string decompName(const Decomp decomp) {
    switch (decomp) {
        case Decomp::Exact:
            return "exact";
        case Decomp::RX:
            return "Rx";
        case Decomp::RZ:
            return "Rz";
        case Decomp::RZExact:
            return "RzExact";
        case Decomp::RZRX:
            return "RzRx";
        case Decomp::RZRZ:
            return "RzRz";
        default:
            assert(0);
            return "";
    }
}

Decomp decompNextToTry(const Decomp decomp) {
    switch (decomp) {
        case Decomp::RX:
        case Decomp::RZ:
        case Decomp::RZExact:
            return Decomp::Exact;
        case Decomp::RZRX:
            return Decomp::RX;
        case Decomp::RZRZ:
            return Decomp::RZ;
        default:
            // Exact should never call this function. If exact fails, we are
            // in bad shape and should handle it differently
            assert(0);
            return Decomp::Exact;
    }
}

bool decompShouldSkip(const Decomp decomp, const int rotations) {
    // Rx(phi2).Rz(phi1) needs two angles. One can be 0, that's fine.
    // Also, Rz(phi2).Rz(phi1) = Rz(phi2 + phi1) is really just 1 angle.
    return (decomp == Decomp::RZRX && rotations == 1)
           || (decomp == Decomp::RZRZ && rotations == 2);
}

std::size_t decompFromRotationCount(const Decomp decomp) {
    // The first 1 entry of the array of rotations is an Rz for
    // Decomp::RZ{Exact,RX,RZ}, but in the case of plain
    // old Decomp::{Exact,RX,RZ}, there are 0 such entries
    return decomp != Decomp::Exact && decomp != Decomp::RX && decomp != Decomp::RZ;
}

std::size_t decompCount(const Decomp decomp) {
    // The last 1 entry of the array of rotations is an Rx/Rz for
    // Decomp::RX/RZ, but in the case of Decomp::Exact,
    // there are 0 such entries
    return decomp != Decomp::Exact && decomp != Decomp::RZExact;
}

bool decompShouldTrack(const Decomp decomp) {
    // We only care about tracking Rx rotations (across XX gates)
    return decomp == Decomp::RX || decomp == Decomp::RZRX;
}

arma::cx_mat decompFromMat(const Decomp decomp, const double phi) {
    const auto i = std::complex<double>(0, 1);

    if (decomp == Decomp::Exact || decomp == Decomp::RX || decomp == Decomp::RZ) {
        return arma::eye<arma::cx_mat>(2,2);
    } else if (decomp == Decomp::RZExact || decomp == Decomp::RZRX || decomp == Decomp::RZRZ) {
        // Reuse RZ rotation code from decompMat()
        return decompMat(Decomp::RZ, phi);
    } else {
        assert(0);
        return {};
    }
}

arma::cx_mat decompMat(const Decomp decomp, const double phi) {
    const auto i = std::complex<double>(0, 1);

    if (decomp == Decomp::Exact || decomp == Decomp::RZExact) {
        return arma::eye<arma::cx_mat>(2,2);
    } else if (decomp == Decomp::RX || decomp == Decomp::RZRX) {
        arma::cx_mat rx({std::cos(phi/2.0), -i*std::sin(phi/2.0),
                         -i*std::sin(phi/2.0), std::cos(phi/2.0)});
        rx.reshape(2, 2);
        return rx;
    } else if (decomp == Decomp::RZ || decomp == Decomp::RZRZ) {
        arma::cx_mat rz({std::exp(-i*phi/2.0), 0,
                         0, std::exp(i*phi/2.0)});
        rz.reshape(2, 2);
        return rz;
    } else {
        assert(0);
        return {};
    }
}

} // namespace quantum
} // namespace xacc
