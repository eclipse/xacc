from mccaskey/base-fc26-gcc6
run dnf -y install openssl openssl-devel python-devel \
    https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc26.x86_64.rpm \
    && git clone https://github.com/cppmicroservices/cppmicroservices \
    && cd cppmicroservices && mkdir build && cd build && cmake .. -DCMAKE_CXX_FLAGS='-Wimplicit-fallthrough=0' && make install && cd /projects \
    && git clone --recursive https://github.com/ORNL-QCI/xacc \
    && rm -rf /projects/xacc/tpls/fire/.git \ 
    && source /etc/bashrc && module load mpi/mpich-x86_64 && cd /projects/xacc && mkdir build \
    && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/xacc && make && make install && ctest
