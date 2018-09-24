FIND_PACKAGE(Doxygen)
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)
ADD_CUSTOM_TARGET(doxygen
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generating API documentation with Doxygen"
)
