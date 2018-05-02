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
set (XACC_INCLUDE_DIRS "${XACC_ROOT}/include/cppmicroservices3;${XACC_ROOT}/include/xacc;${XACC_ROOT}/include/quantum/gate;${XACC_ROOT}/include/quantum/aqc;${XACC_ROOT}/include/spdlog;${XACC_ROOT}/include/exprtk;${XACC_ROOT}/include/eigen;${XACC_ROOT}/include/rapidjson/include;${XACC_ROOT}/include")
set (XACC_LIBRARY_DIR "${XACC_ROOT}/lib")
link_directories("${XACC_ROOT}/lib")
set (XACC_LIBRARIES "xacc;CppMicroServices;boost_system;boost_filesystem;boost_regex;boost_graph;boost_program_options;boost_chrono;boost_unit_test_framework;cpr;curl")
set(BOOST_ROOT ${XACC_ROOT})
