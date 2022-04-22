/** ExaTN: MPI Communicator Proxy & Process group
REVISION: 2021/09/26
Copyright (C) 2018-2021 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2021 Oak Ridge National Laboratory (UT-Battelle) **/

#include "MPIProxy.hpp"

#include "mpi.h"

#include <cstdlib>

#include <iostream>
#include <algorithm>

namespace xacc {

//Temporary buffers:
constexpr std::size_t MAX_NUM_MPI_PROCESSES = 65536;
std::vector<unsigned int> processes1;
std::vector<unsigned int> processes2;


MPICommProxy::~MPICommProxy()
{
 if(destroy_on_free_){
  if(!(this->isEmpty())){
   if(mpi_comm_ptr_.use_count() == 1){
    auto * mpicomm = this->get<MPI_Comm>();
    int res;
    auto errc = MPI_Comm_compare(*mpicomm,MPI_COMM_WORLD,&res); assert(errc == MPI_SUCCESS);
    if(res != MPI_IDENT){
     errc = MPI_Comm_compare(*mpicomm,MPI_COMM_SELF,&res); assert(errc == MPI_SUCCESS);
     if(res != MPI_IDENT){
      errc = MPI_Comm_free(mpicomm); assert(errc == MPI_SUCCESS);
     }
    }
   }
  }
 }
}


bool MPICommProxy::operator==(const MPICommProxy & another) const
{
 bool equal = true;
 auto * lhs_comm = this->get<MPI_Comm>();
 auto * rhs_comm = another.get<MPI_Comm>();
 int res;
 auto errc = MPI_Comm_compare(*lhs_comm,*rhs_comm,&res); assert(errc == MPI_SUCCESS);
 equal = (res == MPI_IDENT);
 return equal;
}


bool ProcessGroup::isCongruentTo(const ProcessGroup & another) const
{
 bool is_congruent = (*this == another);
 if(!is_congruent){
  is_congruent = (this->process_ranks_.size() == another.process_ranks_.size());
  if(is_congruent && this->process_ranks_.size() > 0){
   processes1.reserve(MAX_NUM_MPI_PROCESSES);
   processes2.reserve(MAX_NUM_MPI_PROCESSES);
   processes1 = this->process_ranks_;
   std::sort(processes1.begin(),processes1.end());
   processes2 = another.process_ranks_;
   std::sort(processes2.begin(),processes2.end());
   is_congruent = (processes1 == processes2);
  }
 }
 return is_congruent;
}


bool ProcessGroup::isContainedIn(const ProcessGroup & another) const
{
 bool is_contained = (*this == another);
 if(!is_contained){
  is_contained = (this->process_ranks_.size() <= another.process_ranks_.size());
  if(is_contained){
   processes1.reserve(MAX_NUM_MPI_PROCESSES);
   processes2.reserve(MAX_NUM_MPI_PROCESSES);
   processes1 = this->process_ranks_;
   std::sort(processes1.begin(),processes1.end());
   processes2 = another.process_ranks_;
   std::sort(processes2.begin(),processes2.end());
   is_contained = std::includes(processes2.begin(),processes2.end(),processes1.begin(),processes1.end());
  }
 }
 return is_contained;
}


std::shared_ptr<ProcessGroup> ProcessGroup::split(int my_subgroup) const
{
 std::shared_ptr<ProcessGroup> subgroup(nullptr);
 if(this->getSize() == 1){
  if(my_subgroup >= 0) subgroup = std::make_shared<ProcessGroup>(*this);
 }else{
  if(my_subgroup >= 0) subgroup = std::make_shared<ProcessGroup>(*this);
  if(!(intra_comm_.isEmpty())){
   auto & mpicomm = intra_comm_.getRef<MPI_Comm>();
   int color = MPI_UNDEFINED;
   if(my_subgroup >= 0) color = my_subgroup;
   int my_orig_rank;
   auto errc = MPI_Comm_rank(mpicomm,&my_orig_rank); assert(errc == MPI_SUCCESS);
   MPI_Comm subgroup_mpicomm;
   errc = MPI_Comm_split(mpicomm,color,my_orig_rank,&subgroup_mpicomm); assert(errc == MPI_SUCCESS);
   if(color != MPI_UNDEFINED){
    int subgroup_size;
    errc = MPI_Comm_size(subgroup_mpicomm,&subgroup_size); assert(errc == MPI_SUCCESS);
    MPI_Group orig_group,new_group;
    errc = MPI_Comm_group(mpicomm,&orig_group); assert(errc == MPI_SUCCESS);
    errc = MPI_Comm_group(subgroup_mpicomm,&new_group); assert(errc == MPI_SUCCESS);
    int sub_ranks[subgroup_size],orig_ranks[subgroup_size];
    for(int i = 0; i < subgroup_size; ++i) sub_ranks[i] = i;
    errc = MPI_Group_translate_ranks(new_group,subgroup_size,sub_ranks,orig_group,orig_ranks);
    std::vector<unsigned int> subgroup_ranks(subgroup_size); //vector of global MPI ranks
    const auto & ranks = this->getProcessRanks();
    for(int i = 0; i < subgroup_size; ++i) subgroup_ranks[i] = ranks[orig_ranks[i]];
    subgroup = std::make_shared<ProcessGroup>(MPICommProxy(subgroup_mpicomm,true),
                                              subgroup_ranks,
                                              this->getMemoryLimitPerProcess());
   }
  }else{
   std::cout << "#ERROR(xacc::ProcessGroup::split): Empty MPI communicator!\n" << std::flush;
   assert(false);
  }
 }
 return subgroup;
}

} //namespace xacc