# Instructions for building XACC on the QCS from scratch

## Install uuid-dev, curl, and gcc 7
```bash
sudo yum install libuuid libuuid-devel curl-devel openssl-devel
sudo yum install centos-release-scl
sudo yum install devtoolset-7
sudo yum remove cmake
sudo python3 -m pip install --upgrade pip 
sudo python3 -m pip install cmake
export PATH=$PATH:/usr/local/bin [you might want to add this to .bashrc]
```

## Build libzmq
```bash
git clone https://github.com/zeromq/libzmq
cd libzmq/ && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq
make -j12 install
```

## Build cppzmq
Now go back to top-level and pull/build cppzmq
```bash
cd ../..
git clone https://github.com/zeromq/cppzmq
cd cppzmq/ && mkdir build && cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq -DCMAKE_PREFIX_PATH=~/.zmq
make -j12 install
```

## Build msgpack-c
Now go back to top-level and pull/build msgpack
```bash
cd ../..
git clone https://github.com/msgpack/msgpack-c/
cd msgpack-c/ && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq
make -j12 install
cd ../..
```


## Build XACC
```bash
scl enable devtoolset-7 bash
git clone --recursive https://github.com/eclipse/xacc
cd xacc && mkdir build && cd build
cmake .. -DUUID_LIBRARY=/usr/lib64/libuuid.so.1 -DUUID_INCLUDE_DIR=/usr/include 
make -j2 install
```
