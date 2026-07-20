#pragma once
#include <cuda_runtime.h>
#include <cutensornet.h>

#define HANDLE_CUDA_ERROR(x)                                                   \
  {                                                                            \
    const auto err = x;                                                        \
    if (err != cudaSuccess) {                                                  \
      printf("CUDA error %s in line %d\n", cudaGetErrorString(err), __LINE__); \
      fflush(stdout);                                                          \
      std::abort();                                                            \
    }                                                                          \
  };

#define HANDLE_CUTN_ERROR(x)                                                   \
  {                                                                            \
    const auto err = x;                                                        \
    if (err != CUTENSORNET_STATUS_SUCCESS) {                                   \
      printf("cuTensorNet error %s in line %d\n",                              \
             cutensornetGetErrorString(err), __LINE__);                        \
      fflush(stdout);                                                          \
      std::abort();                                                            \
    }                                                                          \
  };