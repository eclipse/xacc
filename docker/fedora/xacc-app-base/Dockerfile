from fedora:25
run mkdir -p /projects
workdir /projects
run dnf install -y python-devel gcc-c++ openssl-devel cmake \
    gdb mpich-devel boost-mpich-devel gmp-devel make git spdlog \
    http://eclipseice.ornl.gov/downloads/xacc/rpms/x86_64/xacc-1.0-1.fc25.x86_64.rpm \
    https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm 

