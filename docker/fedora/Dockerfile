from mccaskey/base-fedora-gcc6
run dnf -y install llvm-devel clang-devel https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm \
    && git clone --recursive https://github.com/ORNL-QCI/xacc && git clone --recursive https://github.com/ORNL-QCI/common && \
    rm -rf /projects/xacc/tpls/common/tpls/fire/.git && rm -rf /projects/xacc/tpls/common/.git && rm -rf /projects/common/tpls/fire/.git \
    && echo "export PATH=$PATH:/usr/local/scaffold" >> ~/.bashrc \
    && source /etc/bashrc && module load mpi/mpich-x86_64 && cd /projects/xacc && mkdir build \
    && cd build && cmake .. && make && PATH=$PATH:/usr/local/scaffold ctest
