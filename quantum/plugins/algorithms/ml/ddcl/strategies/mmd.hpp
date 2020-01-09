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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_ALGORITHM_DDCL_STRATEGIES_MMD_LOSS_HPP_
#define XACC_ALGORITHM_DDCL_STRATEGIES_MMD_LOSS_HPP_

#include "ddcl.hpp"
#include "xacc.hpp"
#include <cassert>
#include <set>
#include <Eigen/Dense>
#include "InstructionIterator.hpp"

namespace xacc {
namespace algorithm {

class MMDLossStrategy : public LossStrategy {
protected:
  // Helper functions
  Eigen::MatrixXd mix_rbf_kernel(std::vector<int> x,std::vector<int> y,
                                 std::vector<double> sigma_list, int num_bit){
    //it's always the case that x = y = [0, 1, ..., pow(2, num_bit)-1];
    std::vector<int> temp_x(x.size());
    std::vector<int> temp_y(y.size());
    Eigen::MatrixXd dx2 = Eigen::MatrixXd::Zero(x.size(), x.size());
    for(int i = 0; i < num_bit; i++){
      for(int j = 0; j < x.size(); j++){
        temp_x[j] = (x[j]>>i)&1;
        temp_y[j] = (y[j] >>i)&1;
      }
      for(int k = 0; k < x.size(); k++){
        for(int l = 0; l < x.size(); l++){
          dx2(k,l) += (temp_x[k] != temp_y[l]);
        }
      }

    }
    return _mix_rbf_kernel_d(dx2, sigma_list);

  }


  Eigen::MatrixXd _mix_rbf_kernel_d(Eigen::MatrixXd dx2, std::vector<double> sigma_list){
    Eigen::MatrixXd K = Eigen::MatrixXd::Zero(dx2.rows(), dx2.cols());
      Eigen::MatrixXd _dx2 = dx2;
      for(auto &sigma : sigma_list){
        double gamma = 1.0/(2*sigma);
        for(int i = 0; i < dx2.rows(); i++){
          for(int j = 0; j < dx2.cols(); j++){
              _dx2(i,j) = std::exp(-gamma*dx2(i,j));
          }
        }
        K += _dx2;
      }
      return K;
  }

  double kernel_expect(Eigen::MatrixXd K,
                       std::vector<double> px,
                       std::vector<double>  py){
    int len = px.size();
    //This takes px and py and turns them into eigen::Vector objects
    Eigen::VectorXd P = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(px.data(), px.size());
    Eigen::VectorXd Q = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(py.data(), py.size());

    auto temp = K*Q;
    double expectation = P.dot(temp);


    return expectation;
  }

public:
  std::pair<double, std::vector<double>>
  compute(Counts &counts, const std::vector<double> &target, const HeterogeneousMap& = {}) override {
    int shots = 0;
    for (auto &x : counts) {
      shots += x.second;
    }


    // Compute the probability distribution
    std::vector<double> q(target.size()); // all zeros
    for (auto &x : counts) {
      int idx = std::stoi(x.first, nullptr, 2);
      q[idx] = (double)x.second / shots;
    }


    std::vector<double> pxy(q.size());
    for(int i = 0; i < q.size(); i++)
      {
        pxy[i] = std::abs(target[i] - q[i]);
      }


    //worried about edge cases, anywhere in here where I can access this information
    //without type instability
    ///Also how can I allow sigma_list to be passed as parameters into the
    //loss?
    int num_bit = (int)log2(target.size());

    std::vector<int> basis(pow(2,num_bit));
    for(int i = 0; i < pow(2,num_bit); i++){
      basis[i] = i;
    }

    std::vector<double> sigma_list = {0.25};
    Eigen::MatrixXd K = mix_rbf_kernel(basis, basis, sigma_list, num_bit);
    auto mmd = kernel_expect(K, pxy, pxy);

    return std::make_pair(mmd, q);
  }


  bool isValidGradientStrategy(const std::string &gradientStrategy) override {
        // FIXME define what grad strategies this guy works with

    return true;
  }

  const std::string name() const override { return "mmd"; }
  const std::string description() const override { return ""; }
};


  //Loss function implemetation complete



  class MMDParameterShiftGradientStrategy : public GradientStrategy {
  protected:
    std::vector<double> currentParameterSet;

    double kernel_expect(Eigen::MatrixXd K,
                         std::vector<double> px,
                         std::vector<double>  py){
      int len = px.size();
      //This takes px and py and turns them into eigen::Vector objects
      //YOU_CALLED_A_FIXED_SIZE_METHOD_ON_A_DYNAMIC_SIZE_MATRIX
      Eigen::VectorXd P = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(px.data(), px.size());
      Eigen::VectorXd Q = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(py.data(), py.size());

      double expectation = P.dot(K*Q);


      return expectation;
    }

    Eigen::MatrixXd mix_rbf_kernel(std::vector<int> x,std::vector<int> y,
                                   std::vector<double> sigma_list, int num_bit){
      //it's always the case that x = y = [0, 1, ..., pow(2, num_bit)-1];
      std::vector<int> temp_x(x.size());
      std::vector<int> temp_y(y.size());
      Eigen::MatrixXd dx2 = Eigen::MatrixXd::Zero(x.size(), x.size());
      for(int i = 0; i < num_bit; i++){
        for(int j = 0; j < x.size(); j++){
            temp_x[j] = (x[j]>>i)&1;
            temp_y[j] = (y[j] >>i)&1;
        }
        for(int k = 0; k < x.size(); k++){
          for(int l = 0; l < x.size(); l++){
            dx2(k,l) += (temp_x[k] != temp_y[l]);
          }
        }
      }
        return _mix_rbf_kernel_d(dx2, sigma_list);
    }

    Eigen::MatrixXd _mix_rbf_kernel_d(Eigen::MatrixXd dx2, std::vector<double> sigma_list){
      Eigen::MatrixXd K = Eigen::MatrixXd::Zero(dx2.rows(), dx2.cols());
      Eigen::MatrixXd _dx2 = dx2;
      for(auto &sigma : sigma_list){
        double gamma = 1.0/(2*sigma);
          for(int i = 0; i < dx2.rows(); i++){
            for(int j = 0; j < dx2.cols(); j++){
              _dx2(i,j) = std::exp(-gamma*dx2(i,j));
            }
          }
          K += _dx2;
      }
      return K;
    }

  public:
    std::vector<Circuit>
    getCircuitExecutions(Circuit circuit, const std::vector<double> &x) override {
      currentParameterSet = x;
      std::set<std::size_t> uniqueBits = circuit->uniqueBits();
      std::vector<Circuit> grad_circuits;
      auto provider = xacc::getIRProvider("quantum");
      for (int i = 0; (i < x.size()); i++) {
        //shifts params +/- pi/2
        auto xplus = x[i] + xacc::constants::pi / 2.;
        auto xminus = x[i] - xacc::constants::pi / 2.;
        std::vector<double> tmpx_plus = x, tmpx_minus = x;
        tmpx_plus[i] = xplus;
        tmpx_minus[i] = xminus;
        //builds circuits with those params
        auto xplus_circuit = circuit->operator()(tmpx_plus);
        auto xminus_circuit = circuit->operator()(tmpx_minus);
        //run circuit for pos vals
        for (std::size_t i = 0; i < xplus_circuit->nLogicalBits(); i++) {
          auto m =
            provider->createInstruction("Measure", std::vector<std::size_t>{i});
          xplus_circuit->addInstruction(m);
        }
        //run circuit for minus vals
        for (std::size_t i = 0; i < xminus_circuit->nLogicalBits(); i++) {
          auto m =
            provider->createInstruction("Measure", std::vector<std::size_t>{i});
          xminus_circuit->addInstruction(m);
        }
        //save result

        grad_circuits.push_back(xplus_circuit);
        grad_circuits.push_back(xminus_circuit);
      }
      return grad_circuits;
    }
    void compute(std::vector<double> &grad, std::vector<std::shared_ptr<AcceleratorBuffer>> results,
                 const std::vector<double> &q_dist,
                 const std::vector<double> &target_dist) override {
      assert(2*grad.size() == results.size());

      //Get the number of shots
      int shots = 0;
      for (auto &x : results[0]->getMeasurementCounts()) {
        shots += x.second;
      }

      //q+ and q- vectors
      int counter = 0;
      std::vector<std::vector<double>> qplus_theta, qminus_theta;
      for (int i = 0; i < results.size(); i += 2) {

        std::vector<double> qp(q_dist.size()), qm(q_dist.size());
        for (auto &x : results[i]->getMeasurementCounts()) {
          int idx = std::stoi(x.first, nullptr, 2);
          qp[idx] = (double)x.second / shots;
        }
        for (auto &x : results[i + 1]->getMeasurementCounts()) {
          int idx = std::stoi(x.first, nullptr, 2);
          qm[idx] = (double)x.second / shots;
        }
        std::vector<double> shiftedp = currentParameterSet;
        std::vector<double> shiftedm = currentParameterSet;
        auto xplus = currentParameterSet[counter] + xacc::constants::pi / 2;
        auto xminus = currentParameterSet[counter] - xacc::constants::pi / 2;

        shiftedp[counter] = xplus;
        shiftedm[counter] = xminus;


        results[i]->addExtraInfo("gradient-index", counter);
        results[i+1]->addExtraInfo("gradient-index", counter);

        results[i]->addExtraInfo("shift-direction", "plus");
        results[i+1]->addExtraInfo("shift-direction", "minus");

        results[i]->addExtraInfo("qdist", qp);
        results[i+1]->addExtraInfo("qdist", qm);

        results[i]->addExtraInfo("gradient-parameters", shiftedp);
        results[i+1]->addExtraInfo("gradient-parameters", shiftedm);

        qplus_theta.push_back(qp);
        qminus_theta.push_back(qm);

        counter++;

      }

      std::vector<double> sigma_list = {0.1};
      int num_bit = (int) log2(q_dist.size());
      std::vector<int> basis(pow(2,num_bit));
      for(int i = 0; i < pow(2,num_bit); i++){
        basis[i] = i;
      }
      Eigen::MatrixXd K = mix_rbf_kernel(basis, basis, sigma_list, num_bit);
      //compute gradient vector
      std::vector<double> grad_pos(counter);
      std::vector<double> grad_neg(counter);
      std::vector<double> grad_pos_targ(counter);
      std::vector<double> grad_neg_targ(counter);

      for(int i = 0; i < counter; i++){
        grad_pos[i] = kernel_expect(K, qplus_theta[i], q_dist);
        grad_neg[i] = kernel_expect(K, qminus_theta[i], q_dist);
        grad_pos_targ[i] = kernel_expect(K, qplus_theta[i], target_dist);
        grad_neg_targ[i] = kernel_expect(K, qminus_theta[i], target_dist);
        grad[i] = grad_pos[i]-grad_pos_targ[i]-grad_neg[i]+grad_neg_targ[i];
      }
      return;
    }
    const std::string name() const override { return "mmd-parameter-shift"; }
    const std::string description() const override { return ""; }
  };

} // namespace algorithm
} // namespace xacc
#endif
