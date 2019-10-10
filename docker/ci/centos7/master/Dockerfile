from xacc/centos:7
run git clone -b master https://github.com/eclipse/xacc \
    && cd xacc && mkdir build && cd build \
    && cmake .. -DPYTHON_INCLUDE_DIR=/usr/include/python3.6m -DXACC_BUILD_TESTS=TRUE
    && make -j4 install && ctest