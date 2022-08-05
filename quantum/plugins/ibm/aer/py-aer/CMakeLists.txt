set(LIBRARY_NAME xacc-py-aer-adapter)
find_package(Python COMPONENTS Interpreter Development)
get_filename_component(PYTHON_LIB_NAME ${Python_LIBRARIES} NAME)
configure_file(aer_python_adapter.in.cpp
               ${CMAKE_BINARY_DIR}/quantum/plugins/ibm/aer/py-aer/aer_python_adapter.cpp)
file(GLOB SRC ${CMAKE_BINARY_DIR}/quantum/plugins/ibm/aer/py-aer/aer_python_adapter.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64") 
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -mtune=native -D_GNU_SOURCE -fPIC -fwrapv")
else()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=native -D_GNU_SOURCE -fPIC -fwrapv")
endif()
add_library(${LIBRARY_NAME} SHARED ${SRC})

if(Python_FOUND)
    message(STATUS "${BoldGreen}Found Python version ${Python_VERSION}.${ColorReset}")
    # Always install the Aer Pulse Python Adapter when having Python.
    # Users may need to do `pip install qiskit` if they don't have Qiskit yet.
    target_include_directories(${LIBRARY_NAME} PUBLIC . ${Python_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/tpls/pybind11/include ${CMAKE_BINARY_DIR})
    target_link_libraries(${LIBRARY_NAME} PRIVATE Python::Python)
   
    # Check if we have Qiskit installed 
    execute_process(COMMAND ${Python_EXECUTABLE} -c "import qiskit; print('Qiskit: ', qiskit.__version__)" RESULT_VARIABLE QISKIT_EXISTS)
    if (QISKIT_EXISTS EQUAL "1")
      message(STATUS "${BoldYellow}Qiskit not found. To use the Aer Pulse Simulator, please install Qiskit.${ColorReset}")
    endif()
else()
    message(STATUS "${BoldYellow}Python interpreter or development headers not found. Aer Pulse Simulator is disabled.${ColorReset}")
endif()

if(APPLE)
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "@loader_path/../lib")
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
  set_target_properties(${LIBRARY_NAME}
                        PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
  set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-rdynamic -shared")
endif()

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
