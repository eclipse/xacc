## Instructions for building XACC on the QCS

# build libzmq
git clone https://github.com/zeromq/libzmq
cd libzmq/ && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq
make -j12 install

# Now go back to top-level and pull/build cppzmq
cd ../..
git clone https://github.com/zeromq/cppzmq
cd cppzmq/ && mkdir build && cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq -DCMAKE_PREFIX_PATH=~/.zmq
make -j12 install

# Now go back to top-level and pull/build msgpack
cd ../..
git clone https://github.com/msgpack/msgpack-c/
cd msgpack-c/ && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq
make -j12 install
cd ../..

# Install uuid-dev
sudo yum install uuid-dev

# Build XACC
git clone --recursive https://github.com/eclipse/xacc
cd xacc && mkdir build && cd build
cmake .. -DPYTHON_INCLUDE_DIR=/usr/include/python3.6m -DUUID_LIBRARY=/usr/lib64/libuuid.so.1
make -j4 install
