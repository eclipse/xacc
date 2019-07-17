from xacc/ubuntu:18.04
run git clone --recursive -b xacc-devel https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. -DPYTHON_INCLUDE_DIR=/usr/include/python3.6m -DXACC_BUILD_TESTS=TRUE \
    && make -j$(nproc) install && ctest --output-on-failure
