FROM xacc/base-ci
RUN git clone --recursive https://github.com/eclipse/xacc \
    && cd xacc && mkdir build && cd build \
    && cmake .. -DPYTHON_INCLUDE_DIR=/usr/include/python3.5 && make -j4 install
RUN apt-get update -y && apt-get --no-install-recommends -yq install libblas-dev liblapack-dev && python3 -m pip install ipopo configparser numpy scipy
