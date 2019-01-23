from xacc/base-ci
run git clone --recursive https://github.com/eclipse/xacc \
    && cd xacc && mkdir build && cd build \
    && cmake .. -DPYTHON_INCLUDE_DIR=/usr/include/python3.5 && make -j4 install
run apt-get update -y && apt-get install -y libblas-dev liblapack-dev && python3 -m pip install ipopo configparser numpy scipy
