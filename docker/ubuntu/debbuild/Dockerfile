from ubuntu:latest
maintainer Alex McCaskey <mccaskeyaj@ornl.gov>
run apt-get -y upgrade && apt-get -y update && apt-get install -y software-properties-common \
   python-software-properties libgmp-dev libmpfr-dev ruby ruby-dev rubygems && add-apt-repository ppa:ubuntu-toolchain-r/test \
   && apt-get -y update && apt-get -y install libboost-all-dev git make libtool cmake gcc-6 g++-6 mpich \
   && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6 \ 
   && rm -rf /var/lib/apt/lists/* \ 
   && gem install --no-ri --no-rdoc fpm 

run apt-get -y update && apt-get -y install wget && wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb && \
    apt-get -y update && apt-get -y install $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | \
    sed "s/.*ends:\ //" | sed 's/,//g') && dpkg -i scaffold_2.0_amd64.deb \
    && apt-get -y install libssl-dev libspdlog-dev && git clone --recursive https://github.com/ORNL-QCI/xacc \
    && cd xacc && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/xacc_install && make && make install

run cd /xacc && mkdir deb_build && cd deb_build && mkdir -p usr/local/xacc && cp -r /xacc/build/xacc_install/* usr/local/xacc/ \
   && fpm -s dir -t deb --name xacc --version 1.0 --depends libboost-all-dev --depends gcc-6 --depends g++-6 --depends mpich \ 
   --depends libspdlog-dev --depends cmake --depends git --depends make --depends libtool --description "XACC - eXtreme-scale ACCelerator programming framework" . && cp *.deb /
