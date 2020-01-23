from xacc/ubuntu:18.04
run git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. -DXACC_BUILD_TESTS=TRUE -DXACC_BUILD_EXAMPLES=TRUE \
    && make -j18 install && PYTHONPATH=$HOME/.xacc ctest --output-on-failure
