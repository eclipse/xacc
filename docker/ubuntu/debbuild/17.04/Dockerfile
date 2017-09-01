from mccaskey/xacc-zesty-dev
maintainer Alex McCaskey <mccaskeyaj@ornl.gov>
run apt-get -y update && apt-get install -y ruby ruby-dev rubygems \ 
   && gem install --no-ri --no-rdoc fpm 

run mkdir deb_build && cd deb_build && mkdir -p usr/local/xacc \ 
   && cp -r /usr/local/xacc/* usr/local/xacc/ \
   && fpm -s dir -t deb --name xacc --version 1.0 --depends libboost-all-dev \ 
        --depends gcc-6 --depends g++-6 --depends python-dev --depends openmpi \ 
        --depends cmake --depends git --depends make --depends libtool \ 
        --description "XACC - eXtreme-scale ACCelerator programming framework" . && cp *.deb /
