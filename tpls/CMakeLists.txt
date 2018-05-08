
set(BUILD_SHARED_LIBS TRUE)
add_subdirectory(cppmicroservices)

set(USE_SYSTEM_CURL ON CACHE BOOL "" FORCE)
set(BUILD_CPR_TESTS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(cpr)

set(BOOST_LIBS_OPTIONAL program_options system filesystem regex graph CACHE STRING "" FORCE)
add_subdirectory(boost-cmake)

INSTALL( DIRECTORY "${CMAKE_SOURCE_DIR}/tpls/boost-cmake/boost/boost_1_64_0/boost" DESTINATION include )

