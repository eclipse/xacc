#!/bin/bash

install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-gate.dylib
install_name_tool -change libboost_filesystem.dylib @rpath/libboost_filesystem.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-gate.dylib
install_name_tool -change libboost_program_options.dylib @rpath/libboost_program_options.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-gate.dylib
install_name_tool -change libboost_regex.dylib @rpath/libboost_regex.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-gate.dylib
install_name_tool -change libboost_chrono.dylib @rpath/libboost_chrono.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-gate.dylib
install_name_tool -change libboost_graph.dylib @rpath/libboost_graph.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-gate.dylib
install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libboost_filesystem.dylib
install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libboost_chrono.dylib
install_name_tool -change libboost_regex.dylib @rpath/libboost_regex.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libboost_graph.dylib
install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-aqc.dylib
install_name_tool -change libboost_filesystem.dylib @rpath/libboost_filesystem.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-aqc.dylib
install_name_tool -change libboost_program_options.dylib @rpath/libboost_program_options.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-aqc.dylib
install_name_tool -change libboost_regex.dylib @rpath/libboost_regex.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-aqc.dylib
install_name_tool -change libboost_chrono.dylib @rpath/libboost_chrono.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-aqc.dylib
install_name_tool -change libboost_graph.dylib @rpath/libboost_graph.dylib build/lib.macosx-10.6-intel-3.6/xacc/lib/libxacc-quantum-aqc.dylib
