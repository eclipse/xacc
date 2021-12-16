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
 *   Austin Adams - initial implementation
 *******************************************************************************/
#include <cassert>
#include "IonTrapOptFunction.hpp"

namespace xacc {
namespace quantum {

//
// Gradients up to global phase (exact)
//

void IonTrapOptFunction::nablaHExact_1(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = (std::exp(-i*phi(0,0))*(-std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))))/std::sqrt(2.0);
}

void IonTrapOptFunction::nablaHExact_2(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/2.0*std::exp(-i*(phi(0,0)+phi(1,0)))*(-i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+std::exp(i*phi(1,0))*(-std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))+i*std::exp(i*phi(1,0))*std::conj(goal(1,1))));

    gh(1,0) = 1.0/2.0*std::exp(-i*(phi(0,0)+phi(1,0)))*(i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))-std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(2.0*i*phi(1,0))*(std::exp(i*phi(0,0))*std::conj(goal(1,0))-i*std::conj(goal(1,1))));
}

void IonTrapOptFunction::nablaHExact_3(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-i*phi(0,0))*((-1.0+std::exp(i*(phi(1,0)-phi(2,0))))*std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*(1.0-std::exp(-i*(phi(1,0)-phi(2,0))))*std::conj(goal(1,0))-i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))*(std::cos(phi(1,0))+std::cos(phi(2,0))-i*(std::sin(phi(1,0))+std::sin(phi(2,0))))+i*std::conj(goal(1,1))*(std::cos(phi(1,0))+std::cos(phi(2,0))+i*(std::sin(phi(1,0))+std::sin(phi(2,0)))));

    gh(1,0) = 1.0/std::sqrt(2.0)*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(2,0)))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))*(-std::conj(goal(0,1))+std::conj(goal(1,0))*(std::cos(phi(0,0)+phi(2,0))+i*std::sin(phi(0,0)+phi(2,0))))-(std::exp(i*phi(0,0))*std::conj(goal(0,0))+std::exp(i*phi(2,0))*std::conj(goal(1,1)))*std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0))));

    gh(2,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-i*(phi(0,0)+phi(1,0)+phi(2,0)))*(i*std::exp(i*(phi(0,0)+phi(1,0)))*(std::exp(i*phi(0,0))+std::exp(i*phi(1,0)))*std::conj(goal(0,0))+(std::exp(2.0*i*phi(1,0))-std::exp(i*(phi(0,0)+phi(1,0))))*std::conj(goal(0,1))-std::exp(2.0*i*phi(2,0))*((std::exp(2.0*i*phi(0,0))-std::exp(i*(phi(0,0)+phi(1,0))))*std::conj(goal(1,0))+i*(std::exp(i*phi(0,0))+std::exp(i*phi(1,0)))*std::conj(goal(1,1))));
}

void IonTrapOptFunction::nablaHExact_4(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/4.0*std::exp(-i*(phi(0,0)+phi(1,0)+phi(2,0)+phi(3,0)))*(i*std::exp(2.0*i*phi(0,0))*(std::exp(2.0*i*phi(2,0))-std::exp(i*(phi(1,0)+phi(2,0)))-std::exp(i*(phi(1,0)+phi(3,0)))-std::exp(i*(phi(2,0)+phi(3,0))))*std::conj(goal(0,0))+std::exp(i*phi(1,0))*(std::exp(2.0*i*phi(2,0))+std::exp(i*(phi(1,0)+phi(2,0)))+std::exp(i*(phi(1,0)+phi(3,0)))-std::exp(i*(phi(2,0)+phi(3,0))))*std::conj(goal(0,1))+std::exp(i*phi(3,0))*(-std::exp(2.0*i*phi(0,0))*(std::exp(2.0*i*phi(2,0))-std::exp(i*(phi(1,0)+phi(2,0)))+std::exp(i*(phi(1,0)+phi(3,0)))+std::exp(i*(phi(2,0)+phi(3,0))))*std::conj(goal(1,0))+i*std::exp(i*phi(1,0))*(std::exp(2.0*i*phi(2,0))+std::exp(i*(phi(1,0)+phi(2,0)))-std::exp(i*(phi(1,0)+phi(3,0)))+std::exp(i*(phi(2,0)+phi(3,0))))*std::conj(goal(1,1))));

    gh(1,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(3,0)))*(-std::conj(goal(0,1))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(3,0)))-i*std::sin(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0))+std::exp(i*(phi(0,0)+phi(3,0)))*std::conj(goal(1,0))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(3,0)))+i*std::sin(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0))+i*std::exp(i*phi(3,0))*std::conj(goal(1,1))*(std::cos(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0)+i*std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(3,0))))-std::exp(i*phi(0,0))*std::conj(goal(0,0))*(i*std::cos(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0)+std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(3,0)))));

    gh(2,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(3,0)))*(std::exp(i*(phi(0,0)+phi(3,0)))*std::conj(goal(1,0))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(2,0)+phi(3,0)))-i*std::sin(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0))-std::conj(goal(0,1))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(2,0)+phi(3,0)))+i*std::sin(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0))+std::conj(goal(1,1))*(-i*std::cos(phi(3,0))+std::sin(phi(3,0)))*(std::cos(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0)-i*std::sin(1.0/2.0*(phi(0,0)-2.0*phi(2,0)+phi(3,0))))+i*std::exp(i*phi(0,0))*std::conj(goal(0,0))*(std::cos(phi(0,0)/2.0-phi(1,0)+phi(2,0)-phi(3,0)/2.0)+i*std::sin(1.0/2.0*(phi(0,0)-2.0*phi(2,0)+phi(3,0)))));

    gh(3,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(2,0)+2.0*phi(3,0)))*(std::exp(2.0*i*phi(3,0))*(-std::exp(i*phi(0,0))*std::conj(goal(1,0))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))+i*std::sin((phi(0,0)-phi(2,0))/2.0))+std::conj(goal(1,1))*(-i*std::cos((phi(0,0)-phi(2,0))/2.0)+std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))))+std::exp(i*phi(2,0))*(std::conj(goal(0,1))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))-i*std::sin((phi(0,0)-phi(2,0))/2.0))+std::exp(i*phi(0,0))*std::conj(goal(0,0))*(i*std::cos((phi(0,0)-phi(2,0))/2.0)+std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0))))));
}

//
// Gradients up to Rx
//

void IonTrapOptFunction::nablaHRx_1(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = -(1.0/2.0)*i*(std::conj(goal(0,1))+std::conj(goal(1,0)))*std::cos(phi(0,0)/2.0)-1.0/2.0*(std::conj(goal(0,0))+std::conj(goal(1,1)))*std::sin(phi(0,0)/2.0);
}

void IonTrapOptFunction::nablaHRx_2(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/std::sqrt(2.0)*std::exp(-i*phi(0,0))*(-((std::conj(goal(0,1))-std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0)))*std::cos(phi(1,0)/2.0))+i*(-std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+std::conj(goal(1,1)))*std::sin(phi(1,0)/2.0));

    gh(1,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-i*phi(0,0))*(-((std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+i*std::exp(i*phi(0,0))*(std::conj(goal(0,1))+std::conj(goal(1,0)))+std::conj(goal(1,1)))*std::cos(phi(1,0)/2.0))+i*(std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))+i*std::exp(i*phi(0,0))*(std::conj(goal(0,0))+std::conj(goal(1,1))))*std::sin(phi(1,0)/2.0));
}

void IonTrapOptFunction::nablaHRx_3(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/2.0*std::exp(-i*(phi(0,0)+phi(1,0)))*((-i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+std::exp(i*phi(1,0))*(-std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))+i*std::exp(i*phi(1,0))*std::conj(goal(1,1))))*std::cos(phi(2,0)/2.0)+(std::exp(2.0*i*phi(1,0))*std::conj(goal(0,1))-std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))-i*std::exp(i*phi(1,0))*(std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))-std::conj(goal(1,1))))*std::sin(phi(2,0)/2.0));

    gh(1,0) = 1.0/2.0*std::exp(-i*(phi(0,0)+phi(1,0)))*((i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))-std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(2.0*i*phi(1,0))*(std::exp(i*phi(0,0))*std::conj(goal(1,0))-i*std::conj(goal(1,1))))*std::cos(phi(2,0)/2.0)+(-i*std::exp(i*(phi(0,0)+2.0*phi(1,0)))*std::conj(goal(0,0))-std::exp(2.0*i*phi(1,0))*std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))+i*std::exp(i*phi(0,0))*std::conj(goal(1,1)))*std::sin(phi(2,0)/2.0));

    gh(2,0) = 1.0/4.0*std::exp(-i*(phi(0,0)+phi(1,0)))*(-std::cos(phi(2,0)/2.0)*(std::exp(2.0*i*phi(1,0))*(std::exp(i*phi(0,0))*std::conj(goal(0,0))-i*std::conj(goal(0,1)))+std::exp(i*phi(1,0))*(std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+i*std::exp(i*phi(0,0))*(std::conj(goal(0,1))+std::conj(goal(1,0)))+std::conj(goal(1,1)))+std::exp(i*phi(0,0))*(std::conj(goal(1,1))+std::conj(goal(1,0))*(-i*std::cos(phi(0,0))+std::sin(phi(0,0)))))+(std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+i*std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(2.0*i*phi(1,0))*(i*std::exp(i*phi(0,0))*std::conj(goal(1,0))+std::conj(goal(1,1)))+i*std::exp(i*phi(1,0))*(std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))+i*std::exp(i*phi(0,0))*(std::conj(goal(0,0))+std::conj(goal(1,1)))))*std::sin(phi(2,0)/2.0));
}

void IonTrapOptFunction::nablaHRx_4(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-i*(phi(0,0)+phi(1,0)+phi(2,0)))*(i*std::exp(i*(2.0*phi(0,0)+phi(2,0)))*(-std::cos(phi(3,0)/2.0)*(std::conj(goal(0,0))+std::conj(goal(1,0))*(-i*std::cos(phi(2,0))+std::sin(phi(2,0))))+(std::exp(i*phi(2,0))*std::conj(goal(0,0))+i*std::conj(goal(1,0)))*std::sin(phi(3,0)/2.0))+std::exp(i*phi(1,0))*((-i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+std::exp(i*phi(2,0))*(-std::conj(goal(0,1))+std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))+i*std::exp(i*phi(2,0))*std::conj(goal(1,1))))*std::cos(phi(3,0)/2.0)+(std::exp(2.0*i*phi(2,0))*std::conj(goal(0,1))-std::exp(2.0*i*phi(0,0))*std::conj(goal(1,0))-i*std::exp(i*phi(2,0))*(std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))-std::conj(goal(1,1))))*std::sin(phi(3,0)/2.0))+std::exp(2.0*i*phi(1,0))*((std::conj(goal(0,1))+i*std::exp(i*phi(2,0))*std::conj(goal(1,1)))*std::cos(phi(3,0)/2.0)+(std::exp(i*phi(2,0))*std::conj(goal(0,1))-i*std::conj(goal(1,1)))*std::sin(phi(3,0)/2.0)));

    gh(1,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-i*(phi(0,0)+phi(1,0)+phi(2,0)))*(-((std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(2,0))))*std::conj(goal(0,1))*std::cos(phi(3,0)/2.0))+std::exp(2.0*i*phi(1,0))*(std::exp(i*phi(0,0))*std::conj(goal(1,0))-i*std::conj(goal(1,1)))*(std::exp(i*phi(2,0))*std::cos(phi(3,0)/2.0)-std::sin(phi(3,0)/2.0))+std::exp(i*phi(2,0))*(-std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(2,0))))*std::conj(goal(0,1))*std::sin(phi(3,0)/2.0)+std::exp(i*(phi(0,0)+phi(2,0)))*(std::exp(i*phi(0,0))*std::conj(goal(1,0))+i*std::conj(goal(1,1)))*(std::exp(i*phi(2,0))*std::cos(phi(3,0)/2.0)+std::sin(phi(3,0)/2.0))-i*std::exp(i*phi(0,0))*std::conj(goal(0,0))*((std::exp(2.0*i*phi(1,0))-std::exp(i*(phi(0,0)+phi(2,0))))*std::cos(phi(3,0)/2.0)+std::exp(i*phi(2,0))*(std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(2,0))))*std::sin(phi(3,0)/2.0)));

    gh(2,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-i*(phi(0,0)+phi(1,0)+phi(2,0)))*(-std::exp(2.0*i*phi(2,0))*((std::exp(2.0*i*phi(0,0))-std::exp(i*(phi(0,0)+phi(1,0))))*std::conj(goal(1,0))+i*(std::exp(i*phi(0,0))+std::exp(i*phi(1,0)))*std::conj(goal(1,1)))*std::cos(phi(3,0)/2.0)+std::exp(i*phi(1,0))*((std::exp(2.0*i*phi(0,0))+std::exp(i*(phi(0,0)+phi(1,0))))*std::conj(goal(1,0))+i*(std::exp(i*phi(0,0))-std::exp(i*phi(1,0)))*std::conj(goal(1,1)))*std::sin(phi(3,0)/2.0)+i*std::exp(i*phi(0,0))*std::conj(goal(0,0))*((std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(1,0))))*std::cos(phi(3,0)/2.0)+std::exp(2.0*i*phi(2,0))*(std::exp(i*phi(0,0))-std::exp(i*phi(1,0)))*std::sin(phi(3,0)/2.0))+std::conj(goal(0,1))*((std::exp(2.0*i*phi(1,0))-std::exp(i*(phi(0,0)+phi(1,0))))*std::cos(phi(3,0)/2.0)-std::exp(2.0*i*phi(2,0))*(std::exp(i*phi(0,0))+std::exp(i*phi(1,0)))*std::sin(phi(3,0)/2.0)));

    gh(3,0) = -(1.0/8.0)*i*(-std::sqrt(2.0)*std::cos(phi(3,0)/2.0)*((-1.0+std::exp(-i*(phi(0,0)+phi(1,0)))*(std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(2,0)))+std::exp(i*(phi(1,0)+phi(2,0)))))*std::conj(goal(0,1))+(-1.0+std::exp(i*(phi(0,0)-phi(1,0)))+std::exp(i*(phi(0,0)-phi(2,0)))+std::exp(i*(phi(1,0)-phi(2,0))))*std::conj(goal(1,0))+std::conj(goal(1,1))*(i*std::cos(phi(0,0))+i*std::cos(phi(1,0))+i*std::cos(phi(2,0))-i*std::cos(phi(0,0)-phi(1,0)+phi(2,0))+std::sin(phi(0,0))+std::sin(phi(1,0))+std::sin(phi(2,0))-std::sin(phi(0,0)-phi(1,0)+phi(2,0))))-std::sqrt(2.0)*((std::exp(-i*phi(0,0))+std::exp(-i*phi(1,0))+std::exp(-i*phi(2,0))-std::exp(-i*(phi(0,0)-phi(1,0)+phi(2,0))))*std::conj(goal(0,1))+(std::exp(i*phi(0,0))+std::exp(i*phi(1,0))+std::exp(i*phi(2,0))-std::exp(i*(phi(0,0)-phi(1,0)+phi(2,0))))*std::conj(goal(1,0))-i*(-1.0+std::exp(-i*(phi(0,0)+phi(1,0)))*(std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(2,0)))+std::exp(i*(phi(1,0)+phi(2,0)))))*std::conj(goal(1,1)))*std::sin(phi(3,0)/2.0)-i*std::sqrt(2.0)*std::exp(-i*(phi(1,0)+phi(2,0)))*std::conj(goal(0,0))*(std::exp(i*phi(2,0))*(std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(1,0)))-std::exp(i*(phi(0,0)+phi(2,0)))+std::exp(i*(phi(1,0)+phi(2,0))))*std::cos(phi(3,0)/2.0)-(std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(0,0)+phi(1,0)))+std::exp(i*(phi(0,0)+phi(2,0)))-std::exp(i*(phi(1,0)+phi(2,0))))*std::sin(phi(3,0)/2.0)));
}

//
// Gradients up to Rz
//

void IonTrapOptFunction::nablaHRz_1(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/2.0*i*std::exp(-((i*phi(0,0))/2.0))*(-std::conj(goal(0,0))+std::exp(i*phi(0,0))*std::conj(goal(1,1)));
}

void IonTrapOptFunction::nablaHRz_2(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = (std::exp(-(1.0/2.0)*i*(2.0*phi(0,0)+phi(1,0)))*(-std::conj(goal(0,1))+std::exp(i*(2.0*phi(0,0)+phi(1,0)))*std::conj(goal(1,0))))/std::sqrt(2.0);

    gh(1,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-(1.0/2.0)*i*(2.0*phi(0,0)+phi(1,0)))*(-std::conj(goal(0,1))+std::exp(i*(phi(0,0)+phi(1,0)))*(std::exp(i*phi(0,0))*std::conj(goal(1,0))+i*std::conj(goal(1,1)))+std::conj(goal(0,0))*(-i*std::cos(phi(0,0))+std::sin(phi(0,0))));
}

void IonTrapOptFunction::nablaHRz_3(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(2.0*(phi(0,0)+phi(1,0))+phi(2,0)))*(-i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))+std::exp(i*phi(1,0))*(-std::conj(goal(0,1))+std::exp(i*(2.0*phi(0,0)+phi(2,0)))*std::conj(goal(1,0))+i*std::exp(i*(phi(1,0)+phi(2,0)))*std::conj(goal(1,1))));

    gh(1,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(2.0*(phi(0,0)+phi(1,0))+phi(2,0)))*(i*std::exp(2.0*i*phi(0,0))*std::conj(goal(0,0))-std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(i*(2.0*phi(1,0)+phi(2,0)))*(std::exp(i*phi(0,0))*std::conj(goal(1,0))-i*std::conj(goal(1,1))));

    gh(2,0) = 1.0/4.0*std::exp(-(1.0/2.0)*i*(2.0*(phi(0,0)+phi(1,0))+phi(2,0)))*(i*std::exp(i*phi(0,0))*(std::exp(i*phi(0,0))-std::exp(i*phi(1,0)))*std::conj(goal(0,0))-(std::exp(i*phi(0,0))+std::exp(i*phi(1,0)))*std::conj(goal(0,1))+std::exp(i*(phi(1,0)+phi(2,0)))*((std::exp(2.0*i*phi(0,0))+std::exp(i*(phi(0,0)+phi(1,0))))*std::conj(goal(1,0))+i*(std::exp(i*phi(0,0))-std::exp(i*phi(1,0)))*std::conj(goal(1,1))));
}

void IonTrapOptFunction::nablaHRz_4(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-(1.0/2.0)*i*(2.0*(phi(0,0)+phi(1,0)+phi(2,0))+phi(3,0)))*(-i*std::exp(2.0*i*phi(0,0))*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::conj(goal(0,0))+(std::exp(2.0*i*phi(1,0))-std::exp(i*(phi(1,0)+phi(2,0))))*std::conj(goal(0,1))+std::exp(i*(phi(2,0)+phi(3,0)))*(std::exp(2.0*i*phi(0,0))*(std::exp(i*phi(1,0))-std::exp(i*phi(2,0)))*std::conj(goal(1,0))+i*std::exp(i*phi(1,0))*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::conj(goal(1,1))));

    gh(1,0) = 1.0/std::sqrt(2.0)*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(2,0)+phi(3,0)))*(-std::conj(goal(0,1))*std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))+std::exp(i*(phi(0,0)+phi(2,0)+phi(3,0)))*std::conj(goal(1,0))*std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))-(std::exp(i*phi(0,0))*std::conj(goal(0,0))+std::exp(i*(phi(2,0)+phi(3,0)))*std::conj(goal(1,1)))*std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0))));

    gh(2,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-(1.0/2.0)*i*(2.0*(phi(0,0)+phi(1,0)+phi(2,0))+phi(3,0)))*(i*std::exp(i*(phi(0,0)+phi(1,0)))*(std::exp(i*phi(0,0))+std::exp(i*phi(1,0)))*std::conj(goal(0,0))+(std::exp(2.0*i*phi(1,0))-std::exp(i*(phi(0,0)+phi(1,0))))*std::conj(goal(0,1))-std::exp(i*(2.0*phi(2,0)+phi(3,0)))*((std::exp(2.0*i*phi(0,0))-std::exp(i*(phi(0,0)+phi(1,0))))*std::conj(goal(1,0))+i*(std::exp(i*phi(0,0))+std::exp(i*phi(1,0)))*std::conj(goal(1,1))));

    gh(3,0) = 1.0/(2.0*std::sqrt(2.0))*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(2,0)+phi(3,0)))*(-i*std::exp(i*(phi(2,0)+phi(3,0)))*std::conj(goal(1,1))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))-i*std::sin((phi(0,0)-phi(2,0))/2.0))+i*std::exp(i*phi(0,0))*std::conj(goal(0,0))*(std::cos(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))+i*std::sin((phi(0,0)-phi(2,0))/2.0))+std::exp(i*(phi(0,0)+phi(2,0)+phi(3,0)))*std::conj(goal(1,0))*(std::cos((phi(0,0)-phi(2,0))/2.0)-i*std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0))))-std::conj(goal(0,1))*(std::cos((phi(0,0)-phi(2,0))/2.0)+i*std::sin(1.0/2.0*(phi(0,0)-2.0*phi(1,0)+phi(2,0)))));
}

//
// Gradients from Rz up to exact
//

void IonTrapOptFunction::nablaHRzExact_1(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/2.0*i*std::exp(-((i*phi(0,0))/2.0))*(-std::conj(goal(0,0))+std::exp(i*phi(0,0))*std::conj(goal(1,1)));
}

void IonTrapOptFunction::nablaHRzExact_2(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = (std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)))*(std::exp(i*phi(0,0))*std::conj(goal(0,1))-std::exp(2.0*i*phi(1,0))*std::conj(goal(1,0))-i*std::exp(i*phi(1,0))*(std::conj(goal(0,0))-std::exp(i*phi(0,0))*std::conj(goal(1,1)))))/(2.0*std::sqrt(2.0));

    gh(1,0) = (std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)))*(-std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(2.0*i*phi(1,0))*std::conj(goal(1,0))))/std::sqrt(2.0);
}

void IonTrapOptFunction::nablaHRzExact_3(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/4.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(i*std::exp(i*phi(1,0))*(std::exp(i*phi(1,0))-std::exp(i*phi(2,0)))*std::conj(goal(0,0))+std::exp(i*phi(0,0))*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::conj(goal(0,1))+std::exp(i*phi(2,0))*(-((std::exp(2.0*i*phi(1,0))+std::exp(i*(phi(1,0)+phi(2,0))))*std::conj(goal(1,0)))+i*std::exp(i*phi(0,0))*(std::exp(i*phi(1,0))-std::exp(i*phi(2,0)))*std::conj(goal(1,1))));

    gh(1,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(-i*std::exp(2.0*i*phi(1,0))*std::conj(goal(0,0))-std::exp(i*(phi(0,0)+phi(2,0)))*std::conj(goal(0,1))+std::exp(i*(2.0*phi(1,0)+phi(2,0)))*std::conj(goal(1,0))+i*std::exp(i*(phi(0,0)+2.0*phi(2,0)))*std::conj(goal(1,1)));

    gh(2,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(i*std::exp(2.0*i*phi(1,0))*std::conj(goal(0,0))-std::exp(i*(phi(0,0)+phi(1,0)))*std::conj(goal(0,1))+std::exp(2.0*i*phi(2,0))*(std::exp(i*phi(1,0))*std::conj(goal(1,0))+std::conj(goal(1,1))*(-i*std::cos(phi(0,0))+std::sin(phi(0,0)))));
}

void IonTrapOptFunction::nablaHRzExact_4(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = (1.0/(2.0*std::sqrt(2.0)))*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(1,0)+phi(3,0)))*(i*std::exp(i*phi(1,0))*std::conj(goal(0,0))*(std::cos(1.0/2.0*(phi(1,0)-2.0*phi(2,0)+phi(3,0)))+i*std::sin((phi(1,0)-phi(3,0))/2.0))+std::conj(goal(1,1))*(std::cos(1.0/2.0*(phi(1,0)-2.0*phi(2,0)+phi(3,0)))-i*std::sin((phi(1,0)-phi(3,0))/2.0))*(-i*std::cos(phi(0,0)+phi(3,0))+std::sin(phi(0,0)+phi(3,0)))-std::conj(goal(1,0))*(std::cos(phi(1,0)+phi(3,0))+i*std::sin(phi(1,0)+phi(3,0)))*(std::cos((phi(1,0)-phi(3,0))/2.0)-i*std::sin(1.0/2.0*(phi(1,0)-2.0*phi(2,0)+phi(3,0))))+std::exp(i*phi(0,0))*std::conj(goal(0,1))*(std::cos((phi(1,0)-phi(3,0))/2.0)+i*std::sin(1.0/2.0*(phi(1,0)-2.0*phi(2,0)+phi(3,0)))));

    gh(1,0) = (1.0/(2.0*std::sqrt(2.0)))*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0)+phi(3,0))))*(std::exp(i*(phi(0,0)+phi(2,0)))*(std::exp(i*phi(2,0))-std::exp(i*phi(3,0)))*std::conj(goal(0,1))-std::exp(i*(2.0*phi(1,0)+phi(3,0)))*(-std::exp(i*phi(2,0))+std::exp(i*phi(3,0)))*std::conj(goal(1,0))+i*(std::exp(i*phi(2,0))+std::exp(i*phi(3,0)))*(-std::exp(2.0*i*phi(1,0))*std::conj(goal(0,0))+std::exp(i*(phi(0,0)+phi(2,0)+phi(3,0)))*std::conj(goal(1,1))));

    gh(2,0) = (1.0/std::sqrt(2.0))*std::exp(-(1.0/2.0)*i*(phi(0,0)+phi(1,0)+phi(3,0)))*(-std::exp(i*phi(0,0))*std::conj(goal(0,1))*std::cos(1.0/2.0*(phi(1,0)-2.0*phi(2,0)+phi(3,0)))+std::conj(goal(1,0))*std::cos(1.0/2.0*(phi(1,0)-2.0*phi(2,0)+phi(3,0)))*(std::cos(phi(1,0)+phi(3,0))+i*std::sin(phi(1,0)+phi(3,0)))-(std::exp(i*phi(1,0))*std::conj(goal(0,0))+std::conj(goal(1,1))*(std::cos(phi(0,0)+phi(3,0))+i*std::sin(phi(0,0)+phi(3,0))))*std::sin(1.0/2.0*(phi(1,0)-2.0*phi(2,0)+phi(3,0))));

    gh(3,0) = (1.0/(2.0*std::sqrt(2.0)))*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0)+phi(3,0))))*(i*std::exp(i*(phi(1,0)+phi(2,0)))*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::conj(goal(0,0))+std::exp(i*(phi(0,0)+phi(2,0)))*(-std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::conj(goal(0,1))-std::exp(2.0*i*phi(3,0))*((std::exp(2.0*i*phi(1,0))-std::exp(i*(phi(1,0)+phi(2,0))))*std::conj(goal(1,0))+i*std::exp(i*phi(0,0))*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::conj(goal(1,1))));
}

//
// Gradients from Rz up to Rz
//

void IonTrapOptFunction::nablaHRzRz_1(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/2.0*i*std::exp(-((i*phi(0,0))/2.0))*(-std::conj(goal(0,0))+std::exp(i*phi(0,0))*std::conj(goal(1,1)));
}

void IonTrapOptFunction::nablaHRzRz_3(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = (std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)+phi(2,0)))*(std::exp(i*phi(0,0))*std::conj(goal(0,1))-std::exp(i*phi(1,0))*(i*std::conj(goal(0,0))+std::exp(i*(phi(1,0)+phi(2,0)))*std::conj(goal(1,0))-i*std::exp(i*(phi(0,0)+phi(2,0)))*std::conj(goal(1,1)))))/(2.0*std::sqrt(2.0));

    gh(1,0) = (std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)+phi(2,0)))*(-std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(i*(2.0*phi(1,0)+phi(2,0)))*std::conj(goal(1,0))))/std::sqrt(2.0);

    gh(2,0) = (std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)+phi(2,0)))*(-std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(i*phi(1,0))*(-i*std::conj(goal(0,0))+std::exp(i*(phi(1,0)+phi(2,0)))*std::conj(goal(1,0))+i*std::exp(i*(phi(0,0)+phi(2,0)))*std::conj(goal(1,1)))))/(2.0*std::sqrt(2.0));
}

void IonTrapOptFunction::nablaHRzRz_4(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/4.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(-std::exp(i*phi(1,0))*std::conj(goal(1,0))*((std::exp(2.0*i*phi(2,0))+std::exp(i*(phi(1,0)+phi(2,0))))*std::cos(phi(3,0)/2.0)-(std::exp(i*phi(1,0))-std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0))+std::exp(i*phi(0,0))*std::conj(goal(0,1))*((std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::cos(phi(3,0)/2.0)-std::exp(i*phi(2,0))*(-std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0))-i*std::exp(i*phi(0,0))*std::conj(goal(1,1))*((std::exp(2.0*i*phi(2,0))-std::exp(i*(phi(1,0)+phi(2,0))))*std::cos(phi(3,0)/2.0)+(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0))+i*std::exp(i*phi(1,0))*std::conj(goal(0,0))*((std::exp(i*phi(1,0))-std::exp(i*phi(2,0)))*std::cos(phi(3,0)/2.0)+std::exp(i*phi(2,0))*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0)));

    gh(1,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(std::exp(2.0*i*phi(1,0))*((-i*std::conj(goal(0,0))+std::exp(i*phi(2,0))*std::conj(goal(1,0)))*std::cos(phi(3,0)/2.0)-(i*std::exp(i*phi(2,0))*std::conj(goal(0,0))+std::conj(goal(1,0)))*std::sin(phi(3,0)/2.0))+std::exp(i*(phi(0,0)+phi(2,0)))*(-std::cos(phi(3,0)/2.0)*(std::conj(goal(0,1))+std::conj(goal(1,1))*(-i*std::cos(phi(2,0))+std::sin(phi(2,0))))+(std::exp(i*phi(2,0))*std::conj(goal(0,1))+i*std::conj(goal(1,1)))*std::sin(phi(3,0)/2.0)));

    gh(2,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(std::cos(phi(3,0)/2.0)*(i*std::exp(2.0*i*phi(1,0))*std::conj(goal(0,0))-std::exp(i*(phi(0,0)+phi(1,0)))*std::conj(goal(0,1))+std::exp(2.0*i*phi(2,0))*(std::exp(i*phi(1,0))*std::conj(goal(1,0))+std::conj(goal(1,1))*(-i*std::cos(phi(0,0))+std::sin(phi(0,0)))))+(-i*std::exp(i*(phi(1,0)+2.0*phi(2,0)))*std::conj(goal(0,0))-std::exp(i*(phi(0,0)+2.0*phi(2,0)))*std::conj(goal(0,1))+std::exp(2.0*i*phi(1,0))*std::conj(goal(1,0))+i*std::exp(i*(phi(0,0)+phi(1,0)))*std::conj(goal(1,1)))*std::sin(phi(3,0)/2.0));

    gh(3,0) = 1.0/2.0*std::exp(-((i*phi(0,0))/2.0))*(-std::exp(i*phi(0,0))*std::conj(goal(1,1))*(1.0/2.0*(std::exp(-i*phi(1,0))+std::exp(-i*phi(2,0)))*std::cos(phi(3,0)/2.0)-1.0/2.0*(-1.0+std::exp(-i*(phi(1,0)-phi(2,0))))*std::sin(phi(3,0)/2.0))+std::conj(goal(0,0))*(-(1.0/2.0)*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::cos(phi(3,0)/2.0)+1.0/2.0*(-1.0+std::exp(i*(phi(1,0)-phi(2,0))))*std::sin(phi(3,0)/2.0))-std::exp(i*phi(0,0))*std::conj(goal(0,1))*(1.0/2.0*i*(1.0-std::exp(-i*(phi(1,0)-phi(2,0))))*std::cos(phi(3,0)/2.0)-1.0/2.0*(i*std::cos(phi(1,0))+i*std::cos(phi(2,0))+std::sin(phi(1,0))+std::sin(phi(2,0)))*std::sin(phi(3,0)/2.0))+1.0/2.0*i*std::conj(goal(1,0))*((-1.0+std::exp(i*(phi(1,0)-phi(2,0))))*std::cos(phi(3,0)/2.0)+(std::cos(phi(1,0))+std::cos(phi(2,0))+i*(std::sin(phi(1,0))+std::sin(phi(2,0))))*std::sin(phi(3,0)/2.0)));
}

//
// Gradients from Rz up to Rx
//

void IonTrapOptFunction::nablaHRzRx_2(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/2.0*std::exp(-((i*phi(0,0))/2.0))*(-i*(std::conj(goal(0,0))-std::exp(i*phi(0,0))*std::conj(goal(1,1)))*std::cos(phi(1,0)/2.0)+(std::exp(i*phi(0,0))*std::conj(goal(0,1))-std::conj(goal(1,0)))*std::sin(phi(1,0)/2.0));

    gh(1,0) = 1.0/2.0*std::exp(-((i*phi(0,0))/2.0))*(-i*(std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::conj(goal(1,0)))*std::cos(phi(1,0)/2.0)-(std::conj(goal(0,0))+std::exp(i*phi(0,0))*std::conj(goal(1,1)))*std::sin(phi(1,0)/2.0));
}

void IonTrapOptFunction::nablaHRzRx_3(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = (1.0/(2.0*std::sqrt(2.0)))*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)))*(-((i*std::exp(i*phi(1,0))*std::conj(goal(0,0))-std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::exp(2.0*i*phi(1,0))*std::conj(goal(1,0))-i*std::exp(i*(phi(0,0)+phi(1,0)))*std::conj(goal(1,1)))*std::cos(phi(2,0)/2.0))+(i*std::exp(2.0*i*phi(1,0))*std::conj(goal(0,0))+std::exp(i*(phi(0,0)+phi(1,0)))*std::conj(goal(0,1))-std::exp(i*phi(1,0))*std::conj(goal(1,0))+std::conj(goal(1,1))*(-i*std::cos(phi(0,0))+std::sin(phi(0,0))))*std::sin(phi(2,0)/2.0));

    gh(1,0) = (std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)))*(std::exp(2.0*i*phi(1,0))*(std::conj(goal(1,0))*std::cos(phi(2,0)/2.0)-i*std::conj(goal(0,0))*std::sin(phi(2,0)/2.0))+std::exp(i*phi(0,0))*(-std::conj(goal(0,1))*std::cos(phi(2,0)/2.0)+i*std::conj(goal(1,1))*std::sin(phi(2,0)/2.0))))/std::sqrt(2.0);

    gh(2,0) = (1.0/std::sqrt(2.0))*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*phi(1,0)))*(-(1.0/2.0)*(std::exp(2.0*i*phi(1,0))*std::conj(goal(0,0))+i*std::exp(i*phi(1,0))*(std::exp(i*phi(0,0))*std::conj(goal(0,1))+std::conj(goal(1,0)))+std::exp(i*phi(0,0))*std::conj(goal(1,1)))*std::cos(phi(2,0)/2.0)-1.0/2.0*(std::conj(goal(0,1))*(-i*std::cos(phi(0,0))+std::sin(phi(0,0)))+std::exp(i*phi(1,0))*(std::conj(goal(0,0))+std::exp(i*phi(0,0))*std::conj(goal(1,1))+std::conj(goal(1,0))*(-i*std::cos(phi(1,0))+std::sin(phi(1,0)))))*std::sin(phi(2,0)/2.0));
}

void IonTrapOptFunction::nablaHRzRx_4(const arma::mat &phi, arma::cx_mat &gh) {
    const auto i = std::complex<double>(0, 1);

    gh(0,0) = 1.0/4.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(-std::exp(i*phi(1,0))*std::conj(goal(1,0))*((std::exp(2.0*i*phi(2,0))+std::exp(i*(phi(1,0)+phi(2,0))))*std::cos(phi(3,0)/2.0)-(std::exp(i*phi(1,0))-std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0))+std::exp(i*phi(0,0))*std::conj(goal(0,1))*((std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::cos(phi(3,0)/2.0)-std::exp(i*phi(2,0))*(-std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0))-i*std::exp(i*phi(0,0))*std::conj(goal(1,1))*((std::exp(2.0*i*phi(2,0))-std::exp(i*(phi(1,0)+phi(2,0))))*std::cos(phi(3,0)/2.0)+(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0))+i*std::exp(i*phi(1,0))*std::conj(goal(0,0))*((std::exp(i*phi(1,0))-std::exp(i*phi(2,0)))*std::cos(phi(3,0)/2.0)+std::exp(i*phi(2,0))*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::sin(phi(3,0)/2.0)));

    gh(1,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(std::exp(2.0*i*phi(1,0))*((-i*std::conj(goal(0,0))+std::exp(i*phi(2,0))*std::conj(goal(1,0)))*std::cos(phi(3,0)/2.0)-(i*std::exp(i*phi(2,0))*std::conj(goal(0,0))+std::conj(goal(1,0)))*std::sin(phi(3,0)/2.0))+std::exp(i*(phi(0,0)+phi(2,0)))*(-std::cos(phi(3,0)/2.0)*(std::conj(goal(0,1))+std::conj(goal(1,1))*(-i*std::cos(phi(2,0))+std::sin(phi(2,0))))+(std::exp(i*phi(2,0))*std::conj(goal(0,1))+i*std::conj(goal(1,1)))*std::sin(phi(3,0)/2.0)));

    gh(2,0) = 1.0/2.0*std::exp(-(1.0/2.0)*i*(phi(0,0)+2.0*(phi(1,0)+phi(2,0))))*(std::cos(phi(3,0)/2.0)*(i*std::exp(2.0*i*phi(1,0))*std::conj(goal(0,0))-std::exp(i*(phi(0,0)+phi(1,0)))*std::conj(goal(0,1))+std::exp(2.0*i*phi(2,0))*(std::exp(i*phi(1,0))*std::conj(goal(1,0))+std::conj(goal(1,1))*(-i*std::cos(phi(0,0))+std::sin(phi(0,0)))))+(-i*std::exp(i*(phi(1,0)+2.0*phi(2,0)))*std::conj(goal(0,0))-std::exp(i*(phi(0,0)+2.0*phi(2,0)))*std::conj(goal(0,1))+std::exp(2.0*i*phi(1,0))*std::conj(goal(1,0))+i*std::exp(i*(phi(0,0)+phi(1,0)))*std::conj(goal(1,1)))*std::sin(phi(3,0)/2.0));

    gh(3,0) = 1.0/2.0*std::exp(-((i*phi(0,0))/2.0))*(-std::exp(i*phi(0,0))*std::conj(goal(1,1))*(1.0/2.0*(std::exp(-i*phi(1,0))+std::exp(-i*phi(2,0)))*std::cos(phi(3,0)/2.0)-1.0/2.0*(-1.0+std::exp(-i*(phi(1,0)-phi(2,0))))*std::sin(phi(3,0)/2.0))+std::conj(goal(0,0))*(-(1.0/2.0)*(std::exp(i*phi(1,0))+std::exp(i*phi(2,0)))*std::cos(phi(3,0)/2.0)+1.0/2.0*(-1.0+std::exp(i*(phi(1,0)-phi(2,0))))*std::sin(phi(3,0)/2.0))-std::exp(i*phi(0,0))*std::conj(goal(0,1))*(1.0/2.0*i*(1.0-std::exp(-i*(phi(1,0)-phi(2,0))))*std::cos(phi(3,0)/2.0)-1.0/2.0*(i*std::cos(phi(1,0))+i*std::cos(phi(2,0))+std::sin(phi(1,0))+std::sin(phi(2,0)))*std::sin(phi(3,0)/2.0))+1.0/2.0*i*std::conj(goal(1,0))*((-1.0+std::exp(i*(phi(1,0)-phi(2,0))))*std::cos(phi(3,0)/2.0)+(std::cos(phi(1,0))+std::cos(phi(2,0))+i*(std::sin(phi(1,0))+std::sin(phi(2,0))))*std::sin(phi(3,0)/2.0)));
}

std::complex<double> IonTrapOptFunction::H(const arma::mat &phi) {
    arma::cx_mat actual = arma::eye<arma::cx_mat>(2,2);

    for (std::size_t i = 0; i < phi.n_rows; i++) {
        double phi_ = phi(i, 0);
        arma::cx_mat rot;

        if (i < decompFromRotationCount(decomp)) {
            rot = decompFromMat(decomp, phi_);
        } else if (i + decompCount(decomp) < phi.n_rows) {
            rot = {{1, std::complex<double>(-std::sin(phi_), -std::cos(phi_))},
                   {std::complex<double>(std::sin(phi_), -std::cos(phi_)), 1}};
            rot /= sqrt(2);
        } else {
            rot = decompMat(decomp, phi_);
        }

        actual = rot * actual;
    }

    return arma::trace(goal.t() * actual);
}

double IonTrapOptFunction::EvaluateWithGradient(const arma::mat &phi, arma::mat &g) {
    static const NablaMethod nablaHExact[] = {&xacc::quantum::IonTrapOptFunction::nablaHExact_1,
                                              &xacc::quantum::IonTrapOptFunction::nablaHExact_2,
                                              &xacc::quantum::IonTrapOptFunction::nablaHExact_3,
                                              &xacc::quantum::IonTrapOptFunction::nablaHExact_4};
    static const NablaMethod nablaHRx[] = {&xacc::quantum::IonTrapOptFunction::nablaHRx_1,
                                           &xacc::quantum::IonTrapOptFunction::nablaHRx_2,
                                           &xacc::quantum::IonTrapOptFunction::nablaHRx_3,
                                           &xacc::quantum::IonTrapOptFunction::nablaHRx_4};
    static const NablaMethod nablaHRz[] = {&xacc::quantum::IonTrapOptFunction::nablaHRz_1,
                                           &xacc::quantum::IonTrapOptFunction::nablaHRz_2,
                                           &xacc::quantum::IonTrapOptFunction::nablaHRz_3,
                                           &xacc::quantum::IonTrapOptFunction::nablaHRz_4};
    static const NablaMethod nablaHRzExact[] = {&xacc::quantum::IonTrapOptFunction::nablaHRzExact_1,
                                                &xacc::quantum::IonTrapOptFunction::nablaHRzExact_2,
                                                &xacc::quantum::IonTrapOptFunction::nablaHRzExact_3,
                                                &xacc::quantum::IonTrapOptFunction::nablaHRzExact_4};
    static const NablaMethod nablaHRzRx[] = {nullptr, // Rx(phi2).Rz(phi1) needs two angles. One can be 0, that's fine
                                             &xacc::quantum::IonTrapOptFunction::nablaHRzRx_2,
                                             &xacc::quantum::IonTrapOptFunction::nablaHRzRx_3,
                                             &xacc::quantum::IonTrapOptFunction::nablaHRzRx_4};
    static const NablaMethod nablaHRzRz[] = {&xacc::quantum::IonTrapOptFunction::nablaHRzRz_1,
                                             nullptr, // Rz(phi2).Rz(phi1) = Rz(phi2 + phi1) is really just 1 angle
                                             &xacc::quantum::IonTrapOptFunction::nablaHRzRz_3,
                                             &xacc::quantum::IonTrapOptFunction::nablaHRzRz_4};
    static const NablaMethod *nablaArr[] = {nablaHExact, nablaHRx, nablaHRz, nablaHRzExact, nablaHRzRx, nablaHRzRz};

    assert(phi.n_rows-1 < sizeof nablaHExact / sizeof *nablaHExact);
    assert(phi.n_rows-1 < sizeof nablaHRx / sizeof *nablaHRx);
    assert(phi.n_rows-1 < sizeof nablaHRz / sizeof *nablaHRz);
    assert(phi.n_rows-1 < sizeof nablaHRzExact / sizeof *nablaHExact);
    assert(phi.n_rows-1 < sizeof nablaHRzRx / sizeof *nablaHRzRx);
    assert(phi.n_rows-1 < sizeof nablaHRzRz / sizeof *nablaHRzRz);
    assert((int)decomp < sizeof nablaArr / sizeof *nablaArr);

    auto nablaMethod = nablaArr[(int)decomp][phi.n_rows-1];
    assert(nablaMethod != nullptr);

    arma::cx_mat gh(phi.n_rows, 1);
    (this->*nablaMethod)(phi, gh);

    std::complex<double> h = H(phi);
    g = arma::real(-(std::conj(h)*gh + h*arma::conj(gh)));

    return 4 - std::norm(h);
}

} // namespace quantum
} // namespace xacc
