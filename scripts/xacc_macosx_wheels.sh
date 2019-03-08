#!/bin/bash

set -e

mkdir -p $HOME/wheelhouse

eval "$(pyenv init -)"
eval "$(pyenv virtualenv-init -)"

function clone {
	git clone --recursive https://github.com/ornl-qci/$1
}

git clone --recursive https://github.com/eclipse/xacc
clone xacc-rigetti
clone xacc-ibm
clone xacc-vqe
clone tnqvm
clone xacc-dwave

for version in 3.4.7 3.5.4 3.6.4 3.7.0
do
	pyenv virtualenv $version xacc-$version
	pyenv activate xacc-$version
	python --version
 	python -m pip install --upgrade pip
	python -m pip install wheel ipopo configparser
	export libPath=$(python -c "import distutils.util; print(distutils.util.get_platform())")
	echo $libPath
	cd xacc
	export ver=`case $version in "3.6.4") echo 3.6 ;; "3.5.4") echo 3.5 ;; "3.5.0") echo 3.5 ;; "3.4.7") echo 3.4 ;; "3.3.7") echo 3.3 ;; "2.7.14") echo 2.7 ;; *) echo "invalid";; esac`
    export verstr=`case $ver in "3.6") echo "cp36-cp36m" ;; "3.5") echo "cp35-cp35m" ;; "3.4") echo "cp34-cp34m" ;; "3.3") echo "cp33-cp33m" ;; "2.7") echo "cp27-cp27mu" ;; *) echo "invalid";; esac`

	# ------------------- XACC BUILD ------------------#
	python setup.py build -t tmp_build --executable="/usr/bin/env python"
	export buildPath=build/lib.$libPath-$ver
	echo "./xacc" >> build/lib.$libPath-$ver/xacc.pth
	python setup.py bdist_wheel --skip-build
	mv dist/*.whl $HOME/wheelhouse/

    export prefix="build\/lib."
    export suffix="-$ver"
    export arch=$(echo $libPath | sed -e "s/^$prefix//" -e "s/$suffix$//" | sed -e 's/-/_/g' | sed -e 's/\./_/g')
    echo $arch
	echo $verstr
	which python

	export xaccdir=$(pwd)

	# ---------------- RIGETTI BUILD -------------------#
	cd ../xacc-rigetti
    PYTHONPATH=../xacc/$buildPath/xacc python setup.py build -t tmp_build --executable="/usr/bin/env python"

	python setup.py bdist_wheel --skip-build
	mv dist/*.whl $HOME/wheelhouse/

	# ---------------- DWAVE BUILD -------------------#
	cd ../xacc-dwave
    PYTHONPATH=../xacc/$buildPath/xacc python setup.py build -t tmp_build --executable="/usr/bin/env python"

	python setup.py bdist_wheel --skip-build
	mv dist/*.whl $HOME/wheelhouse/

	# ---------------- IBM BUILD -------------------#
	cd ../xacc-ibm
    PYTHONPATH=../xacc/$buildPath/xacc python setup.py build -t tmp_build --executable="/usr/bin/env python"

    python setup.py bdist_wheel --skip-build
	mv dist/*.whl $HOME/wheelhouse/

	# ---------------- TNQVM BUILD -------------------#
	cd ../tnqvm
    PYTHONPATH=../xacc/$buildPath/xacc python setup.py build -t tmp_build --executable="/usr/bin/env python"

    python setup.py bdist_wheel --skip-build
	mv dist/*.whl $HOME/wheelhouse/

	# ---------------- VQE BUILD -------------------#
	cd ../xacc-vqe
    PYTHONPATH=../xacc/$buildPath/xacc python setup.py build -t tmp_build --executable="/usr/bin/env python"

    python setup.py bdist_wheel --skip-build
	mv dist/*.whl $HOME/wheelhouse/

	cd ..
	#python -m pip uninstall -y xacc
	source deactivate
	pyenv uninstall -f xacc-$version
done

