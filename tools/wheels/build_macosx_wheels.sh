#!/bin/bash

mkdir -p $HOME/xacc-wheelhouse
eval "$(pyenv init -)"
eval "$(pyenv virtualenv-init -)"

for version in 3.6.4 #2.7.14-ucs2 2.7.14-ucs4 3.3.7 3.4.7 3.5.4 3.6.4
do
	pyenv virtualenv $version xacc-$version
	pyenv activate xacc-$version
	python --version
 	python -m pip install --upgrade pip
	python -m pip install wheel
	export libPath=$(python -c "import distutils.util; print(distutils.util.get_platform())")
	echo $libPath
	git clone --recursive -b mccaskey/cpr_build https://github.com/eclipse/xacc
	cd xacc
	export ver=`case $version in "3.6.4") echo 3.6 ;; "3.5.4") echo 3.5 ;; "3.5.0") echo 3.5 ;; "3.4.7") echo 3.4 ;; "3.3.7") echo 3.3 ;; "2.7.14") echo 2.7 ;; *) echo "invalid";; esac`
	python setup.py build -t tmp_build --executable="/usr/bin/env python"
	echo "./xacc" >> build/lib.$libPath-$ver/xacc.pth
	install_name_tool -change libcpr.dylib @rpath/libcpr.dylib build/lib.$libPath-$ver/xacc/lib/libxacc.dylib
	install_name_tool -change $PWD/tmp_build/lib/libcpr.dylib @rpath/libcpr.dylib build/lib.$libPath-$ver/xacc/pyxacc.so
	install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-gate.dylib
	install_name_tool -change libboost_filesystem.dylib @rpath/libboost_filesystem.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-gate.dylib
	install_name_tool -change libboost_program_options.dylib @rpath/libboost_program_options.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-gate.dylib
	install_name_tool -change libboost_regex.dylib @rpath/libboost_regex.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-gate.dylib
	install_name_tool -change libboost_chrono.dylib @rpath/libboost_chrono.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-gate.dylib
	install_name_tool -change libboost_graph.dylib @rpath/libboost_graph.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-gate.dylib
	install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.$libPath-$ver/xacc/lib/libboost_filesystem.dylib
	install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.$libPath-$ver/xacc/lib/libboost_chrono.dylib
	install_name_tool -change libboost_regex.dylib @rpath/libboost_regex.dylib build/lib.$libPath-$ver/xacc/lib/libboost_graph.dylib
	install_name_tool -change libboost_system.dylib @rpath/libboost_system.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-aqc.dylib
	install_name_tool -change libboost_filesystem.dylib @rpath/libboost_filesystem.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-aqc.dylib
	install_name_tool -change libboost_program_options.dylib @rpath/libboost_program_options.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-aqc.dylib
	install_name_tool -change libboost_regex.dylib @rpath/libboost_regex.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-aqc.dylib
	install_name_tool -change libboost_chrono.dylib @rpath/libboost_chrono.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-aqc.dylib
	install_name_tool -change libboost_graph.dylib @rpath/libboost_graph.dylib build/lib.$libPath-$ver/xacc/lib/libxacc-quantum-aqc.dylib
	python setup.py bdist_wheel --skip-build
	mv dist/*.whl $HOME/xacc-wheelhouse
	source deactivate
	#rm -rf xacc
done

