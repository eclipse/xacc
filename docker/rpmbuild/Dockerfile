from fedora:latest
maintainer Alex McCaskey <mccaskeyaj@ornl.gov>
run mkdir -p /projects
workdir /projects
run dnf install -y https://github.com/genereese/togo/releases/download/v2.3r7/togo-2.3-7.noarch.rpm \
   gcc-c++ cmake mpich-devel boost-mpich-devel make git wget \
   https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm \ 
   spdlog && git clone --recursive https://github.com/ORNL-QCI/xacc \ 
   && source /etc/bashrc && module load mpi/mpich-x86_64 && cd xacc && mkdir build \
   && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/xacc_install && make install
run cd /projects/xacc && mkdir rpm_build && cd rpm_build && togo configure -n "Alex McCaskey" -e "mccaskeyaj@ornl.gov" \
   && togo project create xacc && cd xacc && mkdir -p root/usr/local/xacc \
   && cp -r /projects/xacc/build/xacc_install/* root/usr/local/xacc/ && togo file exclude root/usr/local/xacc \
   && sed -i -r "s/(Version: *).*/\11.0/" spec/header && sed -i -r "s/(Summary: *).*/\1XACC - eXtreme-scale ACCelerator programming environment./" spec/header \
   && sed -i -r "s/(Release: *).*/\11.fc25/" spec/header && sed -i -r "s/(License: *).*/\1BSD/" spec/header \
   && sed -i -r "s/(Buildarch: *).*/\1x86_64/" spec/header && sed -i -r "s/(Group: *).*/\1Development\/System/" spec/header \
   && sed -i -r "s/(Requires: *).*/\1gcc-c++ cmake git make libtool spdlog mpich-devel boost-mpich-devel/" spec/header \
   && sed -i -r '/Requires/s/^#//g' spec/header \
   && togo build package && cp rpms/*.rpm /projects/ \
   && cat spec/header
