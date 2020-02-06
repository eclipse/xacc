/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander i,. McCaskey - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "PauliOperator.hpp"
#include <Eigen/Dense>
using namespace std::complex_literals;

using namespace xacc;

template <typename Derived>
double fidelity(const Eigen::MatrixBase<Derived> &chi_1,
                const Eigen::MatrixBase<Derived> &chi_2) {
  Eigen::JacobiSVD<Eigen::MatrixXcd> svd(chi_1, Eigen::ComputeThinU |
                                                    Eigen::ComputeThinV);
  Eigen::MatrixXcd sqrt_chi_1 = svd.matrixU() *
                                svd.singularValues().cwiseSqrt().asDiagonal() *
                                svd.matrixV().transpose();

  Eigen::JacobiSVD<Eigen::MatrixXcd> svd2(chi_2, Eigen::ComputeThinU |
                                                     Eigen::ComputeThinV);
  Eigen::MatrixXcd sqrt_chi_2 = svd2.matrixU() *
                                svd2.singularValues().cwiseSqrt().asDiagonal() *
                                svd2.matrixV().transpose();

  Eigen::MatrixXcd tmp = sqrt_chi_1 * sqrt_chi_2;
  Eigen::JacobiSVD<Eigen::MatrixXcd> svd3(tmp, Eigen::ComputeThinU |
                                                   Eigen::ComputeThinV);

  auto svd_sum = svd3.singularValues().sum();

  return svd_sum * svd_sum;
}

TEST(QPTTester, checkHadamard) {
  if (xacc::hasAccelerator("aer")) {
    auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 1024)});
    auto buffer = xacc::qalloc(1);

    auto compiler = xacc::getCompiler("xasm");

    auto ir = compiler->compile(R"(__qpu__ void f(qbit q) {
        H(q[0]);
    })",
                                nullptr);
    auto h = ir->getComposite("f");

    auto qpt = xacc::getService<Algorithm>("qpt");
    EXPECT_TRUE(qpt->initialize(
        {std::make_pair("circuit", h), std::make_pair("accelerator", acc)}));
    qpt->execute(buffer);

    Eigen::MatrixXcd chi_hadamard_theory(4, 4);
    chi_hadamard_theory << 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1;
    auto chi_exp_real_vec = (*buffer)["chi-real"].as<std::vector<double>>();
    auto chi_exp_imag_vec = (*buffer)["chi-imag"].as<std::vector<double>>();

    Eigen::MatrixXd tmpchireal =
        Eigen::Map<Eigen::MatrixXd>(chi_exp_real_vec.data(), 4, 4);
    Eigen::MatrixXd tmpchiimag =
        Eigen::Map<Eigen::MatrixXd>(chi_exp_imag_vec.data(), 4, 4);

    Eigen::MatrixXcd chi = Eigen::MatrixXcd::Zero(4, 4);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        chi(i, j) = std::complex<double>(tmpchireal(i, j), tmpchiimag(i, j));
      }
    }

    EXPECT_NEAR(1.0, fidelity(.5 * chi, .5 * chi_hadamard_theory), 1e-1);

    std::cout << "FID: " << fidelity(.5 * chi, .5 * chi_hadamard_theory)
              << "\n";
  }
}

TEST(QPTTester, checkCX) {
  if (xacc::hasAccelerator("aer")) {
    auto acc = xacc::getAccelerator("aer", {std::make_pair("shots", 1024)});
    auto buffer = xacc::qalloc(2);

    auto compiler = xacc::getCompiler("xasm");

    auto ir = compiler->compile(R"(__qpu__ void cx(qbit q) {
        CX(q[0],q[1]);
    })",
                                nullptr);
    auto h = ir->getComposite("cx");

    auto qpt = xacc::getService<Algorithm>("qpt");
    EXPECT_TRUE(qpt->initialize(
        {std::make_pair("circuit", h), std::make_pair("accelerator", acc)}));
    qpt->execute(buffer);

    Eigen::MatrixXcd true_cx_chi(16, 16);
    true_cx_chi << 1., 0., 0., 1., 1., 0., 0., -1., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0.,
        1., 1., 0., 0., -1., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0., 1., 1.,
        0., 0., -1., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., -1., 0., 0., -1., -1., 0., 0., 1., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0.;

    auto chi_exp_real_vec = (*buffer)["chi-real"].as<std::vector<double>>();
    auto chi_exp_imag_vec = (*buffer)["chi-imag"].as<std::vector<double>>();

    Eigen::MatrixXd tmpchireal =
        Eigen::Map<Eigen::MatrixXd>(chi_exp_real_vec.data(), 16, 16);
    Eigen::MatrixXd tmpchiimag =
        Eigen::Map<Eigen::MatrixXd>(chi_exp_imag_vec.data(), 16, 16);

    Eigen::MatrixXcd chi = Eigen::MatrixXcd::Zero(16, 16);
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        chi(i, j) = std::complex<double>(tmpchireal(i, j), tmpchiimag(i, j));
      }
    }

    std::cout << "Chi:\n" << chi << "\n\n";

    std::cout << fidelity(.25 * chi, .25 * true_cx_chi) << "\n";

    EXPECT_NEAR(1.0, fidelity(.25 * chi, .25 * true_cx_chi), 1e-1);
  }
}

TEST(QPTTester, checkBell) {
  if (xacc::hasAccelerator("aer")) {
    auto acc = xacc::getAccelerator("aer", {std::make_pair("shots", 1024)});
    auto buffer = xacc::qalloc(2);

    auto compiler = xacc::getCompiler("xasm");

    auto ir = compiler->compile(R"(__qpu__ void bell(qbit q) {
        H(q[0]);
        CX(q[0],q[1]);
    })",
                                nullptr);
    auto h = ir->getComposite("bell");

    auto qpt = xacc::getService<Algorithm>("qpt");
    EXPECT_TRUE(qpt->initialize(
        {std::make_pair("circuit", h), std::make_pair("accelerator", acc)}));
    qpt->execute(buffer);

    Eigen::MatrixXcd true_bell_chi(16, 16);
    true_bell_chi << 0.5 + 0.i, 0.5 + 0.i, 0. - 0.5i, 0.5 + 0.i, -0.5 + 0.i,
        0.5 + 0.i, 0. + 0.5i, 0.5 + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0.5 + 0.i, 0.5 + 0.i, 0. - 0.5i,
        0.5 + 0.i, -0.5 + 0.i, 0.5 + 0.i, 0. + 0.5i, 0.5 + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.5i, 0. + 0.5i, 0.5 + 0.i, 0. + 0.5i, 0. - 0.5i, 0. + 0.5i,
        -0.5 + 0.i, 0. + 0.5i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0.5 + 0.i, 0.5 + 0.i, 0. - 0.5i,
        0.5 + 0.i, -0.5 + 0.i, 0.5 + 0.i, 0. + 0.5i, 0.5 + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        -0.5 + 0.i, -0.5 + 0.i, 0. + 0.5i, -0.5 + 0.i, 0.5 + 0.i, -0.5 + 0.i,
        0. - 0.5i, -0.5 + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0.5 + 0.i, 0.5 + 0.i, 0. - 0.5i,
        0.5 + 0.i, -0.5 + 0.i, 0.5 + 0.i, 0. + 0.5i, 0.5 + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. - 0.5i, 0. - 0.5i, -0.5 + 0.i, 0. - 0.5i, 0. + 0.5i, 0. - 0.5i,
        0.5 + 0.i, 0. - 0.5i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0.5 + 0.i, 0.5 + 0.i, 0. - 0.5i,
        0.5 + 0.i, -0.5 + 0.i, 0.5 + 0.i, 0. + 0.5i, 0.5 + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i, 0. + 0.i,
        0. + 0.i, 0. + 0.i;

    auto chi_exp_real_vec = (*buffer)["chi-real"].as<std::vector<double>>();
    auto chi_exp_imag_vec = (*buffer)["chi-imag"].as<std::vector<double>>();

    Eigen::MatrixXd tmpchireal =
        Eigen::Map<Eigen::MatrixXd>(chi_exp_real_vec.data(), 16, 16);
    Eigen::MatrixXd tmpchiimag =
        Eigen::Map<Eigen::MatrixXd>(chi_exp_imag_vec.data(), 16, 16);

    Eigen::MatrixXcd chi = Eigen::MatrixXcd::Zero(16, 16);
    Eigen::MatrixXcd choi = Eigen::MatrixXcd::Zero(16, 16);
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        chi(i, j) = std::complex<double>(tmpchireal(i, j), tmpchiimag(i, j));
      }
    }

    std::cout << "Chi:\n" << chi << "\n\n";

    std::cout << fidelity(.25 * chi, .25 * true_bell_chi) << "\n";

    EXPECT_NEAR(1.0, fidelity(.25 * chi, .25 * true_bell_chi), 1e-1);
  }
}
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::external::load_external_language_plugins();

  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
