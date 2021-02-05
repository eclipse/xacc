option(XACC_ENABLE_QCEC "Build QCEC IR Verifier" OFF)

if (XACC_ENABLE_QCEC)
  add_subdirectory(qcec)
endif()
