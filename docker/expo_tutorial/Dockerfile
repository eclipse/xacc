from xacc/base 
label maintainer="Alex McCaskey <mccaskeyaj@ornl.gov>"

run rm /bin/sh && ln -s /bin/bash /bin/sh && apt-get -y update && apt-get -y install vim gfortran libblas-dev liblapack-dev pkg-config libhdf5-dev \
    && python3 -m pip install --upgrade pip xacc && python3 -m pip install jupyterlab pyquil mpmath deepdiff \
       numpy scipy openfermion xacc-rigetti xacc-vqe tnqvm xacc-ibm --no-cache-dir && ipython3 kernel install \
    && python3 -m pip uninstall -y h5py && python3 -m pip install h5py==2.8.0rc1 \
    && git clone https://github.com/amccaskey/OpenFermion-Psi4 && cd OpenFermion-Psi4 && python3 -m pip install --user . \
    && cd / && git clone --recursive https://github.com/ornl-qci/xacc-vqe-bayesopt \
    && cd xacc-vqe-bayesopt && mkdir build && cd build && cmake .. -DXACC_DIR=$(python3 -m pyxacc -L) \
    && apt-get install -y apt-transport-https ca-certificates curl software-properties-common \
    && curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - \
    && add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic test" \
    && apt-get update -y && apt-get install -y docker-ce \
    && sed -i 's/os.rename/shutil.move/g' /usr/local/lib/python3.6/dist-packages/openfermion/hamiltonians/_molecular_data.py \
    && sed -i 's/import os/import os, shutil/g' /usr/local/lib/python3.6/dist-packages/openfermion/hamiltonians/_molecular_data.py

run cd / && git clone https://github.com/psi4/psi4 && cd psi4 && mkdir build && cd build \
    && cmake .. -DPYTHON_EXECUTABLE=$(which python3) -DCMAKE_INSTALL_PREFIX=$(python3 -m site --user-site)/psi4 \
    && make -j8 install \
    && echo "./psi4" >> /root/.local/lib/python3.6/site-packages/psi4.pth \
    && echo "export PATH=$PATH:$(python3 -m site --user-site)/psi4/bin" >> ~/.bashrc

run echo "alias python=python3" >> ~/.bashrc \
    && echo "function xacc-logs() { docker logs -f \$(docker ps -aqf name=expo) ; }" >> ~/.bashrc
