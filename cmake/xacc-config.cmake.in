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

set(XACC_INCLUDE_ROOT "${XACC_ROOT}/include")
#set (XACC_INCLUDE_DIRS "${XACC_INCLUDE_ROOT}/antlr4-runtime;${XACC_INCLUDE_ROOT}/cppmicroservices4;${XACC_INCLUDE_ROOT}/xacc;${XACC_INCLUDE_ROOT}/quantum/gate;${XACC_INCLUDE_ROOT}/quantum/utils;${XACC_INCLUDE_ROOT}/quantum/aqc;${XACC_INCLUDE_ROOT}/spdlog;${XACC_INCLUDE_ROOT}/exprtk;${XACC_INCLUDE_ROOT}/eigen;${XACC_INCLUDE_ROOT}/rapidjson/include;${XACC_INCLUDE_ROOT};${XACC_INCLUDE_ROOT}/gtest")
#set (XACC_LIBRARIES "xacc;xacc-quantum-gate;xacc-quantum-aqc;xacc-pauli;antlr4-runtime;CppMicroServices")

set (XACC_TEST_LIBRARIES "gtest;gtest_main")

macro(add_xacc_test _TEST_NAME)
    link_directories(${XACC_ROOT}/lib)
    include_directories(${XACC_INCLUDE_ROOT}/gtest)
    add_executable(${_TEST_NAME}Tester ${_TEST_NAME}Tester.cpp)
    add_test(NAME xacc_${_TEST_NAME}Tester COMMAND ${_TEST_NAME}Tester)
    target_link_libraries(${_TEST_NAME}Tester ${XACC_TEST_LIBRARIES})
endmacro()


set(CppMicroServicesConfig_DIR "${XACC_ROOT}/share/cppmicroservices4/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XACC_ROOT}/share/xacc")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CppMicroServicesConfig_DIR}")
include(CppMicroServicesConfig)

macro(xacc_configure_library_rpath LIBNAME)
  if(APPLE)
    set_target_properties(${LIBNAME} PROPERTIES INSTALL_RPATH "@loader_path")
    set_target_properties(${LIBNAME}
                          PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  else()
    set_target_properties(${LIBNAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS "-shared")
  endif()
endmacro()

macro(xacc_configure_plugin_rpath LIBNAME)
  if(APPLE)
    set_target_properties(${LIBNAME} PROPERTIES INSTALL_RPATH "@loader_path/../lib")
    set_target_properties(${LIBNAME}
                          PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  else()
    set_target_properties(${LIBNAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
    set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS "-shared")
  endif()
endmacro()


set(XACCLIB "libxacc.so")
set(XACCGATELIB "libxacc-quantum-gate.so")
set(XACCAQCLIB "libxacc-quantum-annealing.so")
set(XACCPAULILIB "libxacc-pauli.so")
set(XACCFERMIONLIB "libxacc-fermion.so")
set(CPPUSLIB "libCppMicroServices.so.4.0.0")

if (APPLE)
  set(XACCLIB "libxacc.dylib")
  set(XACCGATELIB "libxacc-quantum-gate.dylib")
  set(XACCAQCLIB "libxacc-quantum-annealing.dylib")
  set(XACCPAULILIB "libxacc-pauli.dylib")
  set(XACCFERMIONLIB "libxacc-fermion.dylib")
  set(CPPUSLIB "libCppMicroServices.4.0.0.dylib")
endif()

# Create imported target CppMicroServices::CppMicroServices
add_library(CppMicroServices::CppMicroServices SHARED IMPORTED)

set_target_properties(CppMicroServices::CppMicroServices PROPERTIES
  INTERFACE_COMPILE_FEATURES "cxx_variadic_templates;cxx_nullptr"
  INTERFACE_INCLUDE_DIRECTORIES "${XACC_INCLUDE_ROOT}/cppmicroservices4"
  INTERFACE_LINK_LIBRARIES "dl;pthread"
)

# Import target "CppMicroServices::CppMicroServices" for configuration "Release"
set_property(TARGET CppMicroServices::CppMicroServices APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(CppMicroServices::CppMicroServices PROPERTIES
  IMPORTED_LOCATION_RELEASE "${XACC_ROOT}/lib/${CPPUSLIB}"
  IMPORTED_SONAME_RELEASE "${CPPUSLIB}"
  )

# Create imported target xacc::xacc
add_library(xacc::xacc SHARED IMPORTED)

set_target_properties(xacc::xacc PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${XACC_INCLUDE_ROOT}/xacc"
  INTERFACE_LINK_LIBRARIES "CppMicroServices::CppMicroServices"
)

target_compile_features(xacc::xacc
                        INTERFACE
                        cxx_std_14
                        cxx_alias_templates)

# Import target "xacc::xacc" for configuration "Release"
set_property(TARGET xacc::xacc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xacc::xacc PROPERTIES
  IMPORTED_LOCATION_RELEASE "${XACC_ROOT}/lib/${XACCLIB}"
  IMPORTED_SONAME_RELEASE "${XACCLIB}"
  )

add_library(xacc::quantum_gate SHARED IMPORTED)

set_target_properties(xacc::quantum_gate PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${XACC_INCLUDE_ROOT}/xacc;${XACC_INCLUDE_ROOT}/quantum/gate"
  INTERFACE_LINK_LIBRARIES "xacc::xacc"
)

# Import target "xacc::quantum_gate" for configuration "Release"
set_property(TARGET xacc::quantum_gate APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xacc::quantum_gate PROPERTIES
  IMPORTED_LOCATION_RELEASE "${XACC_ROOT}/lib/${XACCGATELIB}"
  IMPORTED_SONAME_RELEASE "${XACCGATELIB}"
  )

# Create imported target xacc::quantum_aqc
add_library(xacc::quantum_annealing SHARED IMPORTED)

set_target_properties(xacc::quantum_annealing PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${XACC_INCLUDE_ROOT}/xacc;${XACC_INCLUDE_ROOT}/quantum/annealing"
  INTERFACE_LINK_LIBRARIES "xacc::xacc"
)

# Import target "xacc::quantum_annealing" for configuration "Release"
set_property(TARGET xacc::quantum_annealing APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xacc::quantum_annealing PROPERTIES
  IMPORTED_LOCATION_RELEASE "${XACC_ROOT}/lib/${XACCAQCLIB}"
  IMPORTED_SONAME_RELEASE "${XACCAQCLIB}"
  )


# Create imported target xacc::pauli
add_library(xacc::pauli SHARED IMPORTED)

set_target_properties(xacc::pauli PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${XACC_INCLUDE_ROOT}/xacc;${XACC_INCLUDE_ROOT}/quantum/gate"
  INTERFACE_LINK_LIBRARIES "xacc::xacc"
)

# Import target "xacc::pauli" for configuration "Release"
set_property(TARGET xacc::pauli APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xacc::pauli PROPERTIES
  IMPORTED_LOCATION_RELEASE "${XACC_ROOT}/lib/${XACCPAULILIB}"
  IMPORTED_SONAME_RELEASE "${XACCPAULILIB}"
  )


# Create imported target xacc::fermion
add_library(xacc::fermion SHARED IMPORTED)

set_target_properties(xacc::fermion PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${XACC_INCLUDE_ROOT}/xacc;${XACC_INCLUDE_ROOT}/quantum/gate"
  INTERFACE_LINK_LIBRARIES "xacc::xacc"
)

# Import target "xacc::fermion" for configuration "Release"
set_property(TARGET xacc::fermion APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xacc::fermion PROPERTIES
  IMPORTED_LOCATION_RELEASE "${XACC_ROOT}/lib/${XACCFERMIONLIB}"
  IMPORTED_SONAME_RELEASE "${XACCFERMIONLIB}"
  )

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${XACC_ROOT}" CACHE PATH "default install path" FORCE )
endif()
