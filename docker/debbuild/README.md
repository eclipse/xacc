# update so we can get gcc 6
apt-get -y upgrade && apt-get -y update && apt-get install -y software-properties-common && add-apt-repository ppa:ubuntu-toolchain-r/test
# install xacc deps
apt-get -y update && apt-get install -y $(dpkg --info xacc_1.0_amd64.deb | grep Depends | sed "s/.*ends:\ //" | sed 's/,//g') git cmake libspdlog-dev
