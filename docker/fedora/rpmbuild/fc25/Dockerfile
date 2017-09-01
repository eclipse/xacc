from mccaskey/xacc-fc25-dev
maintainer Alex McCaskey <mccaskeyaj@ornl.gov>
run dnf install -y https://github.com/genereese/togo/releases/download/v2.3r7/togo-2.3-7.noarch.rpm 
run mkdir rpm_build && cd rpm_build && togo configure -n "Alex McCaskey" -e "mccaskeyaj@ornl.gov" \
   && togo project create xacc && cd xacc && mkdir -p root/usr/local/xacc \ 
   && cp -r /usr/local/xacc/* root/usr/local/xacc/ && togo file exclude root/usr/local/xacc \
   && sed -i -r "s/(Version: *).*/\11.0/" spec/header && sed -i -r "s/(Summary: *).*/\1XACC - eXtreme-scale ACCelerator programming environment./" spec/header \
   && sed -i -r "s/(Release: *).*/\1mpich_devel.fc25/" spec/header && sed -i -r "s/(License: *).*/\1BSD/" spec/header \
   && sed -i -r "s/(Buildarch: *).*/\1x86_64/" spec/header && sed -i -r "s/(Group: *).*/\1Development\/System/" spec/header \
   && sed -i -r "s/(Requires: *).*/\1gcc-c++ cmake git make libtool mpich-devel boost-mpich-devel/" spec/header && sed -i -r '/Requires/s/^#//g' spec/header && togo build package && cp rpms/*.rpm /projects/ \
   && cat spec/header
