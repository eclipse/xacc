FROM xacc/manylinux1_x86_64

ARG python_version_str
ARG python_version

RUN curl -sL https://github.com/squeaky-pl/centos-devtools/releases/download/6.2/gcc-6.2.0-binutils-2.27-x86_64.tar.bz2 | tar xvj

ENV CC=/opt/devtools-6.2/bin/gcc
ENV CXX=/opt/devtools-6.2/bin/g++

RUN rm -rf /usr/lib64/libstdc++.so.6* && mkdir -p all_wheels && yum remove -y gcc \
    && ln -s /opt/devtools-6.2/lib64/libstdc++* /usr/lib64/

RUN git clone --recursive https://github.com/eclipse/xacc \
    && cd xacc \
    && export myPython=/opt/python/${python_version_str}/bin/python \
    && $myPython -m pip install ipopo configparser numpy scipy \
    && export xaccVersion=$($myPython -c "import os; print(open(os.path.join(os.getcwd(), 'VERSION')).read().strip())") \
    && $myPython --version \
    && $myPython setup.py build -t tmp \
    && echo "./xacc" >> build/lib.linux-x86_64-${python_version}/xacc.pth \
    && $myPython setup.py bdist_wheel --skip-build \
    && /opt/_internal/cpython-3.6.4/bin/python tools/wheels/fix_xacc_rpaths.py -v repair dist/xacc-$xaccVersion-${python_version_str}-linux_x86_64.whl \
    && cp wheelhouse/xacc-$xaccVersion-${python_version_str}-manylinux1_x86_64.whl /all_wheels

RUN export PATH=$PATH:/opt/devtools-6.2/bin \
    && cd / && yum install -y blas-devel lapack-devel \
    && git clone --recursive https://github.com/ornl-qci/tnqvm && cd tnqvm \
    && export myPython=/opt/python/${python_version_str}/bin/python \
    && export tnqvmVersion=$($myPython -c "import os; print(open(os.path.join(os.getcwd(), 'VERSION')).read().strip())") \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py build -t tmp \
    && mkdir -p build/lib.linux-x86_64-${python_version}/xacc/lib \
    && cp /usr/lib64/liblapack.so.3 build/lib.linux-x86_64-${python_version}/xacc/lib/ \
    && cp /usr/lib64/libblas.so.3 build/lib.linux-x86_64-${python_version}/xacc/lib/ \
    && cp /usr/lib64/libgfortran.so.1 build/lib.linux-x86_64-${python_version}/xacc/lib/ \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py bdist_wheel --skip-build \
    && /opt/_internal/cpython-3.6.4/bin/python /xacc/tools/wheels/null_rpaths.py -v repair dist/tnqvm-$tnqvmVersion-${python_version_str}-linux_x86_64.whl \
    && cp wheelhouse/tnqvm-$tnqvmVersion-${python_version_str}-manylinux1_x86_64.whl /all_wheels

RUN export PATH=$PATH:/opt/devtools-6.2/bin \
    && cd / && git clone --recursive https://github.com/ornl-qci/xacc-rigetti && cd xacc-rigetti \
    && export myPython=/opt/python/${python_version_str}/bin/python \
    && export rigettiVersion=$($myPython -c "import os; print(open(os.path.join(os.getcwd(), 'VERSION')).read().strip())") \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py build -t tmp \
    && mkdir -p build/lib.linux-x86_64-${python_version}/xacc/lib \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py bdist_wheel --skip-build \
    && /opt/_internal/cpython-3.6.4/bin/python /xacc/tools/wheels/null_rpaths.py -v repair dist/xacc_rigetti-$rigettiVersion-${python_version_str}-linux_x86_64.whl \
    && cp wheelhouse/xacc_rigetti-$rigettiVersion-${python_version_str}-manylinux1_x86_64.whl /all_wheels

RUN export PATH=$PATH:/opt/devtools-6.2/bin \
    && cd / && git clone --recursive https://github.com/ornl-qci/xacc-dwave && cd xacc-dwave \
    && export myPython=/opt/python/${python_version_str}/bin/python \
    && export dwaveVersion=$($myPython -c "import os; print(open(os.path.join(os.getcwd(), 'VERSION')).read().strip())") \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py build -t tmp \
    && mkdir -p build/lib.linux-x86_64-${python_version}/xacc/lib \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py bdist_wheel --skip-build \
    && /opt/_internal/cpython-3.6.4/bin/python /xacc/tools/wheels/null_rpaths.py -v repair dist/xacc_dwave-$dwaveVersion-${python_version_str}-linux_x86_64.whl \
    && cp wheelhouse/xacc_dwave-$dwaveVersion-${python_version_str}-manylinux1_x86_64.whl /all_wheels

RUN export PATH=$PATH:/opt/devtools-6.2/bin \
    && cd / && git clone --recursive https://github.com/ornl-qci/xacc-ibm && cd xacc-ibm \
    && export myPython=/opt/python/${python_version_str}/bin/python \
    && export ibmVersion=$($myPython -c "import os; print(open(os.path.join(os.getcwd(), 'VERSION')).read().strip())") \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py build -t tmp \
    && mkdir -p build/lib.linux-x86_64-${python_version}/xacc/lib \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py bdist_wheel --skip-build \
    && /opt/_internal/cpython-3.6.4/bin/python /xacc/tools/wheels/null_rpaths.py -v repair dist/xacc_ibm-$ibmVersion-${python_version_str}-linux_x86_64.whl \
    && cp wheelhouse/xacc_ibm-$ibmVersion-${python_version_str}-manylinux1_x86_64.whl /all_wheels

RUN export PATH=$PATH:/opt/devtools-6.2/bin \
    && cd / && git clone --recursive https://github.com/ornl-qci/xacc-vqe && cd xacc-vqe \
    && export myPython=/opt/python/${python_version_str}/bin/python \
    && export vqeVersion=$($myPython -c "import os; print(open(os.path.join(os.getcwd(), 'VERSION')).read().strip())") \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py build -t tmp \
    && mkdir -p build/lib.linux-x86_64-${python_version}/xacc/lib \
    && PYTHONPATH=/xacc/build/lib.linux-x86_64-${python_version}/xacc \
           $myPython setup.py bdist_wheel --skip-build \
    && /opt/_internal/cpython-3.6.4/bin/python /xacc/tools/wheels/null_rpaths.py -v repair dist/xacc_vqe-$vqeVersion-${python_version_str}-linux_x86_64.whl \
    && cp wheelhouse/xacc_vqe-$vqeVersion-${python_version_str}-manylinux1_x86_64.whl /all_wheels

