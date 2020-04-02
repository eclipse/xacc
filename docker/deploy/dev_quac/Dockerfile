from xacc/deploy-base
workdir /home/dev
run cd /home/dev && git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. \
    && make -j$(nproc) install \
    && cd ../../ && git clone https://github.com/ornl-qci/tnqvm && cd tnqvm && mkdir build && cd build \
    && cmake .. -DXACC_DIR=~/.xacc && make -j$(nproc) install \
    && cd /home/dev && git clone -b maint https://bitbucket.org/petsc/petsc petsc && cd petsc \
    && export PETSC_DIR=${PWD} && export PETSC_ARCH=linux-gnu-c-complex \
    && ./configure --with-scalar-type=complex --download-mpich --download-fblaslapack=1 \
                   --with-debugging=no COPTFLAGS=-O3 CXXOPTFLAGS=-O3 FOPTFLAGS=-O3 --with-64-bit-indices \
    && make PETSC_DIR=${PETSC_DIR} PETSC_ARCH=${PETSC_ARCH} all \
    && cd /home/dev && git clone -b xacc-integration https://github.com/ORNL-QCI/QuaC.git \
    && cd QuaC && mkdir build && cd build \
    && cmake .. -DPETSC_DIR=${PETSC_DIR} -DPETSC_ARCH=linux-gnu-c-complex -DXACC_DIR=~/.xacc \
    && make install

# Theia application
workdir /home/dev
ARG version=latest
ADD $version.package.json ./package.json

RUN yarn --cache-folder ./ycache && rm -rf ./ycache && \
    NODE_OPTIONS="--max_old_space_size=4096" yarn theia build ;\
    yarn theia download:plugins
EXPOSE 3000
ENV SHELL=/bin/bash \
    THEIA_DEFAULT_PLUGINS=local-dir:/home/dev/plugins
ENV PYTHONPATH "${PYTHONPATH}:/root/.xacc:/root/.local/lib/python3.6/site-packages/psi4/lib"
ENTRYPOINT [ "yarn", "theia", "start", "/home/dev", "--hostname=0.0.0.0" ]

