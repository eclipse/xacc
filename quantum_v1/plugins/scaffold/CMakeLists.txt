option(SCAFFOLD_PATH "Path of scaffold install" "scaffcc")

if (SCAFFOLD_PATH)
  add_subdirectory(compiler)
#add_subdirectory(pragmas)
endif()