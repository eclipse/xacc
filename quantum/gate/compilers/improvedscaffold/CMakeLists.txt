#***********************************************************************************
# Copyright (c) 2016, UT-Battelle
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of the xacc nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Contributors:
#   Initial API and implementation - Alex McCaskey
#
#**********************************************************************************/
set (PACKAGE_NAME "ImprovedScaffold XACC Runtime")
set (PACKAGE_DESCIPTION "ImprovedScaffold XACC Programming Framework")
set (LIBRARY_NAME xacc-improved-scaffold)

file (GLOB HEADERS *.hpp)
file (GLOB SRC *.cpp)

set(CMAKE_CXX_FLAGS "-D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O3 -w -fomit-frame-pointer -fvisibility-inlines-hidden -fPIC -Woverloaded-virtual -Wcast-qual")

set_source_files_properties(ScaffoldASTConsumer.cpp PROPERTIES COMPILE_FLAGS -fno-rtti)

include_directories(${CMAKE_SOURCE_DIR}/quantum/gate)
include_directories(/usr/local/scaffold/include)
include_directories(/usr/local/scaffold/build/include)
include_directories(/usr/local/scaffold/build/tools/clang/include)

add_library(${LIBRARY_NAME} SHARED ${SRC})

install(FILES ${HEADERS} DESTINATION include)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)

link_directories("/usr/local/scaffold/build/Release+Asserts/lib")

# Gather tests
file (GLOB test_files tests/*.cpp)
add_tests("${test_files}" "/usr/local/scaffold/include;/usr/local/scaffold/build/include;${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/quantum/gate/utils" "${LIBRARY_NAME};xacc-gateqir;clangTooling;clangFrontendTool;clangFrontend;clangDriver;clangSerialization;clangCodeGen;clangParse;clangSema;clangStaticAnalyzerFrontend;clangStaticAnalyzerCheckers;clangStaticAnalyzerCore;clangAnalysis;clangARCMigrate;clangEdit;clangAST;clangLex;clangBasic;LLVMAsmParser;LLVMInstrumentation;LLVMLinker;LLVMArchive;LLVMBitReader;LLVMDebugInfo;LLVMJIT;LLVMipo;LLVMVectorize;LLVMBitWriter;LLVMTableGen;LLVMHexagonCodeGen;LLVMHexagonDesc;LLVMHexagonInfo;LLVMHexagonAsmPrinter;LLVMPTXCodeGen;LLVMPTXDesc;LLVMPTXInfo;LLVMPTXAsmPrinter;LLVMMBlazeDisassembler;LLVMMBlazeAsmParser;LLVMMBlazeCodeGen;LLVMMBlazeDesc;LLVMMBlazeInfo;LLVMMBlazeAsmPrinter;LLVMCppBackendCodeGen;LLVMCppBackendInfo;LLVMMSP430CodeGen;LLVMMSP430Desc;LLVMMSP430Info;LLVMMSP430AsmPrinter;LLVMXCoreCodeGen;LLVMXCoreDesc;LLVMXCoreInfo;LLVMCellSPUCodeGen;LLVMCellSPUDesc;LLVMCellSPUInfo;LLVMMipsDisassembler;LLVMMipsAsmParser;LLVMMipsCodeGen;LLVMMipsDesc;LLVMMipsInfo;LLVMMipsAsmPrinter;LLVMARMDisassembler;LLVMARMAsmParser;LLVMARMCodeGen;LLVMARMDesc;LLVMARMInfo;LLVMARMAsmPrinter;LLVMPowerPCCodeGen;LLVMPowerPCDesc;LLVMPowerPCInfo;LLVMPowerPCAsmPrinter;LLVMSparcCodeGen;LLVMSparcDesc;LLVMSparcInfo;LLVMX86AsmParser;LLVMX86Disassembler;LLVMX86CodeGen;LLVMSelectionDAG;LLVMAsmPrinter;LLVMX86Desc;LLVMX86Info;LLVMX86AsmPrinter;LLVMX86Utils;gtest_main;gtest;LLVMMCDisassembler;LLVMMCParser;LLVMInterpreter;LLVMCodeGen;LLVMScalarOpts;LLVMInstCombine;LLVMTransformUtils;LLVMipa;LLVMAnalysis;LLVMMCJIT;LLVMRuntimeDyld;LLVMExecutionEngine;LLVMTarget;LLVMMC;LLVMObject;LLVMCore;LLVMSupport;dl;pthread")

