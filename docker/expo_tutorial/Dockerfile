FROM xacc/xacc-python-install

LABEL maintainer="Alex McCaskey <mccaskeyaj@ornl.gov>"

# Replace shell with bash so we can source files
RUN rm /bin/sh && ln -s /bin/bash /bin/sh && apt-get -y update && apt-get -y install vim
 
RUN python3 -m pip install --upgrade pip && python3 -m pip install jupyterlab pyquil numpy scipy openfermion --no-cache-dir && ipython3 kernel install

run apt-get install -y gfortran libblas-dev liblapack-dev libhdf5-dev
# && git clone https://github.com/SebWouters/CheMPS2 && cd CheMPS2 && mkdir build && cd build && cmake .. && make -j4 install && cd /

run git clone https://github.com/amccaskey/OpenFermion-Psi4 && cd OpenFermion-Psi4 && python3 -m pip install --user .
run git clone https://github.com/ornl-qci/xacc-rigetti \
    && git clone https://github.com/ornl-qci/xacc-ibm \
    && git clone https://github.com/ornl-qci/xacc-vqe \ 
    && git clone https://github.com/ornl-qci/tnqvm \
    && cd xacc-rigetti && python3 -m pip install --user . \
    && cd ../xacc-ibm && python3 -m pip install --user .
run apt-get install -y pkg-config && cd /xacc-vqe && python3 -m pip install --user . \
    && cd ../tnqvm && python3 -m pip install --user . 
run echo "export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH" >> ~/.bashrc
