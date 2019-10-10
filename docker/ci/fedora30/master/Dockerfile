from xacc/fedora:30
run git clone -b master https://github.com/eclipse/xacc \
    && cd xacc/ && mkdir build && cd build/ \
    && cmake .. -DXACC_BUILD_TESTS=TRUE -DPYTHON_INCLUDE_DIR=/usr/include/python3.7m -DXACC_BUILD_EXAMPLES=TRUE \
    && make -j4 install && ctest