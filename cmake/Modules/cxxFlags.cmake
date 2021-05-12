include(CheckCXXCompilerFlag)
# Add a compile flag if the compiler supports
function(add_compile_flags_if_supported flag)
    check_cxx_compiler_flag("${flag}" "XACC_COMPILER_SUPPORTS_${flag}_FLAG")
    if (XACC_COMPILER_SUPPORTS_${flag}_FLAG)
        add_compile_options(${flag})    
    endif()
endfunction()