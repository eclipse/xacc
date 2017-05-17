from mccaskey/base-ubuntu-gcc6
run wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb && \
    apt-get -y update && apt-get -y install $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | \
    sed "s/.*ends:\ //" | sed 's/,//g') && dpkg -i scaffold_2.0_amd64.deb \
    && apt-get -y install libspdlog-dev && git clone --recursive https://github.com/ORNL-QCI/xacc \
    && cd xacc && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/xacc && make && make install && ctest
