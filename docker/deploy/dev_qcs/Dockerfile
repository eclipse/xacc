from xacc/deploy-base
workdir /home/dev
run cd /home/dev && apt-get update && apt-get install -y uuid-dev pkg-config \
    && git clone https://github.com/zeromq/libzmq \
    && cd libzmq/ && mkdir build && cd build \
    && cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq \
    && make -j$(nproc) install \
    && cd ../.. \
    && git clone https://github.com/zeromq/cppzmq \
    && cd cppzmq/ && mkdir build && cd build/ \
    && cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq -DCMAKE_PREFIX_PATH=~/.zmq \
    && make -j$(nproc) install \
    && cd ../.. \
    && git clone -b c_master https://github.com/msgpack/msgpack-c/ \
    && cd msgpack-c/ && mkdir build && cd build \
    && cmake .. -DCMAKE_INSTALL_PREFIX=~/.zmq \
    && make -j$(nproc) install \
    && cd ../.. \
    && git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. \
    && make -j$(nproc) install \
    && cd ../../ && git clone https://github.com/ornl-qci/tnqvm && cd tnqvm && mkdir build && cd build \
    && cmake .. -DXACC_DIR=~/.xacc && make -j$(nproc) install && cd ../.. \
    && git clone https://github.com/rigetti/qcs-cli && cd qcs-cli \
    && npm install && npm link

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

