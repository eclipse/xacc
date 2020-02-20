set(LIBRARY_NAME xacc-optimal-control)
# This is preliminary, we'll figure out how to organize this.
file(GLOB SRC *.cpp)

add_library(${LIBRARY_NAME} SHARED ${SRC})
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 14)
target_include_directories(${LIBRARY_NAME} PUBLIC . ${CMAKE_SOURCE_DIR}/tpls/eigen ${CMAKE_SOURCE_DIR}/tpls/exprtk ./includes/LBFGSpp/include)

add_subdirectory(tests)