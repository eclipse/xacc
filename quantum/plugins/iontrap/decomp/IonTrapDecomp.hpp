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
 *   Alexander J. McCaskey - initial API and implementation
 *   Austin Adams - adapted for GTRI testbed
 *******************************************************************************/
#ifndef QUANTUM_ACCELERATORS_IONTRAPDECOMP_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPDECOMP_HPP_

#include <armadillo>

namespace xacc {
namespace quantum {

enum class Decomp : int { Exact, RX, RZ, RZExact, RZRX, RZRZ };

std::size_t decompFromRotationCount(const Decomp);
arma::cx_mat decompFromMat(const Decomp, const double);
arma::cx_mat decompMat(const Decomp, const double);
std::size_t decompCount(const Decomp);
Decomp decompNextToTry(const Decomp);
bool decompShouldSkip(const Decomp, const int);
std::string decompName(const Decomp);
bool decompShouldTrack(const Decomp);

} // namespace quantum
} // namespace xacc

#endif
