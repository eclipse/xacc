add_subdirectory(nlopt-optimizers)
add_subdirectory(mlpack)

if(XACC_BUILD_GSL)
add_subdirectory(gsl)
endif()
if(XACC_BUILD_SCIPY)
  execute_process(COMMAND ${Python_EXECUTABLE} -c "import scipy" RESULT_VARIABLE SCIPY_EXISTS)
  if(SCIPY_EXISTS EQUAL "1")
  # if not, check we have pip
  execute_process(COMMAND ${Python_EXECUTABLE} -c "import pip" RESULT_VARIABLE PIP_EXISTS)

  if(PIP_EXISTS EQUAL "0")
    # we have pip, so just install scipy
    message(STATUS "${BoldGreen}Installing Scipy.${ColorReset}")
    execute_process(COMMAND ${Python_EXECUTABLE} -m pip install scipy)
  else()
    # we dont have pip, so warn the user
    message(STATUS "${BoldYellow}Scipy not found, but can't install via pip. Ensure you install scipy module if you would like to use the Scipy optimizer.${ColorReset}")
  endif()
else()
  message(STATUS "${BoldGreen}Found Scipy.${ColorReset}")
endif()
  add_subdirectory(scipy)
else()
  message(STATUS "${BoldYellow}XACC will not build the Scipy optimizer. You can turn it on with -DXACC_BUILD_SCIPY=ON${ColorReset}")
endif()