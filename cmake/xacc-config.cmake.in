# - Config file for XACC
# To point to your install of XACC, pass the
# XACC_ROOT flag to your cmake configure.
#
# It defines the following variables
#  XACC_INCLUDE_DIRS - include directories for XACC
#  XACC_LIBRARIES    - libraries to link against
#  XACC_LIBRARY_DIR  - the XACC library directory

include(CTest)

if (NOT XACC_ROOT)
   get_filename_component(XACC_ROOT "${CMAKE_CURRENT_LIST_FILE}" PATH)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${XACC_ROOT}/share/xacc)

if (NOT TARGET format)
   include(format)
endif()

set (XACC_LIBRARY_DIR "${XACC_ROOT}/lib")

link_directories("${XACC_ROOT}/lib")
set(XACC_INCLUDE_ROOT "${XACC_ROOT}/include")

set (XACC_INCLUDE_DIRS "${XACC_INCLUDE_ROOT}/antlr4-runtime;${XACC_INCLUDE_ROOT}/cppmicroservices4;${XACC_INCLUDE_ROOT}/xacc;${XACC_INCLUDE_ROOT}/quantum/gate;${XACC_INCLUDE_ROOT}/quantum/utils;${XACC_INCLUDE_ROOT}/quantum/aqc;${XACC_INCLUDE_ROOT}/spdlog;${XACC_INCLUDE_ROOT}/exprtk;${XACC_INCLUDE_ROOT}/eigen;${XACC_INCLUDE_ROOT}/rapidjson/include;${XACC_INCLUDE_ROOT};${XACC_INCLUDE_ROOT}/gtest")
set (XACC_LIBRARIES "xacc;xacc-quantum-gate;xacc-quantum-aqc;xacc-pauli;antlr4-runtime;CppMicroServices")

set (XACC_TEST_LIBRARIES "${XACC_LIBRARIES};gtest;gtest_main")

macro(add_xacc_test _TEST_NAME)
    add_executable(${_TEST_NAME}Tester ${_TEST_NAME}Tester.cpp)
    add_test(NAME xacc_${_TEST_NAME}Tester COMMAND ${_TEST_NAME}Tester)
    target_link_libraries(${_TEST_NAME}Tester ${XACC_TEST_LIBRARIES})
endmacro()
