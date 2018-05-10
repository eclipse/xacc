from ubuntu:18.04
run apt-get -y update && apt-get install -y gcc g++ cmake git libssl-dev python3 libpython3-dev \
   && git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \ 
   && cmake .. -DXACC_BUILD_TESTS=TRUE -DPYTHON_INCLUDE_DIR=$(python3 -m pyxacc --python-include-dir) \
   && make -j4 install && ctest && PYTHONPATH=~/.xacc python3 -c "import pyxacc; pyxacc.Initialize()"
