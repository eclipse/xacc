from xacc/ci-base
run git clone --recursive https://github.com/eclipse/xacc && cd xacc && mkdir build && cd build \ 
   && cmake .. -DXACC_BUILD_TESTS=TRUE -DPYTHON_INCLUDE_DIR=$(python3 -c "import sysconfig; print(sysconfig.get_paths()['platinclude'])") \
   && make -j$(python3 -c "import multiprocessing; print(multiprocessing.cpu_count())") install && ctest \
   && PYTHONPATH=/usr/local python3 -c "import pyxacc; pyxacc.Initialize()"
