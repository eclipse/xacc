from xacc/xacc
workdir home/dev
run git clone -b maint https://bitbucket.org/petsc/petsc petsc && cd petsc \
    && export PETSC_DIR=${PWD} && export PETSC_ARCH=linux-gnu-c-complex \
    && ./configure --with-scalar-type=complex --download-mpich --download-fblaslapack=1 \
                   --with-debugging=no COPTFLAGS=-O3 CXXOPTFLAGS=-O3 FOPTFLAGS=-O3 --with-64-bit-indices \
    && make PETSC_DIR=${PETSC_DIR} PETSC_ARCH=${PETSC_ARCH} all \
    && cd /home/dev && git clone -b xacc-integration https://github.com/ORNL-QCI/QuaC.git \
    && cd QuaC && mkdir build && cd build \
    && cmake .. -DPETSC_DIR=${PETSC_DIR} -DPETSC_ARCH=linux-gnu-c-complex -DXACC_DIR=~/.xacc \
    && make install

ENV SHELL=/bin/bash \
    THEIA_DEFAULT_PLUGINS=local-dir:/home/dev/plugins
ENV PYTHONPATH "${PYTHONPATH}:/root/.xacc"
ENTRYPOINT [ "yarn", "theia", "start", "/home/dev", "--hostname=0.0.0.0" ]