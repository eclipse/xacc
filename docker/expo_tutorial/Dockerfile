FROM xacc/xacc-python-install

LABEL maintainer="Alex McCaskey <mccaskeyaj@ornl.gov>"

# Replace shell with bash so we can source files
RUN rm /bin/sh && ln -s /bin/bash /bin/sh && apt-get -y update && apt-get -y install vim gfortran libblas-dev liblapack-dev pkg-config \
    && python3 -m pip install --upgrade pip && python3 -m pip install jupyterlab pyquil mpmath deepdiff \
       numpy scipy openfermion --no-cache-dir && ipython3 kernel install \
    && git clone https://github.com/amccaskey/OpenFermion-Psi4 && cd OpenFermion-Psi4 && python3 -m pip install --user .
run git clone https://github.com/ornl-qci/xacc-rigetti \
    && git clone https://github.com/ornl-qci/xacc-ibm \
    && git clone https://github.com/ornl-qci/xacc-vqe \ 
    && git clone https://github.com/ornl-qci/tnqvm \
    && cd xacc-rigetti && python3 -m pip install --user . \
    && cd ../xacc-ibm && python3 -m pip install --user . \
    && cd /xacc-vqe && python3 -m pip install --user . \
    && cd ../tnqvm && python3 -m pip install --user . 

run cd / && git clone https://github.com/psi4/psi4 && cd psi4 && mkdir build && cd build \
    && cmake .. -DPYTHON_EXECUTABLE=$(which python3) -DCMAKE_INSTALL_PREFIX=$(python3 -m site --user-site)/psi4 \
    && make -j8 install 
run python3 -m pip uninstall -y h5py && python3 -m pip install h5py==2.8.0rc1
run echo "./psi4" >> /root/.local/lib/python3.6/site-packages/psi4.pth \
    && echo "export PATH=$PATH:$(python3 -m site --user-site)/psi4/bin" >> ~/.bashrc
# hack - fix bug in openfermion... need to contribute back
# i think it has to do with volumes mounted via the docker container
run sed -i 's/os.rename/shutil.move/g' /usr/local/lib/python3.6/dist-packages/openfermion/hamiltonians/_molecular_data.py \
    && sed -i 's/import os/import os, shutil/g' /usr/local/lib/python3.6/dist-packages/openfermion/hamiltonians/_molecular_data.py
run apt-get install -y apt-transport-https ca-certificates curl software-properties-common \
    && curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - \
    && add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic test" 
run apt-get update -y && apt-get install -y docker-ce
run echo "alias python=python3" >> ~/.bashrc \
    && echo "function xacc-logs() { docker logs -f \$(docker ps -aqf name=expo) ; }" >> ~/.bashrc
