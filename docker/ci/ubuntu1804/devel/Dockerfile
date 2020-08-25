from xacc/ubuntu:18.04
run git clone --depth 1 --recursive -b xacc-devel https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. -DXACC_BUILD_TESTS=TRUE \
    && make -j$(nproc) install && PYTHONPATH=$HOME/.xacc ctest --output-on-failure
