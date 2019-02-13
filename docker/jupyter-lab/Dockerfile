from xacc/base
label maintainer="Alex McCaskey <mccaskeyaj@ornl.gov>"

run rm /bin/sh && ln -s /bin/bash /bin/sh && apt-get -y update && apt-get -y install libcurl4-openssl-dev vim gfortran libblas-dev liblapack-dev pkg-config libhdf5-dev ipython3

run python3 -m pip install --upgrade pip && python3 -m pip install --user jupyterlab pyquil mpmath deepdiff prompt-toolkit==1.0.15 dwave-qbsolv dwave-ocean-sdk \
       numpy scipy openfermion --no-cache-dir && ipython3 kernel install \
    && python3 -m pip uninstall -y h5py && python3 -m pip install h5py==2.8.0rc1 
run apt-get update && apt-get install -y apt-transport-https ca-certificates curl software-properties-common \
    && curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - \
    && add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic test" \
    && apt-get update -y && apt-get install -y docker-ce \
    && sed -i 's/os.rename/shutil.move/g' ~/.local/lib/python3.6/site-packages/openfermion/hamiltonians/_molecular_data.py \
    && sed -i 's/import os/import os, shutil/g' ~/.local/lib/python3.6/site-packages/openfermion/hamiltonians/_molecular_data.py

run git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DPYTHON_INCLUDE_DIR=/usr/include/python3.6 && make -j4 install && cd ../.. \
    && git clone https://github.com/ornl-qci/tnqvm && cd tnqvm && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DXACC_DIR=~/.xacc && make -j4 install && cd ../.. \
    && git clone https://github.com/ornl-qci/xacc-rigetti && cd xacc-rigetti && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DXACC_DIR=~/.xacc && make -j4 install && cd ../.. \
    && git clone --recursive https://github.com/ornl-qci/xacc-dwave && cd xacc-dwave && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DXACC_DIR=~/.xacc && make -j4 install && cd ../.. \
    && git clone --recursive https://github.com/ornl-qci/xacc-ibm && cd xacc-ibm && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DXACC_DIR=~/.xacc && make -j4 install && cd ../.. \
    && git clone https://github.com/ornl-qci/xacc-vqe && cd xacc-vqe && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DXACC_DIR=~/.xacc -DPYTHON_INCLUDE_DIR=/usr/include/python3.6 && make -j4 install && cd ../.. \
    && git clone --recursive https://github.com/ornl-qci/xacc-vqe-bayesopt && cd xacc-vqe-bayesopt && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DXACC_DIR=~/.xacc && make -j4 install && cd ../.. \
    && git clone https://github.com/ornl-qci/xacc-projectq && cd xacc-projectq && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DXACC_DIR=~/.xacc && make -j4 install && cd ../.. \
    && rm -rf xacc/build xacc-rigetti/build tnqvm/build xacc-ibm/build xacc-projectq/build xacc-vqe/build xacc-vqe-bayesopt/build \
             xacc-vqe-fcidump/build

run python3 -m pip install ipopo && echo "alias python=python3" >> ~/.bashrc 
