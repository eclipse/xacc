ExternalProject_Add(cppmicroservices
	GIT_REPOSITORY https://github.com/cppmicroservices/cppmicroservices
	GIT_TAG v3.1.0	
        CMAKE_ARGS -DCMAKE_CXX_FLAGS='-Wimplicit-fallthrough=0'
        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/cppus_install
)
