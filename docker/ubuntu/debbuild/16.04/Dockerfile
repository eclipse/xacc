from mccaskey/xacc-xenial-dev
maintainer Alex McCaskey <mccaskeyaj@ornl.gov>
run apt-get -y update && apt-get -y install ruby ruby-dev rubygems \ 
    && gem install --no-ri --no-rdoc fpm 

run mkdir deb_build && cd deb_build && mkdir -p usr/local/xacc \ 
    && cp -r /usr/local/xacc/* usr/local/xacc/ \ 
    && fpm -s dir -t deb --name xacc-openmpi --version 0.0.1 --depends libboost-all-dev \ 
        --depends gcc-6 --depends g++-6 --depends openmpi --depends cmake \ 
        --depends git --depends make --depends libtool \ 
        --description "XACC - eXtreme-scale ACCelerator programming framework" . && cp *.deb /
