from ubuntu:16.04

run mkdir /projects
workdir /projects
# Build XACC and its Dependencies, Scaffold included
run apt-get -y upgrade && apt-get -y update && apt-get install -y software-properties-common wget && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && wget http://eclipseice.ornl.gov/downloads/xacc/debs/16.04/xacc_1.0_amd64.deb \
    && wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb \
    && apt-get -y update && apt-get install -y $(dpkg --info xacc_1.0_amd64.deb | grep Depends | sed "s/.*ends:\ //" | sed 's/,//g') \
    && apt-get -y update && apt-get -y install $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | sed "s/.*ends:\ //" | sed 's/,//g') \
    && dpkg -i scaffold_2.0_amd64.deb && dpkg -i xacc_1.0_amd64.deb
