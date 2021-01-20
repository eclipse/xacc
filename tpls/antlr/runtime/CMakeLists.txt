
include_directories(
  src
  src/atn
  src/dfa
  src/misc
  src/support
  src/tree
  src/tree/pattern
  src/tree/xpath
)


file(GLOB libantlrcpp_SRC
  "src/*.cpp"
  "src/atn/*.cpp"
  "src/dfa/*.cpp"
  "src/misc/*.cpp"
  "src/support/*.cpp"
  "src/tree/*.cpp"
  "src/tree/pattern/*.cpp"
  "src/tree/xpath/*.cpp"
)

add_library(antlr4_shared SHARED ${libantlrcpp_SRC})

set(LIB_OUTPUT_DIR "${CMAKE_HOME_DIRECTORY}/dist") # put generated libraries here.
message(STATUS "Output libraries to ${LIB_OUTPUT_DIR}")

# make sure 'make' works fine even if ${LIB_OUTPUT_DIR} is deleted.
add_custom_target(make_lib_output_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_OUTPUT_DIR}
    )

add_dependencies(antlr4_shared make_lib_output_dir)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_libraries(antlr4_shared ${UUID_LIBRARIES})
elseif(APPLE)
  target_link_libraries(antlr4_shared ${COREFOUNDATION_LIBRARY})
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(disabled_compile_warnings "/wd4251")
else()
  set(disabled_compile_warnings "-Wno-overloaded-virtual")
endif ()


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-dollar-in-identifier-extension -Wno-four-char-constants")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
  set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-multichar")
endif()

set(extra_share_compile_flags "")
if (WIN32)
  set(extra_share_compile_flags "-DANTLR4CPP_EXPORTS")
endif(WIN32)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(antlr4_shared PRIVATE "/MD$<$<CONFIG:Debug>:d>")
endif()

set(static_lib_suffix "")
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(static_lib_suffix "-static")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(extra_share_compile_flags "-DANTLR4CPP_EXPORTS -MP /wd4251")
endif()

set_target_properties(antlr4_shared
                      PROPERTIES VERSION   ${ANTLR_VERSION}
                                 SOVERSION ${ANTLR_VERSION}
                                 OUTPUT_NAME antlr4-runtime
                                 LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
                                 LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIB_OUTPUT_DIR}
                                 LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIB_OUTPUT_DIR}
                                 # TODO: test in windows. DLL is treated as runtime.
                                 # see https://cmake.org/cmake/help/v3.0/prop_tgt/LIBRARY_OUTPUT_DIRECTORY.html
                                 RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
                                 ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
                                 COMPILE_FLAGS "${disabled_compile_warnings} ${extra_share_compile_flags}")

install(TARGETS antlr4_shared
        DESTINATION lib)

install(DIRECTORY "src/" 
        DESTINATION "include/antlr4-runtime"
        COMPONENT dev 
        FILES_MATCHING PATTERN "*.h"
        )



