from xacc/ubuntu:20.04
#run apt-add-repository "deb http://archive.ubuntu.com/ubuntu/ focal main restricted universe multiverse" && apt-get install -y libz3-dev
run git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. -DXACC_BUILD_TESTS=TRUE -DXACC_BUILD_EXAMPLES=TRUE 
    #&& make -j$(nproc) install && PYTHONPATH=$HOME/.xacc ctest --output-on-failure
