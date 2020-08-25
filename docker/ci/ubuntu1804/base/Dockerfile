FROM ubuntu:18.04
RUN apt-get -y update && apt-get --no-install-recommends -yq install gcc g++ git wget \
            libcurl4-openssl-dev libssl-dev python3 libunwind-dev \
            libpython3-dev python3-pip libblas-dev liblapack-dev software-properties-common \
    && python3 -m pip install ipopo cmake \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" > /etc/apt/sources.list.d/llvm.list \
    && apt-get update -y && apt-get --no-install-recommends -yq install libclang-9-dev llvm-9-dev \
    && ln -s /usr/bin/llvm-config-9 /usr/bin/llvm-config \
    && rm -rf /var/lib/apt/lists/* 
