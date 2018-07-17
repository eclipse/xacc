from theiaide/theia-full:next
run apt-get -y update \ 
    && unlink /usr/bin/clangd \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" > \ 
                 /etc/apt/sources.list.d/llvm.list \
    && apt-get -y update && apt-get install -y clang-tools-7 cmake libssl-dev \
              python3 libpython3-dev python3-pip vim gdb gfortran libblas-dev \
              liblapack-dev pkg-config \ 
    && ln -s /usr/bin/clangd-7 /usr/bin/clangd \
    && ln -s /usr/bin/clang++-7 /usr/bin/clang++ \
    && ln -s /usr/bin/clang-7 /usr/bin/clang \
    && git clone --recursive https://github.com/eclipse/xacc \
    && cd xacc && mkdir build && cd build \
    && CC=clang CXX=clang++ cmake .. -DPYTHON_INCLUDE_DIR=/usr/include/python3.5 -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    && make -j4 install 
add settings.json /home/.theia/



