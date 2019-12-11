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
#include "AssignmentErrorKernelDecorator.hpp"
#include "InstructionIterator.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include <fstream>
#include <set>
#include <Eigen/Dense>

namespace xacc{
namespace quantum{
  void AssignmentErrorKernelDecorator::initialize(const HeterogeneousMap& params){

    if(params.keyExists<bool>("gen-kernel")){
      gen_kernel = params.get<bool>("gen-kernel");
    }
  }//initialize

  void AssignmentErrorKernelDecorator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                                             const std::shared_ptr<CompositeInstruction> function){
    int num_bits = (int)buffer->size();
    if(gen_kernel){
      if(decoratedAccelerator){
        //kernel is protected member, and is updated in function, no return value required.
        generateKernel(buffer);
      }

    }
    else{
      //generating the list of permutations is O(2^num_bits), we want to minimize the number of times we have to call it.
      if(permutations.empty()){
        permutations = generatePermutations(num_bits);
      }
    }
    //get the raw state
    decoratedAccelerator->execute(buffer, function);
    int shots = 0;
    for(auto &x : buffer->getMeasurementCounts()){
      shots += x.second;
    }
    int size = std::pow(2, num_bits);
    Eigen::VectorXd init_state(size);
    int i = 0;
    for(auto &x: permutations){
      init_state(i) = buffer->computeMeasurementProbability(x);
      i++;
    }
    std::cout<<"unmitigated_state: "<<std::endl;
    std::cout<<init_state<<std::endl<<std::endl;
    Eigen::VectorXd EM_state = errorKernel*init_state;
    std::cout<<"Error Mitigated:"<<std::endl;
    std::cout<<EM_state<<std::endl<<"\n";
    //checking for negative values and performing a "clip and renorm"
    for(int i = 0; i < EM_state.size(); i++){
      if(EM_state(i) < 0.0){
        int count = floor(shots*EM_state(i)+0.5);
        //std::cout<<count<<std::endl;
        std::cout<<"found negative value, clipping and renorming"<<std::endl;
        std::cout<<"removed "<<abs(count)<<" shots from total shots"<<std::endl;
        shots += count;
        EM_state(i) = 0.0;
      }
    }

    std::cout<<"Updated EM_state:"<<std::endl<<EM_state<<std::endl;
    //update buffer with new counts and save original counts to extra info

    std::map<std::string, double> origCounts;

    int total = 0;
    i = 0;
    for(auto &x: permutations){
      origCounts[x] = (double) buffer->getMeasurementCounts()[x];
      int count = floor(shots*EM_state(i)+0.5);
      total += count;
      buffer->appendMeasurement(x, count);
      i++;
    }
    //std::cout<<origCounts<<std::endl;
    buffer->addExtraInfo("unmitigated-counts", origCounts);

    return;
  }//execute



  void AssignmentErrorKernelDecorator::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>> functions){
    int num_bits = buffer->size();
    if(gen_kernel){
      if(decoratedAccelerator){
        generateKernel(buffer);
      }
    }
    else{
      if(permutations.empty()){
        permutations = generatePermutations(num_bits);
      }
    }
    //get the raw states
    decoratedAccelerator->execute(buffer, functions);
    int size = std::pow(2, num_bits);

    std::vector<Eigen::VectorXd> init_states;
    //compute number of shots;
    auto buffers = buffer->getChildren();
    int shots = 0;
    for(auto &x : buffers[0]->getMeasurementCounts()){
      shots += x.second;
    }
    std::cout<<"shots = "<<shots<<std::endl;


    int i = 0;
    for(auto &b: buffers){
      Eigen::VectorXd temp(size);
      int j = 0;
      for(auto&x: permutations){
        std::cout<<b->computeMeasurementProbability(x)<<std::endl;
        temp(j) = b->computeMeasurementProbability(x);
        j++;
      }
      std::cout<<"seg fault right here "<<std::endl;
      init_states.push_back(temp);
    }
    std::cout<<"unmitigated states: "<<std::endl;
    std::vector<Eigen::VectorXd> EM_states;
    i = 0;
    for(auto &x : init_states){
      std::cout<<x<<std::endl<<std::endl;
      EM_states.push_back(errorKernel*x);
      i++;
    }
    std::cout<<"mitigated states: "<<std::endl;
    for(auto &x: EM_states){
      std::cout<<x<<std::endl<<std::endl;
    }
    for(int i = 0; i < EM_states.size(); i++){
      for(int j = 0; j<EM_states[i].size(); j++){
        if(EM_states[i](j) < 0.0){
          int count = floor(shots*EM_states[i](j)+0.5);
          std::cout<<"found negative value, clipping and renorming "<<std::endl;
          std::cout<<"removed "<<abs(count)<<" shots from total shots"<<std::endl;
          shots += count;
          EM_states[i](j) = 0.0;
        }
      }
    }

    std::cout<<"Update EM_states: "<<std::endl;
    for(auto &x: EM_states){
      std::cout<<x<<std::endl<<std::endl;
    }

    std::vector<std::map<std::string, double>> origCounts(buffer->nChildren());

    int total = 0;
    i = 0;
    for(auto &b: buffers){
      int j = 0;
      for(auto &x: permutations){
        origCounts[i][x] = (double) b->getMeasurementCounts()[x];
        int count = floor(shots*EM_states[i](j)+0.5);
        j++;
        total += count;
        b->appendMeasurement(x, count);
        b->addExtraInfo("unmitigated-counts", origCounts[i]);
      }
      i++;
    }



    return;
  }//execute (vectorized)

} //namespace quantum
} //namespace xacc
