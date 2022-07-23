from ubuntu:20.04
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get -y update && apt-get install -y gcc g++ gfortran gdb git wget \
            libcurl4-openssl-dev libssl-dev python3 libunwind-dev \
            clang-tools clangd clang-tidy \
            libpython3-dev python3-pip libblas-dev liblapack-dev software-properties-common \
            build-essential curl vim gpg gnupg libsecret-1-dev libsecret-1-0 xz-utils && \
    python3 -m pip install --upgrade pip --user \
    && python3 -m pip install setuptools \
    && python3 -m pip install python-language-server flake8 autopep8 \
    cmake ipopo pint numpy scipy pydantic qiskit pylint pyquil cirq matplotlib openfermion pyscf  \
    qsearch qfactor scikit-quant myqlm qlmaas && \       
    rm -rf /var/lib/apt/lists/*

# Install Node.js
RUN curl -sL https://deb.nodesource.com/setup_16.x | bash \
    && apt-get install nodejs -yq \
    # smoke tests
    && node --version \
    && npm --version

# Use "latest" or "next" version for Theia packages
ARG version=latest
# Optionally build a striped Theia application with no map file or .ts sources.
# Makes image ~150MB smaller when enabled
ARG strip=true

ENV YARN_VERSION=1.22.19 \
    strip=$strip

# Install Yarn
# From: https://github.com/nodejs/docker-node
RUN set -ex \
  && for key in \
    6A010C5166006599AA17F08146C2130DFD2497F5 \
  ; do \
    gpg --batch --keyserver hkps://keys.openpgp.org --recv-keys "$key" || \
    gpg --batch --keyserver keyserver.ubuntu.com --recv-keys "$key" ; \
  done \
  && curl -fsSLO --compressed "https://yarnpkg.com/downloads/$YARN_VERSION/yarn-v$YARN_VERSION.tar.gz" \
  && curl -fsSLO --compressed "https://yarnpkg.com/downloads/$YARN_VERSION/yarn-v$YARN_VERSION.tar.gz.asc" \
  && gpg --batch --verify yarn-v$YARN_VERSION.tar.gz.asc yarn-v$YARN_VERSION.tar.gz \
  && mkdir -p /opt \
  && tar -xzf yarn-v$YARN_VERSION.tar.gz -C /opt/ \
  && ln -s /opt/yarn-v$YARN_VERSION/bin/yarn /usr/local/bin/yarn \
  && ln -s /opt/yarn-v$YARN_VERSION/bin/yarnpkg /usr/local/bin/yarnpkg \
  && rm yarn-v$YARN_VERSION.tar.gz.asc yarn-v$YARN_VERSION.tar.gz \
  # smoke test
  && yarn --version

# Theia application
WORKDIR /home/theia
ADD $version.package.json package.json

RUN if [ "$strip" = "true" ]; then \
yarn --pure-lockfile && \
    NODE_OPTIONS="--max_old_space_size=4096" yarn theia build && \
    yarn theia download:plugins && \
    yarn --production && \
    yarn autoclean --init && \
    echo *.ts >> .yarnclean && \
    echo *.ts.map >> .yarnclean && \
    echo *.spec.* >> .yarnclean && \
    yarn autoclean --force && \
    yarn cache clean \
;else \
yarn --cache-folder ./ycache && rm -rf ./ycache && \
     NODE_OPTIONS="--max_old_space_size=4096" yarn theia build && yarn theia download:plugins \
;fi


WORKDIR /home/dev
RUN cd /home/dev && git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. -DXACC_BUILD_EXAMPLES=TRUE \
    && make -j$(nproc) install \
    && cd ../../ && git clone https://github.com/ornl-qci/tnqvm && cd tnqvm && mkdir build && cd build \
    && cmake .. -DXACC_DIR=/root/.xacc && make -j$(nproc) install 

EXPOSE 3000
ENV SHELL=/bin/bash \
    THEIA_DEFAULT_PLUGINS=local-dir:/home/theia/plugins \
    PYTHONPATH="${PYTHONPATH}:/root/.xacc"

ENTRYPOINT [ "node", "/home/theia/src-gen/backend/main.js", "/home/dev", "--hostname=0.0.0.0" ]