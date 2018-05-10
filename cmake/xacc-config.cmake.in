# - Config file for XACC
# To point to your install of XACC, pass the 
# XACC_ROOT flag to your cmake configure.
#
# It defines the following variables
#  XACC_INCLUDE_DIRS - include directories for XACC
#  XACC_LIBRARIES    - libraries to link against
#  XACC_LIBRARY_DIR  - the XACC library directory 
if (NOT XACC_ROOT)
   get_filename_component(XACC_ROOT "${CMAKE_CURRENT_LIST_FILE}" PATH)
endif()
set (XACC_INCLUDE_DIRS "${XACC_ROOT}/include/cppmicroservices4;${XACC_ROOT}/include/xacc;${XACC_ROOT}/include/quantum/gate;${XACC_ROOT}/include/quantum/aqc;${XACC_ROOT}/include/spdlog;${XACC_ROOT}/include/exprtk;${XACC_ROOT}/include/eigen;${XACC_ROOT}/include/rapidjson/include;${XACC_ROOT}/include;${XACC_ROOT}/include/gtest")
set (XACC_LIBRARY_DIR "${XACC_ROOT}/lib")
link_directories("${XACC_ROOT}/lib")
set (XACC_LIBRARIES "xacc;xacc-quantum-gate;xacc-quantum-aqc;CppMicroServices;boost_system;boost_filesystem;boost_regex;boost_graph;boost_program_options")
set (XACC_TEST_LIBRARIES "${XACC_LIBRARIES};gtest;gtest_main")
set(BOOST_ROOT ${XACC_ROOT})
macro(add_xacc_test _TEST_NAME)
    add_executable(${_TEST_NAME}Tester ${_TEST_NAME}Tester.cpp)
    add_test(NAME xacc_${_TEST_NAME}Tester COMMAND ${_TEST_NAME}Tester)
    target_link_libraries(${_TEST_NAME}Tester ${XACC_TEST_LIBRARIES})
endmacro()
