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
#ifndef QUANTUM_ACCELERATORS_IONTRAPOPTFUNCTION_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPOPTFUNCTION_HPP_

#include <armadillo>
#include "IonTrapDecomp.hpp"

namespace xacc {
namespace quantum {

class IonTrapOptFunction {
public:
    IonTrapOptFunction(Decomp decomp, const arma::cx_mat &goalUnitary)
        : decomp(decomp), goal(goalUnitary) {}

    double Evaluate(const arma::mat &phi);
    double EvaluateWithGradient(const arma::mat &phi, arma::mat &g);

private:
    typedef void (IonTrapOptFunction::*NablaMethod)(const arma::mat &, arma::cx_mat &);

    const Decomp decomp;
    const arma::cx_mat &goal;

    std::complex<double> H(const arma::mat &phi);
    void nablaHExact_1(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRx_1(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRz_1(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzExact_1(const arma::mat &phi, arma::cx_mat &gh);
    // No nablaHRzRx_1() because Rx(phi2).Rz(phi1) needs two angles. One can be 0, that's fine
    void nablaHRzRz_1(const arma::mat &phi, arma::cx_mat &gh);

    void nablaHExact_2(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRx_2(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRz_2(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzExact_2(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzRx_2(const arma::mat &phi, arma::cx_mat &gh);
    // No nablaHRzRz_2() because Rz(phi2).Rz(phi1) = Rz(phi2 + phi1) is really just 1 angle

    void nablaHExact_3(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRx_3(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRz_3(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzExact_3(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzRx_3(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzRz_3(const arma::mat &phi, arma::cx_mat &gh);

    void nablaHExact_4(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRx_4(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRz_4(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzExact_4(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzRx_4(const arma::mat &phi, arma::cx_mat &gh);
    void nablaHRzRz_4(const arma::mat &phi, arma::cx_mat &gh);
};

} // namespace quantum
} // namespace xacc

#endif
