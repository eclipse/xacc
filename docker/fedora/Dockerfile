from mccaskey/base-fedora-gcc6
run dnf -y install spdlog llvm-devel libstdc++-static clang-devel \
    https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm \
    && git clone --recursive https://github.com/ORNL-QCI/xacc \
    && rm -rf /projects/xacc/tpls/fire/.git \
    && git clone https://github.com/google/fruit && mkdir fruit/build && cd fruit/build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4 && make install \
    && echo "export PATH=$PATH:/usr/local/scaffold" >> ~/.bashrc \
    && source /etc/bashrc && module load mpi/mpich-x86_64 && cd /projects/xacc && mkdir build \
    && cd build && cmake .. && make && PATH=$PATH:/usr/local/scaffold ctest
