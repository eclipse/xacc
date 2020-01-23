from xacc/ubuntu:18.04
run git clone https://github.com/psi4/psi4 && cd psi4 && mkdir build && cd build \
   && python3 -m pip install numpy qiskit scipy cirq pint cma --user \
   && cmake .. -DPYTHON_EXECUTABLE=$(which python3) -DCMAKE_INSTALL_PREFIX=$(python3 -m site --user-site)/psi4 \
   && make -j$(nproc) install
   