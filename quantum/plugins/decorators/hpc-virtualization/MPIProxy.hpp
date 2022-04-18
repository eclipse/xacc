/** ExaTN: MPI Communicator Proxy & Process group
REVISION: 2021/09/27
Copyright (C) 2018-2021 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2021 Oak Ridge National Laboratory (UT-Battelle) **/

#ifndef XACC_MPI_COMM_PROXY_HPP_
#define XACC_MPI_COMM_PROXY_HPP_

#include <vector>
#include <memory>
#include <cassert>

namespace xacc {

class MPICommProxy {
public:

 /** Default constructor constructs an empty communicator. **/
 MPICommProxy(): mpi_comm_ptr_(nullptr), destroy_on_free_(false) {}

 /** Constructs a portable MPI communicator proxy by type-erasing
     the concrete MPI communicator type MPICommType (e.g., MPI_Comm). **/
 template<typename MPICommType>
 MPICommProxy(MPICommType mpi_comm,          //in: MPI intra-communicator
              bool destroy_on_free = false): //in: if TRUE, the stored MPI communicator will be explicitly destroyed when freed
  mpi_comm_ptr_(new MPICommType{mpi_comm}), destroy_on_free_(destroy_on_free) {}

 MPICommProxy(const MPICommProxy &) = default;
 MPICommProxy & operator=(const MPICommProxy &) = default;
 MPICommProxy(MPICommProxy &&) noexcept = default;
 MPICommProxy & operator=(MPICommProxy &&) noexcept = default;
 ~MPICommProxy();

 bool operator==(const MPICommProxy & another) const;
 bool operator!=(const MPICommProxy & another) const {return !(*this == another);}

 /** Returns TRUE if the MPI communicator is empty (non-existing). **/
 bool isEmpty() const {return (mpi_comm_ptr_ == nullptr);}

 /** Retrieves back a pointer to the stored MPI communicator with a proper type. **/
 template<typename MPICommType>
 MPICommType * get() const {return static_cast<MPICommType*>(mpi_comm_ptr_.get());}

 /** Retrieves back a reference to the stored MPI communicator with a proper type. **/
 template<typename MPICommType>
 MPICommType & getRef() const {return *(std::static_pointer_cast<MPICommType>(mpi_comm_ptr_));}

private:

 std::shared_ptr<void> mpi_comm_ptr_; //owning pointer to an MPI communicator (MPI_Comm type)
 bool destroy_on_free_; //whether or not to call MPI_Comm_free in destructor
};


class ProcessGroup {
public:

 static constexpr const std::size_t MAX_MEM_PER_PROCESS = 1UL * 1024UL * 1024UL * 1024UL; //bytes

 /** Constructs a process group from provided global MPI process ranks and their
     associated intra-comunicator, with an optional memory limit per process. **/
 ProcessGroup(const MPICommProxy & intra_comm,
              const std::vector<unsigned int> & global_process_ranks,
              std::size_t mem_per_process = MAX_MEM_PER_PROCESS):
  process_ranks_(global_process_ranks), intra_comm_(intra_comm), mem_per_process_(mem_per_process)
 {
  assert(!process_ranks_.empty());
 }

 /** Constructs the default process group with the given number of MPI process ranks and
     their associated intra-comunicator, with an optional memory limit per process. **/
 ProcessGroup(const MPICommProxy & intra_comm,
              const unsigned int group_size,
              std::size_t mem_per_process = MAX_MEM_PER_PROCESS):
  process_ranks_(group_size), intra_comm_(intra_comm), mem_per_process_(mem_per_process)
 {
  assert(process_ranks_.size() > 0);
  for(unsigned int i = 0; i < process_ranks_.size(); ++i) process_ranks_[i] = i;
 }

 ProcessGroup(const ProcessGroup &) = default;
 ProcessGroup & operator=(const ProcessGroup &) = default;
 ProcessGroup(ProcessGroup &&) noexcept = default;
 ProcessGroup & operator=(ProcessGroup &&) noexcept = default;
 ~ProcessGroup() = default;

 /** Checks whether the process group has the same
     intra-communicator as another process group,
     hence both process groups being fully identical. **/
 inline bool operator==(const ProcessGroup & another) const
 {
  return (intra_comm_ == another.intra_comm_);
 }

 /** Returns TRUE if the process group is composed
     of the same processes as another process group,
     with no regard to their intra-communicators. **/
 bool isCongruentTo(const ProcessGroup & another) const;

 /** Returns TRUE if the process group is contained in
     or congruent to another process group. **/
 bool isContainedIn(const ProcessGroup & another) const;

 /** Returns the size of the process group (number of MPI processes). **/
 unsigned int getSize() const {return process_ranks_.size();}

 /** Returns the process group composition in terms of global MPI process ranks. **/
 const std::vector<unsigned int> & getProcessRanks() const {return process_ranks_;}

 /** Returns the MPI communicator proxy associated with the process group. **/
 const MPICommProxy & getMPICommProxy() const {return intra_comm_;}

 /** Returns TRUE if the provided global MPI process rank belongs to the process group. **/
 bool rankIsIn(const unsigned int global_process_rank,
               unsigned int * local_process_rank = nullptr) const
 {
  for(unsigned int i = 0; i < process_ranks_.size(); ++i){
   if(process_ranks_[i] == global_process_rank){
    if(local_process_rank != nullptr) *local_process_rank = i;
    return true;
   }
  }
  return false;
 }

 /** Resets the memory limit per MPI process in bytes. **/
 void resetMemoryLimitPerProcess(std::size_t memory_limit = MAX_MEM_PER_PROCESS){
  mem_per_process_ = memory_limit;
  return;
 }

 /** Returns the current memory limit per MPI process in bytes. **/
 std::size_t getMemoryLimitPerProcess() const {return mem_per_process_;}

 /** Splits the existing process group into subgroups and returns
     the process subgroup the current MPI process belongs to (or none).
     The argument my_subgroup must be non-negative, otherwise no new
     process subgroup will be returned for the current MPI process.
     All MPI processes with the same (non-negative) my_subgroup value
     will join a new process subgroup returned by this function.
     Note that the (non-negative) my_subgroup value may differ between
     different MPI processes, thus putting them into disjoint subgroups. **/
 std::shared_ptr<ProcessGroup> split(int my_subgroup) const;

protected:

 std::vector<unsigned int> process_ranks_; //global ranks of the MPI processes forming the process group
 MPICommProxy intra_comm_;                 //associated MPI intra-communicator
 std::size_t mem_per_process_;             //dynamic memory limit per process (bytes)
};

} //namespace xacc

#endif //XACC_MPI_COMM_PROXY_HPP_