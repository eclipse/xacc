from fedora:26
run mkdir -p /projects
workdir /projects
run dnf install -y gcc-c++ openssl-devel cpprest-devel \ 
    boost-devel python-devel git make unzip cmake \ 
    lapack-devel blas-devel libtool \
    https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc26.x86_64.rpm
run ls && git clone https://github.com/ornl-qci/xacc && cd xacc \
    && mkdir build && cd build && cmake .. && make install 
run echo "export PATH=/usr/local/xacc/bin:$PATH" >> /root/.bashrc \
    && source ~/.bashrc && cd /projects && mkdir plugin-installs \ 
    && cd plugin-installs \ 
    && xacc-install-plugins.py -j 4 -p xacc-python tnqvm xacc-rigetti xacc-dwave \
    xacc-ibm xacc-scaffold 
add dwave_sapi.h /usr/local/include/
add libdwave_sapi.so /usr/local/lib/
run source ~/.bashrc && xacc-install-plugins.py -j4 -p xacc-dwsapi-embedding \
    && echo "export PYTHONPATH=/usr/local/xacc/lib/python:$PYTHONPATH" >> /root/.bashrc
cmd ['while', 'true',';','do','sleep','1',';','done']
