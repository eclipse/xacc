set(LIBRARY_NAME xacc-optimal-control)

# This is preliminary, we'll figure out how to organize this.
file(GLOB SRC *.cpp)

add_library(${LIBRARY_NAME} SHARED ${SRC})
target_include_directories(${LIBRARY_NAME} PUBLIC . ${CMAKE_SOURCE_DIR}/tpls/eigen)