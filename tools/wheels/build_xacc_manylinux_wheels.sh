#!/bin/bash

export rpath_python=/opt/_internal/cpython-3.6.4/bin/python
git clone --recursive -b mccaskey/cpr_build https://github.com/eclipse/xacc
cd xacc

for version in cp36-cp36m cp35-cp35m cp34-cp34m cp33-cp33m cp27-cp27m cp27-cp27mu
do
        export myPython=/opt/python/$version/bin/python
        $myPython build -t tmp_build --executable="/usr/bin/env python"
        export ver=`case $version in "cp36-cp36m") echo 3.6 ;; "cp35-cp35m") echo 3.5 ;; "cp34-cp34m") echo 3.4 ;; "cp33-cp33m") echo 3.3 ;; "cp27-cp27m") echo 2.7 ;; "cp27-cp27mu") echo 2.7 ;; *) echo "invalid";; esac`
       $myPython setup.py build -t tmp_build --executable="/usr/bin/env python"
       echo "./xacc" >> build/lib.linux-x86_64-$ver/xacc.pth
       $myPython setup.py bdist_wheel --skip-build
       $rpath_python tools/wheels/fix_xacc_rpaths.py -v repair dist/xacc-0.1.0-$version-linux_x86_64.whl
done
