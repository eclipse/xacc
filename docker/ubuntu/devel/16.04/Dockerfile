from mccaskey/base-ubuntu16-04-gcc6
run wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb && \
    apt-get -y update && apt-get -y install $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | \
    sed "s/.*ends:\ //" | sed 's/,//g') && dpkg -i scaffold_2.0_amd64.deb \
    && apt-get -y install python-dev libssl-dev libspdlog-dev \
    && git clone https://github.com/cppmicroservices/cppmicroservices && cd cppmicroservices && mkdir build && cd build \
    && cmake .. && make install && cd /projects \
    && git clone --recursive https://github.com/ORNL-QCI/xacc \
    && cd xacc && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/xacc && make && make install && ctest
