from xacc/master-ci
from xacc/integration_base_ubuntu:18.04
copy --from=0 /root/.xacc /root/.xacc
copy --from=0 /xacc/build/quantum /quantum
copy --from=0 /xacc/python/examples /pyexamples
add .ibm_config /root/
run python3 -m pip install pydantic --user \
    && git clone https://github.com/ornl-qci/tnqvm \
    && cd tnqvm && mkdir build && cd build \
    && cmake .. -DXACC_DIR=~/.xacc && make -j12 install \
    && cd ../../ \
    && export PYTHONPATH=$(python3 -m site --user-site)/psi4/lib:$HOME/.xacc:$PYTHONPATH \
    && export LD_LIBRARY_PATH=/root/.xacc/lib \
    && quantum/examples/base_api/bell_quil_ibm_local \
    && quantum/examples/base_api/bell_xasm_ibm_local \
    && quantum/examples/base_api/deuteron_2qbit_xasm_X0X1 \
    && quantum/examples/base_api/deuteron_2qbit_ir_api_X0X1 \
    && quantum/examples/base_api/circuit_optimization \
    && quantum/examples/base_api/circuit_with_classical_control \
    && quantum/examples/qasm/ddcl_example \
    && quantum/examples/qasm/deuteron_from_qasm \
    && quantum/examples/qasm/deuteron_from_qasm_vqe_algo \
    && quantum/examples/qasm/deuteronH3_from_qasm_vqe_algo \
    && quantum/examples/qasm/nah_ucc3 \
    && python3 pyexamples/bell_aer_readout_error.py \
    && python3 pyexamples/ddcl_2qbit_example.py \
    && python3 pyexamples/ddcl_decorator.py \
    && python3 pyexamples/ddcl_example.py \
    && python3 pyexamples/deuteronH2.py \
    && python3 pyexamples/nah_vqe_ucc1.py \
    && python3 pyexamples/nah_vqe_ucc3.py \
    && python3 pyexamples/psi4_h2.py \
    && python3 pyexamples/readout_error_correction_aer.py \
    && python3 pyexamples/test_qiskit_cx_cancellation.py

