from xacc/xacc
workdir /home/dev
run git clone --recursive https://github.com/ornl-qci/exatn && cd exatn && mkdir build && cd build \
     && cmake .. -DCMAKE_BUILD_TYPE=Release -DEXATN_BUILD_TESTS=TRUE -DPYTHON_INCLUDE_DIR=$(python3 -c "import sysconfig; print(sysconfig.get_paths()['platinclude'])") \
           -DBLAS_LIB=ATLAS -DBLAS_PATH=/usr/lib/x86_64-linux-gnu \
     && make -j$(nproc) install \
     && cd /home/dev/tnqvm/build && rm -rf * && cmake .. -DXACC_DIR=$HOME/.xacc -DEXATN_DIR=$HOME/.exatn \
     && make -j$(nproc) install

workdir /home/dev
ARG version=latest
ADD $version.package.json ./package.json

RUN yarn --cache-folder ./ycache && rm -rf ./ycache && \
    NODE_OPTIONS="--max_old_space_size=4096" yarn theia build ;\
    yarn theia download:plugins
EXPOSE 3000
ENV SHELL=/bin/bash \
    THEIA_DEFAULT_PLUGINS=local-dir:/home/dev/plugins
ENV PYTHONPATH "${PYTHONPATH}:/root/.xacc:$(python3 -m site --user-site)/psi4/lib"
ENTRYPOINT [ "yarn", "theia", "start", "/home/dev", "--hostname=0.0.0.0" ]

