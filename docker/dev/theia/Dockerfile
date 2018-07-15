from theiaide/theia-full
run apt-get -y update \ 
    && unlink /usr/bin/clangd
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" > \ 
                 /etc/apt/sources.list.d/llvm.list \
    && apt-get -y update && apt-get install -y clang-tools-7 cmake libssl-dev \
              python3 libpython3-dev python3-pip vim gdb gfortran libblas-dev \
              liblapack-dev pkg-config \ 
    && ln -s /usr/bin/clangd-7 /usr/bin/clangd \
    && ln -s /usr/bin/clang++-7 /usr/bin/clang++ \
    && ln -s /usr/bin/clang-7 /usr/bin/clang 



