add_subdirectory(minor_graph_embedding)

# Note: TriQ depends on Z3 library. 
# Users need to install Z3, e.g.
# apt install z3
find_package(Z3)
if (NOT Z3_FOUND)
    message(WARNING "Z3 library not found. TriQ placement plugin will not be installed.")
else()
    add_subdirectory(triq)
endif()
