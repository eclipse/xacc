if(NOT CLANG_FORMAT_EXECUTABLE)
        find_program(CLANG_FORMAT_EXECUTABLE
           NAMES
           clang-format-7.0
           clang-format-mp-7.0
           clang-format
        )
        if(CLANG_FORMAT_EXECUTABLE)
                message("-- Found clang-format: ${CLANG_FORMAT_EXECUTABLE}")
        else()
                message("-- clang-format not found! 'format' target not built. Install clang-format to enable formatting.")
        endif()
else()
   message("-- Using clang-format: ${CLANG_FORMAT_EXECUTABLE}")
   if(NOT EXISTS ${CLANG_FORMAT_EXECUTABLE})
        message(FATAL_ERROR "-- clang-format path is invalid!")
   else()
        add_custom_target(format
              COMMAND
                  CLANG_FORMAT_EXE=${CLANG_FORMAT_EXECUTABLE}
                  ${CMAKE_SOURCE_DIR}/scripts/check_format_cpp.sh --apply-patch
              WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
   endif()
endif()
