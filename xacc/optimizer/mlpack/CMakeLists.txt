if(EXISTS ${XACC_ENSMALLEN_INCLUDE_DIR}/ensmallen.hpp)
  if(EXISTS ${XACC_ARMADILLO_INCLUDE_DIR}/armadillo)
    message(
        STATUS "${BoldGreen}Building MLPack Optimizer.${ColorReset}")
    set(LIBRARY_NAME xacc-mlpack)

    file(GLOB
         SRC
         mlpack_optimizer.cpp
         mlpack_activator.cpp)

    usfunctiongetresourcesource(TARGET
                                ${LIBRARY_NAME}
                                OUT
                                SRC)
    usfunctiongeneratebundleinit(TARGET
                                 ${LIBRARY_NAME}
                                 OUT
                                 SRC)

    add_library(${LIBRARY_NAME} SHARED ${SRC})

    target_include_directories(${LIBRARY_NAME}
                               PUBLIC . ${XACC_ENSMALLEN_INCLUDE_DIR}
                                      ${XACC_ARMADILLO_INCLUDE_DIR})

    target_link_libraries(${LIBRARY_NAME} PUBLIC xacc)

    set(_bundle_name xacc_mlpack)
    set_target_properties(${LIBRARY_NAME}
                          PROPERTIES COMPILE_DEFINITIONS
                                     US_BUNDLE_NAME=${_bundle_name}
                                     US_BUNDLE_NAME
                                     ${_bundle_name})

    usfunctionembedresources(TARGET
                             ${LIBRARY_NAME}
                             WORKING_DIRECTORY
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             FILES
                             manifest.json)

    if(APPLE)
      set_target_properties(${LIBRARY_NAME}
                            PROPERTIES INSTALL_RPATH "@loader_path/../lib")
      set_target_properties(${LIBRARY_NAME}
                            PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    else()
      set_target_properties(${LIBRARY_NAME}
                            PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
      set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-shared")
    endif()

    if(XACC_BUILD_TESTS)
      add_subdirectory(tests)
    endif()

    install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

  endif()
endif()
