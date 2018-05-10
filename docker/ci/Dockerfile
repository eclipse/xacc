from xacc/ci-base
run git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \ 
   && cmake .. -DXACC_BUILD_TESTS=TRUE -DPYTHON_INCLUDE_DIR=$(python3 -m pyxacc --python-include-dir) \
   && make -j4 install && ctest && PYTHONPATH=~/.xacc python3 -c "import pyxacc; pyxacc.Initialize()"
