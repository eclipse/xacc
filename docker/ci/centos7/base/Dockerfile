from centos:7
run yum install libcurl-devel python3-devel git centos-release-scl make \
    && yum install devtoolset-8-gcc devtoolset-8-gcc-c++ \
    && scl enable devtoolset-8 -- bash \
    && python3 -m pip install cmake