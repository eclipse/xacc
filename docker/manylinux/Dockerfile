FROM xacc/manylinux1_x86_64

ADD CentOS-Vault.repo /etc/yum.repos.d/
RUN echo 'exclude = *.i?86' >> /etc/yum.conf
RUN echo /usr/local/lib > /etc/ld.so.conf.d/local.conf && echo /usr/local/lib64 >> /etc/ld.so.conf.d/local.conf
ENV PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:/usr/local/lib/pkgconfig

ARG NUM_CPU=2

RUN curl https://ftp.gnu.org/gnu/binutils/binutils-2.29.1.tar.bz2 | tar xj \
    && mkdir build && cd build && ../binutils-2.29.1/configure \
    && make -j $NUM_CPU && make install && cd .. && rm -rf build 

RUN curl https://gmplib.org/download/gmp/gmp-6.1.2.tar.bz2 | tar xj \
    && mkdir build && cd build && ../gmp-6.1.2/configure \
    && make -j $NUM_CPU install && cd .. && rm -rf build 

RUN curl https://ftp.gnu.org/gnu/mpfr/mpfr-3.1.6.tar.bz2 | tar xj \
    && mkdir build && cd build && ../mpfr-3.1.6/configure \
    && make -j $NUM_CPU install && cd .. && rm -rf build

RUN curl http://ftp.gnu.org/gnu/mpc/mpc-1.0.3.tar.gz | tar xz \
    && mkdir build && cd build && ../mpc-1.0.3/configure \
    && make -j $NUM_CPU install && cd .. && rm -rf build

RUN curl http://mirrors-usa.go-parts.com/gcc/releases/gcc-5.5.0/gcc-5.5.0.tar.gz | tar xz 
run mkdir build && cd build \
    && ../gcc-5.5.0/configure --enable-languages=c,c++,fortran --disable-multilib \
    && make -j $NUM_CPU && make install \
    && strip /usr/local/libexec/gcc/x86_64-unknown-linux-gnu/5.5.0/* || true \
    && yum remove -y binutils libstdc++-devel \
    && cd /usr/local/bin && ln -s gcc cc 
CMD ["/bin/bash"]
