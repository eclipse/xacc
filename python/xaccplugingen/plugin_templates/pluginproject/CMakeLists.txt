cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project({project_name} LANGUAGES CXX)

option({project_shortname_upper}_BUILD_TESTS "Build test programs" OFF)

set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
message(STATUS "C++ version ${{CXX_STANDARD}} configured.")

find_package(XACC REQUIRED)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${{XACC_ROOT}}" CACHE PATH "default install path" FORCE )
endif()

set(CppMicroServicesConfig_DIR "${{XACC_ROOT}}/share/cppmicroservices4/cmake")
set(CMAKE_MODULE_PATH ${{CMAKE_MODULE_PATH}} "${{XACC_ROOT}}/share/xacc")
set(CMAKE_MODULE_PATH ${{CMAKE_MODULE_PATH}} "${{CppMicroServicesConfig_DIR}}")
include(tests)
include(CppMicroServicesConfig)

include_directories(${{XACC_INCLUDE_DIRS}})
include_directories(${{CMAKE_SOURCE_DIR}}/utils)

link_directories(${{XACC_LIBRARY_DIR}})

# TODO: add_subdirectory for each component
# add_subdirectory(compiler)

if(PYTHON_INCLUDE_DIR)
   add_subdirectory(python)
endif()

if({project_shortname_upper}_BUILD_TESTS)
   # TODO: include_directories for each component
   # include_directories(${{CMAKE_SOURCE_DIR}}/compiler)
   add_subdirectory(tests)
endif()
