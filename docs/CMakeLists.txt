FIND_PACKAGE(Sphinx)

ADD_CUSTOM_TARGET(docs ALL
    ${SPHINX_EXECUTABLE}
        -q -b html
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_SOURCE_DIR}/docs/build"
    COMMENT "Building HTML documentation with Sphinx"
)

#(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/html"
#    DESTINATION "${CMAKE_SOURCE_DIR}/docs/build/"
#)
