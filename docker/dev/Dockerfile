from xacc/base
run git clone --recursive https://github.com/eclipse/xacc && cd xacc \ 
   && mkdir build && cd build && cmake .. -DPYTHON_INCLUDE_DIR=$(python3 -c "import sysconfig; print(sysconfig.get_paths()['platinclude'])") -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cp compile_commands.json .. \
   && cd /projects && git clone https://github.com/ornl-qci/xacc-rigetti \
   && git clone https://github.com/ornl-qci/xacc-ibm \ 
   && git clone https://github.com/ornl-qci/xacc-vqe \
   && git clone https://github.com/ornl-qci/tnqvm
